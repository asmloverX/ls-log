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
#include <process.h>
#include <stdio.h>
#include "../inc/ls_common.h"
#include "../src/ls_lock.h"

#include "ls_test.h"


static int g_sNum1 = 0;
static int g_sNum2 = 0;


static UINT WINAPI sThreadAddNum(void* arg)
{
  int i;
  struct ls_Lock* llock = (struct ls_Lock*)arg;

  for (i = 0; i < 100000; ++i)
  {
    ls_lockOn(llock);
    ++g_sNum1;
    ++g_sNum2;
    ls_lockOff(llock);
  }

  return 0u;
}


void ls_test_lock(void* arg)
{
  struct ls_Lock* llock = ls_lockCreate();
  HANDLE hThread[2];
  
  UNUSED_PARAM(arg)
  fprintf(stdout, "test of ls_Lock module\n");

  hThread[0] = (HANDLE)_beginthreadex(NULL, 0, sThreadAddNum, llock, 0, NULL);
  hThread[1] = (HANDLE)_beginthreadex(NULL, 0, sThreadAddNum, llock, 0, NULL);

  WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
  CloseHandle(hThread[0]);
  CloseHandle(hThread[1]);

  fprintf(stdout, "\tg_sNum1 = %d\n"
                  "\tg_sNum2 = %d\n", g_sNum1, g_sNum2);

  ls_lockRelease(&llock);
}
