/*
 * Copyright (c) 2012 ASMlove. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list ofconditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materialsprovided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#if _WIN32 || _WIN64
  #include <windows.h>
  #include <process.h>
#else
  /*!< TODO: */
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "ls_circularbuffer.h"
#include "../inc/ls_log.h"
#include "../inc/ls_common.h"
#include "ls_writebase.h"
#include "ls_array.h"
#include "ls_time.h"

#include "ls_writelog.h"



#define LS_WRITELOG_PERCENT   (0.8)   /*!< 80% percent */
#define LS_WRITELOG_STEPMIN   (5)     /*!< 5 minutes   */


/*!
 *! @struct ls_WriteLog
 *! @brief  wirteLog structure
 */
struct ls_WriteLog {
  struct ls_Array*      severityBuffers;              /*!< severity circular buffers array object   */
  struct ls_Array*      logModules;                   /*!< log module circular buffers array object */
  struct ls_WriteBase*  writeObjects[Display_Count];  /*!< 根据Display_Count的写日志对象数组        */
  int                   writeLogLen;                  /*!< 当buffer中的长度达到指定长度就写文件     */
  HANDLE                writeThread;                  /*!< WriteLog模块的线程对象句柄               */
  HANDLE                startEvent;                   /*!< 启动线程的启动事件句柄                   */
  HANDLE                exitEvent;                    /*!< 结束线程的时候结束事件对象句柄           */
};


static UINT WINAPI ls_writeLogThread(void* arg);


struct ls_WriteLog* ls_writeLogCreate(void)
{
  struct ls_WriteLog* self;
  size_t size = sizeof(struct ls_WriteLog);

  self = (struct ls_WriteLog*)calloc(size, sizeof(char));
  if (NULL != self)
  {
    if (NULL == (self->writeObjects[Display_Console] = ls_writeDefCreate()))
      ls_writeLogRelease(&self);
    if (NULL == (self->writeObjects[Display_GUI] = ls_writeGuiCreate()))
      ls_writeLogRelease(&self);
    if (NULL == (self->writeObjects[Display_File] = ls_writeFileCreate()))
      ls_writeLogRelease(&self);
  }
  return self;
}

void ls_writeLogRelease(struct ls_WriteLog** self)
{
  if (NULL != *self)
  {
    if (NULL != (*self)->writeObjects[Display_Console])
      ls_writeDefRelease(&(*self)->writeObjects[Display_Console]);
    if (NULL != (*self)->writeObjects[Display_GUI])
      ls_writeGuiRelease(&(*self)->writeObjects[Display_GUI]);
    if (NULL != (*self)->writeObjects[Display_File])
      ls_writeFileRelease(&(*self)->writeObjects[Display_File]);

    free(*self);
    *self = NULL;
  }
}

void ls_writeLogInit(struct ls_WriteLog* self, struct ls_Array* severityBuffers, struct ls_Array* logModules, int bufLen)
{
  if (NULL != self && NULL != severityBuffers && NULL != logModules)
  {
    int i;

    self->severityBuffers = severityBuffers;
    self->logModules      = logModules;
    self->writeLogLen     = (int)(LS_WRITELOG_PERCENT * (double)bufLen);
    if (self->writeLogLen < LS_BUFFERLEN_DEF)
      self->writeLogLen = LS_BUFFERLEN_DEF;
    for (i = 0; i < Display_Count; ++i)
    {
      if (NULL != self->writeObjects[i] && NULL != self->writeObjects[i]->init)
        self->writeObjects[i]->init(self->writeObjects[i], logModules);
    }

    self->startEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    assert(NULL != self->startEvent);
    self->exitEvent  = CreateEvent(NULL, TRUE, FALSE, NULL);
    assert(NULL != self->exitEvent);

    self->writeThread = (HANDLE)_beginthreadex(NULL, 0u, ls_writeLogThread, self, 0u, NULL);
    if (NULL != self->writeThread)
      WaitForSingleObject(self->startEvent, INFINITE);
    CloseHandle(self->startEvent);
  }
}

void ls_writeLogUninit(struct ls_WriteLog* self)
{
  if (NULL != self)
  {
    int i;
    if (NULL != self->writeThread)
    {
      SetEvent(self->exitEvent);
      if (WAIT_TIMEOUT == WaitForSingleObject(self->writeThread, INFINITE))
        TerminateThread(self->writeThread, ~0u);
      CloseHandle(self->writeThread);
    }
    CloseHandle(self->exitEvent);

    for (i = 0; i < Display_Count; ++i)
    {
      if (NULL != self->writeObjects[i] && NULL != self->writeObjects[i]->uninit)
        self->writeObjects[i]->uninit(self->writeObjects[i]);
    }
  }
}




static void ls_writeLogBuffer(struct ls_LogBuffer* llbuf, void* arg)
{
  if (NULL != llbuf && NULL != arg)
  {
    struct ls_WriteLog* self   = (struct ls_WriteLog*)arg;
    struct ls_Module*   module = (struct ls_Module*)ls_arrayGet(self->logModules, llbuf->moduleIndex);
    
    if (NULL == module)
      return;
    if (NULL != self->writeObjects[llbuf->displayType]->writeMessage)
      self->writeObjects[llbuf->displayType]->writeMessage(self->writeObjects[llbuf->displayType], 
        llbuf->level, module->id, llbuf->message, strlen(llbuf->message));
  }
}


/*!
 *! @enum LS_WriteLogByTimeType
 *! @brief 是否达到了将buffer中的数据写到文件的时间
 */
enum LS_WriteLogByTimeType {
  WriteLogByTime_No   = 0, /*!< NO  */
  WriteLogByTime_Yes  = 1, /*!< YES */
};
/*!
 *! @enum LS_WroteLogType
 *! @brief 日志是否已经写入文件
 */
enum LS_WroteLogType {
  WroteLog_No   = 0, /*!< NO  */
  WroteLog_Yes  = 1, /*!< YES */
};
static void ls_writeLogMessage(struct ls_WriteLog* self, int writeLogByTime)
{
  int i, buffersCount = ls_arraySize(self->severityBuffers);
  struct ls_SeverityBuffer* lsbuffer = NULL;
  int    wroteLog = WroteLog_No;

  for (i = 0; i < buffersCount; ++i)
  {
    lsbuffer = (struct ls_SeverityBuffer*)ls_arrayGet(self->severityBuffers, i);
    if (NULL == lsbuffer)
      return;
    if (self->writeLogLen <= ls_circularBufferDataLength(lsbuffer->buffer) || WriteLogByTime_Yes == writeLogByTime)
    {
      wroteLog = WroteLog_Yes;
      ls_circularBufferPopForeach(lsbuffer->buffer, ls_writeLogBuffer, self);
    }
  }

  if (WroteLog_Yes == wroteLog)
  {
    for (i = 0; i < Display_Count; ++i)
    {
      if (NULL != self->writeObjects[i] && NULL != self->writeObjects[i]->writeFlush)
        self->writeObjects[i]->writeFlush(self->writeObjects[i]);
    }
  }
}

static UINT WINAPI ls_writeLogThread(void* arg)
{
  struct ls_WriteLog* self = (struct ls_WriteLog*)arg;
  struct ls_LogTime   beg, end;
  int    writeLogByTime = WriteLogByTime_No;
  
  if (NULL == self)
    return ~0u;
  SetEvent(self->startEvent);

  ls_getLogTime(&beg);
  for (; ;)
  {
    if (WAIT_OBJECT_0 == WaitForSingleObject(self->exitEvent, 5))
      break;

    ls_getLogTime(&end);
    if (ls_timeDuration(beg, end))
    {
      writeLogByTime = WriteLogByTime_Yes;
      beg = end;
    }
    else
      writeLogByTime = WriteLogByTime_No;
    ls_writeLogMessage(self, writeLogByTime);
  }
  ls_writeLogMessage(self, WriteLogByTime_Yes);

  return 0;
}
