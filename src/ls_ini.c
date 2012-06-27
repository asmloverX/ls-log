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
#include <string.h>
#if _WIN32 || _WIN64
  #include <windows.h>
#else
  /*!< TODO: Linux/FreeBSD */
#endif
#include "../inc/ls_common.h"

#include "ls_ini.h"



/*!
 *! @struct ls_Ini
 *! @brief Ini File operation module 
 */
struct ls_Ini {
  char file[_MAX_PATH]; /*!< ini filename */
};


struct ls_Ini* ls_iniOpen(const char* file)
{
  struct ls_Ini* self;
  size_t size = sizeof(struct ls_Ini);

  if (NULL == file)
    return NULL;

  self = (struct ls_Ini*)calloc(size, sizeof(char));
  if (NULL != self)
    strncpy(self->file, file, strlen(file));

  return self;
}

void ls_iniClose(struct ls_Ini** self)
{
  if (NULL != *self)
  {
    free(*self);
    *self = NULL;
  }
}

int ls_iniGetInt(struct ls_Ini* self, char* app, char* key, int* outValue)
{
  if (NULL != self && NULL != app && NULL != key && NULL != outValue)
  {
    *outValue = GetPrivateProfileIntA(app, key, 0, self->file);

    return LS_Success;
  }
  return LS_Failed_InvalidParameters;
}

int ls_iniGetStr(struct ls_Ini* self, char* app, char* key, int outLen, char* outValue)
{
  if (NULL != self && NULL != app && NULL != key && NULL != outValue && outLen > 0)
  {
    if (GetPrivateProfileStringA(app, key, NULL, outValue, outLen, self->file) >= (unsigned long)outLen)
      return LS_Failed;
    return LS_Success;
  }
  return LS_Failed_InvalidParameters;
}
