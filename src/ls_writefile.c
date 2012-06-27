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
#include <stdio.h>
#include <stdlib.h>
#if _WIN32 || _WIN64
  #include <io.h>
  #include <direct.h>
#else
  #include <unistd.h>
  #include <sys/stat.h>
  #include <sys/types.h>
#endif
#include "../inc/ls_common.h"
#include "../inc/ls_log.h"
#include "ls_array.h"
#include "ls_time.h"
#include "ls_writebase.h"


#define LS_WRITEFILE_FILE_DEF   ("./ls-log")  /*!< default logging system directory */


/*!
 *! @struct ls_WriteFile
 *! @brief  该结构是ls_WriteBase的继承, 定义了该类内部写文件相应的流streams
 *! @attention 其设计是根据severity来调用相应的streams[severity]来将日志信息写入对应的文件
 */
struct ls_ModuleStreams {
  FILE* streams[Severity_Count];  /*!< 文件流 */
};
struct ls_WriteFile {
  void* object;
  void (*init)(struct ls_WriteBase*, struct ls_Array* logModules);
  void (*uninit)(struct ls_WriteBase*);
  void (*writeMessage)(struct ls_WriteBase*, int, int, const char*, int);
  void (*writeFlush)(struct ls_WriteBase*);

  int                      moduleCount; /*!< count of module   */
  struct ls_Module*        modules;     /*!< array of module   */
  struct ls_ModuleStreams* streams;     /*!< streams of module */
};

static int __ls_access(const char* path, int mode)
{
  return access(path, mode);
}

static int __ls_mkdir(const char* path)
{
#if _WIN32 || _WIN64
  return mkdir(path);
#else
  return mkdir(path, S_IRWXU);
#endif
}

static int ls_mkdir(const char* module, const char* monthDir, const char* dayDir)
{
  if (NULL != module && NULL != monthDir && NULL != dayDir)
  {
    char modulePath[_MAX_PATH], monthDirPath[_MAX_PATH], dayDirPath[_MAX_PATH];

    sprintf(modulePath, "%s/%s", LS_WRITEFILE_FILE_DEF, module);
    sprintf(monthDirPath, "%s/%s", modulePath, monthDir);
    sprintf(dayDirPath, "%s/%s", monthDirPath, dayDir);

    if (0 != __ls_access(LS_WRITEFILE_FILE_DEF, 0))
      __ls_mkdir(LS_WRITEFILE_FILE_DEF);
    if (0 != __ls_access(modulePath, 0))
      __ls_mkdir(modulePath);
    if (0 != __ls_access(monthDirPath, 0))
      __ls_mkdir(monthDirPath);
    if (0 != __ls_access(dayDirPath, 0))
      __ls_mkdir(dayDirPath);

    return LS_Success;
  }
  return LS_Failed_InvalidParameters;
}


static void ls_writeFileInit(struct ls_WriteBase* p, struct ls_Array* logModules)
{
  struct ls_WriteFile* self = (struct ls_WriteFile*)p;
  if (NULL != self && self == self->object && NULL != logModules)
  {
    int i;
    int moduleCount = ls_arraySize(logModules);
    size_t size = sizeof(struct ls_ModuleStreams) * moduleCount;
    self->modules = (struct ls_Module*)calloc(sizeof(struct ls_Module) * moduleCount, sizeof(char));
    self->streams = (struct ls_ModuleStreams*)calloc(size, sizeof(char));
    self->moduleCount = moduleCount;

    for (i = 0; i < moduleCount; ++i)
      self->modules[i] = *(struct ls_Module*)ls_arrayGet(logModules, i);
  }
}

static void ls_writeFileUninit(struct ls_WriteBase* p)
{
  struct ls_WriteFile* self = (struct ls_WriteFile*)p;
  if (NULL != self)
  {
    if (NULL != self->streams)
    {
      int i, j;
      for (i = 0; i < self->moduleCount; ++i)
      {
        for (j = 0; j < Severity_Count; ++j)
        {
          if (NULL != self->streams[i].streams[j])
            fclose(self->streams[i].streams[j]);
        }
      }
      free(self->streams);
    }
    if (NULL != self->modules)
      free(self->modules);
  }
}

static void ls_writeFileMessage(struct ls_WriteBase* p, int severity, int module, const char* msg, int len)
{
  struct ls_WriteFile* self = (struct ls_WriteFile*)p;

  if (NULL != self && self == self->object && module >= 0 && module < self->moduleCount && NULL != msg && len > 0)
  {
    char filename[_MAX_PATH];
    char monthDir[32], dayDir[32];
    struct ls_LogTime ltime;
    const char* severity_str;

    ls_getLogTime(&ltime);
    sprintf(monthDir, "%04d-%02d", ltime.year, ltime.mon);
    sprintf(dayDir, "%02d", ltime.day);
    switch (severity)
    {
    case Severity_Debug:
      severity_str = "debug"; break;
    case Severity_Message:
      severity_str = "msg"; break;
    case Severity_Warning:
      severity_str = "warn"; break;
    case Severity_Error:
      severity_str = "err"; break;
    default:
      severity_str = "???"; break;
    }
    sprintf(filename, "%s/%s/%s/%s/%s.log", LS_WRITEFILE_FILE_DEF, self->modules[module].name, monthDir, dayDir, severity_str);

    if (0 != __ls_access(filename, 0))
    {
      if (NULL != self->streams[module].streams[severity])
        fclose(self->streams[module].streams[severity]);
      ls_mkdir(self->modules[module].name, monthDir, dayDir);
      self->streams[module].streams[severity] = fopen(filename, "a+");
    }
    if (NULL == self->streams[module].streams[severity])
      self->streams[module].streams[severity] = fopen(filename, "a+");
    fprintf(self->streams[module].streams[severity], "%s", msg);
  }
}

void ls_writeFileFlush(struct ls_WriteBase* p)
{
  if (NULL != p)
  {
    int i, j;
    struct ls_WriteFile* self = (struct ls_WriteFile*)p;

    for (i = 0; i < self->moduleCount; ++i)
    {
      for (j = 0; j < Severity_Count; ++j)
      {
        if (NULL != self->streams[i].streams[j])
          fflush(self->streams[i].streams[j]);
      }
    }
  }
}


struct ls_WriteBase* ls_writeFileCreate(void)
{
  struct ls_WriteFile* self;
  size_t size = sizeof(struct ls_WriteFile);

  self = (struct ls_WriteFile*)calloc(size, sizeof(char));
  if (NULL != self)
  {
    self->object = self;
    self->writeMessage = ls_writeFileMessage;
    self->writeFlush   = ls_writeFileFlush;
    self->init         = ls_writeFileInit;
    self->uninit       = ls_writeFileUninit;
  }
  return (struct ls_WriteBase*)self;
}

void ls_writeFileRelease(struct ls_WriteBase** p)
{
  struct ls_WriteFile** self = (struct ls_WriteFile**)p;

  if (NULL != *self)
  {
    free(*p);
    *p = NULL;
  }
}
