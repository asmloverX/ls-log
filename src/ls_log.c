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
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "../inc/ls_common.h"
#include "ls_circularbuffer.h"
#include "ls_writebase.h"
#include "ls_writelog.h"
#include "ls_ini.h"
#include "ls_array.h"
#include "ls_time.h"

#include "../inc/ls_log.h"



#define LS_CONFIGFILE     ("ls-log.conf") /*!< default configure filename */
#define LS_ARRAYLEN_DEF   (8)             /*!< default array length       */

struct ls_Log {
  struct ls_Array*    severityBuffers;   /*!< array of ls_SeverityBuffer */
  struct ls_Array*    logModules;        /*!< array of ls_Module         */
  struct ls_WriteLog* writeLog;
};


static struct ls_Log* g_sLog = NULL;


static void ls_getConfigFile(char* outFileName)
{
  if (NULL != outFileName)
  {
    char path[_MAX_PATH];
    if (0 != GetModuleFileNameA(NULL, path, _MAX_PATH))
    {
      strrchr(path, '\\')[1] = 0;
      sprintf(outFileName, "%s%s", path, LS_CONFIGFILE);
    }
  }
}
static int ls_logCreateModules(struct ls_Log* self)
{
  if (NULL == (self->severityBuffers = ls_arrayCreate(8)))
    return LS_Failed_CreateSeverityArray;

  if (NULL == (self->logModules = ls_arrayCreate(8)))
    return LS_Failed_CreateModulesArray;

  if (NULL == (self->writeLog = ls_writeLogCreate()))
      return LS_Failed_CreateWriteLog;
  
  return LS_Success;
}
static void ls_logReleaseModules(struct ls_Log* self)
{
  if (NULL != self->writeLog)
    ls_writeLogRelease(&self->writeLog);
  if (NULL != self->severityBuffers)
    ls_arrayRelease(&self->severityBuffers);
  if (NULL != self->logModules)
    ls_arrayRelease(&self->logModules);
}





static void ls_logRelease(struct ls_Log** self)
{
  if (NULL != *self)
  {
    ls_logReleaseModules(*self);
    free(*self);
    *self = NULL;
  }
}

static struct ls_Log* ls_logCreate(void)
{
  struct ls_Log* self;
  size_t size = sizeof(struct ls_Log);

  self = (struct ls_Log*)calloc(size, sizeof(char));
  if (NULL != self)
  {
    if (LS_Success != ls_logCreateModules(self))
      ls_logRelease(&self);
  }
  return self;
}

static int ls_logInit(struct ls_Log* self)
{
  if (NULL != self)
  {
    char   path[_MAX_PATH];
    struct ls_Ini* iniTool;
    int    i, count = 0, bufLen = LS_SEVERITYBUFLEN_DEF;

    ls_getConfigFile(path);
    if (NULL == (iniTool = ls_iniOpen(path)))
      return LS_Failed_LoadConfig;

    ls_iniGetInt(iniTool, "LS-LOG", "bufferLen", &bufLen);
    for (i = 0; i < Severity_Count; ++i)
    {
      struct ls_SeverityBuffer* buffer = (struct ls_SeverityBuffer*)malloc(sizeof(struct ls_SeverityBuffer));
      if (NULL == buffer)
        return LS_Failed;
      buffer->id     = i;
      buffer->buffer = ls_circularBufferCreate(bufLen);
      ls_arrayInsert(self->severityBuffers, buffer);
    }

    ls_iniGetInt(iniTool, "MODULES", "count", &count);
    for (i = 0; i < count; ++i)
    {
      char key[8];
      struct ls_Module* module = (struct ls_Module*)malloc(sizeof(struct ls_Module));

      if (NULL == module)
        return LS_Failed;
      sprintf(key, "%d", i);
      module->id = i;
      ls_iniGetStr(iniTool, "MODULES", key, sizeof(module->name), module->name);
      ls_arrayInsert(self->logModules, module);
    }
    ls_writeLogInit(self->writeLog, self->severityBuffers, self->logModules, bufLen);

    return LS_Success;
  }
  return LS_Failed_InvalidParameters;
}

static void __releaseElement(void* elem, void* arg)
{
  UNUSED_PARAM(arg);

  if (NULL != elem)
    free(elem);
}
static void ls_logUninit(struct ls_Log* self)
{
  if (NULL != self)
  {
    if (NULL != self->writeLog)
      ls_writeLogUninit(self->writeLog);

    if (NULL != self->severityBuffers)
    {
      ls_arrayForeach(self->severityBuffers, __releaseElement, NULL);
      ls_arrayClear(self->severityBuffers);
    }

    if (NULL != self->logModules)
    {
      ls_arrayForeach(self->logModules, __releaseElement, NULL);
      ls_arrayClear(self->logModules);
    }
  }
}





/*!< base functions */
static int __log_vsnprintf(char* dest, size_t count, const char* fmt, va_list ap)
{
#if defined(_MSC_VER) && (_MSC_VER < 1400)
  return _vsnprintf(dest, count, fmt, ap);
#else
  return vsnprintf(dest, count, fmt, ap);
#endif
}

static int __log_snprintf(char* dest, size_t count, const char* fmt, ...)
{
  int ret;
  va_list ap;

  va_start(ap, fmt);
  ret = __log_vsnprintf(dest, count, fmt, ap);
  va_end(ap);

  return ret;
}


/*!< outside interfaces */
int ls_init(void)
{
  ls_uninit();
  
  if (NULL != (g_sLog = ls_logCreate()))
    return ls_logInit(g_sLog);
  return LS_Failed;
}

void ls_uninit(void)
{
  if (NULL != g_sLog)
  {
    ls_logUninit(g_sLog);
    ls_logRelease(&g_sLog);
  }
}

int ls_logx(int display, int severity, int module, char* file, char* func, int line, const char* fmt, ...)
{
  if (NULL == g_sLog)
  {
    if (LS_Failed == ls_init())
      return LS_Failed;
  }

  if (Display_Console <= display && display < Display_Count && Severity_Debug <= severity 
    && severity < Severity_Count && 0 <= module && module < ls_arraySize(g_sLog->logModules) && NULL != fmt)
  {
    struct ls_LogBuffer llbuf;
    struct ls_LogTime   ltime;
    va_list ap;
    size_t  len;
    struct ls_SeverityBuffer* lsbuffer;

    ls_getLogTime(&ltime);
    llbuf.displayType = (unsigned char)display;
    llbuf.level       = (unsigned char)severity;
    llbuf.moduleIndex = (unsigned short)module;
    __log_snprintf(llbuf.message, sizeof(llbuf.message), "[%02d:%02d:%02d:%03d] %s-%s(%d) : ", 
        ltime.hour, ltime.min, ltime.sec, ltime.millitm, file, func, line);
    len = strlen(llbuf.message);
    va_start(ap, fmt);
    __log_vsnprintf(llbuf.message + len, sizeof(llbuf.message) - len, fmt, ap);
    va_end(ap);

    lsbuffer = (struct ls_SeverityBuffer*)ls_arrayGet(g_sLog->severityBuffers, severity);
    if (NULL != lsbuffer)
    {
      ls_circularBufferPush(lsbuffer->buffer, &llbuf);
      return LS_Success;
    }

    return LS_Failed;
  }
  return LS_Failed_InvalidParameters;
}
