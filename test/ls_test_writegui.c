/*
 * Copyright (c) 2012 ASMlover. All rights reserved.
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
#include "../inc/ls_common.h"
#include "../inc/ls_log.h"
#include "../src/ls_writebase.h"

#include "ls_test.h"


void ls_test_writegui(void* arg)
{
  int i;

  struct ls_WriteBase* wgui = ls_writeGuiCreate();

  UNUSED_PARAM(arg)
  fprintf(stdout, "test unit of ls_WriteGui module\n");

  srand((UINT)GetTickCount());
  for (i = 0; i < 100; ++i)
  {
    char buf[128];
    SYSTEMTIME s;

    GetLocalTime(&s);
    sprintf(buf, "  [%03d] [%04d-%02d-%02d %02d:%02d:%02d:%03d] - %s\n",
      i, s.wYear, s.wMonth, s.wDay, s.wHour, s.wMinute, s.wSecond, s.wMilliseconds, __FUNCSIG__);

    wgui->writeMessage(wgui, rand() % Severity_Count, rand() % 4, buf, strlen(buf));
    fprintf(stdout, "%s", buf);

    Sleep(100);
  }

  ls_writeGuiRelease(&wgui);
}
