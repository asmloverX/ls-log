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
#include <process.h>
#include <stdio.h>
#include "../inc/ls_common.h"
#include "../src/ls_circularbuffer.h"

#include "ls_test.h"



typedef struct TransCircularBuffer {
  struct ls_CircularBuffer* lcbuffer;
  HANDLE exit_ev;
} TransCircularBuffer;

static void getLogBuffer(struct ls_LogBuffer* llbuf)
{
  SYSTEMTIME s;

  GetLocalTime(&s);
  llbuf->displayType = (unsigned char)(rand() % 255);
  llbuf->level       = (unsigned char)(rand() % 255);
  llbuf->moduleIndex = (unsigned short)rand() % 65535;
  sprintf(llbuf->message, 
    "[%04d-%02d-%02d %02d:%02d:%02d:%03d] [displayType=>%d, level=>%d, moduleIndex=>%d]\n", 
    s.wYear, s.wMonth, s.wDay, s.wHour, s.wMinute, s.wSecond, s.wMilliseconds, 
    llbuf->displayType, llbuf->level, llbuf->moduleIndex);
}


static UINT WINAPI __innerThread(void* arg)
{
  TransCircularBuffer* trans = (TransCircularBuffer*)arg;
  struct ls_LogBuffer  llbuf;

  for (; ;)
  {
    if (WAIT_OBJECT_0 == WaitForSingleObject(trans->exit_ev, 5))
      break;

    getLogBuffer(&llbuf);
    ls_circularBufferPush(trans->lcbuffer, &llbuf);
  }

  return 0u;
}

void __vistor_circularBuffer(struct ls_LogBuffer* buffer, void* arg)
{
  struct ls_CircularBuffer* lcbuffer = (struct ls_CircularBuffer*)arg;
  fprintf(stdout, "Left data length is [%d], buffer is : %s", ls_circularBufferDataLength(lcbuffer), buffer->message);
}

void ls_test_circularBuffer(void* arg)
{
  struct ls_CircularBuffer* lcbuffer = ls_circularBufferCreate(10240);
  /* struct ls_LogBuffer llbuf; */
  HANDLE h, exit_ev = CreateEvent(NULL, TRUE, FALSE, NULL);
  TransCircularBuffer trans;
  int i;

  UNUSED_PARAM(arg);
  fprintf(stdout, "test unit of ls_CircularBuffer module\n");

  trans.lcbuffer = lcbuffer;
  trans.exit_ev  = exit_ev;
  h = (HANDLE)_beginthreadex(NULL, 0, __innerThread, &trans, 0, NULL);
  Sleep(100);

  for (i = 0; i < 100; ++i)
  {
    /*
    ls_circularBufferPop(lcbuffer, &llbuf);
    
    fprintf(stdout, "[%03d] Left data length is : %d, Get LogBuffer is : %s", i + 1, ls_circularBufferDataLength(lcbuffer), llbuf.message);
    */
    ls_circularBufferPopForeach(lcbuffer, __vistor_circularBuffer, lcbuffer);
    fprintf(stdout, "\n");

    Sleep(50);
  }

  SetEvent(exit_ev);
  WaitForSingleObject(h, INFINITE);
  CloseHandle(h);
  CloseHandle(exit_ev);

  ls_circularBufferRelease(&lcbuffer);
}
