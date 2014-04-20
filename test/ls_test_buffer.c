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
#include "../src/ls_buffer.h"
#include "../src/ls_lock.h"

#include "ls_test.h"


static void __inner_help(void)
{
  fprintf(stdout, 
    "  Test of ls_Buffer module\n"
    "  usage ls-log buffer [option]\n\n"
    "  options:\n"
    "    -help          display the document of ls_Buffer module\n"
    "    -single        display the test of ls_Buffer module with single type\n"
    "    -mtnolock      display the test of ls_Buffer module with threads no lock\n"
    "    -mtlock        display the test of ls_Buffer module with threads\n"
    "    -funcs         display the test of ls_Buffer module with functions\n"
    );
}


static void getDataBuffer(int count, short* data)
{
  int i;
  for (i = 0; i < count; ++i)
    data[i] = (short)(rand() % count);
}

/*! single thread test */
static void __inner_single(struct ls_Buffer* lbuffer)
{
  short inData[200], outData[200];
  int len   = sizeof(inData);
  int count = 0;

  if (NULL == lbuffer)
    return;

  while (ls_bufferFreeLength(lbuffer) >= len)
  {
    getDataBuffer(200, inData);
    ls_bufferWrite(lbuffer, inData, len);
    ++count;
  }
  fprintf(stdout, "count = %d\n", count);
  Sleep(2000);

  while (len == ls_bufferRead(lbuffer, len, outData))
  {
    int i;
    for (i = 0; i < 200; ++i)
      fprintf(stdout, "[%03d] = %03d\t", i + 1, outData[i]);
    fprintf(stdout, "\n");
    --count;
  }
  fprintf(stdout, "count = %d\n", count);
}


/*! multithreads test with no lock */
typedef struct TransNoLock {
  struct ls_Buffer* lbuffer;
  HANDLE            exit_ev;
} TransNoLock;
static UINT WINAPI __innerThreadNoLock(void* arg)
{
  TransNoLock* trans = (TransNoLock*)arg;
  short inData[200];
  int   len = sizeof(inData);

  for (; ;)
  {
    if (WAIT_OBJECT_0 == WaitForSingleObject(trans->exit_ev, 5))
      break;
    getDataBuffer(200, inData);
    ls_bufferWrite(trans->lbuffer, inData, len);
  }

  return 0U;
}

static void __inner_mtNoLock(struct ls_Buffer* lbuffer)
{
  HANDLE exit_ev = CreateEvent(NULL, TRUE, FALSE, NULL);
  TransNoLock trans;
  HANDLE h;
  int   i, j, count = 0;
  short outData[200];
  int   outLen = sizeof(outData);

  trans.lbuffer = lbuffer;
  trans.exit_ev = exit_ev;
  h = (HANDLE)_beginthreadex(NULL, 0, __innerThreadNoLock, &trans, 0, NULL);
  Sleep(500);

  for (i = 0; i < 1000; ++i)
  {
    if (outLen == ls_bufferRead(lbuffer, outLen, outData))
    {
      for (j = 0; j < 200; ++j)
        fprintf(stdout, "[%03d] = %03d\t", j + 1, outData[j]);
      fprintf(stdout, "\n\n");
      ++count;
      Sleep(10);
    }
  }
  fprintf(stdout, "get success out data count is : %d/%d\n", count, i);

  SetEvent(exit_ev);
  WaitForSingleObject(h, INFINITE);

  CloseHandle(h);
  CloseHandle(exit_ev);
}


/*! multithreads test with lock */
typedef struct TransWithLock {
  struct ls_Buffer* lbuffer;
  struct ls_Lock*   llock;
  HANDLE            exit_ev;
} TransWithLock;
static UINT WINAPI __innerThreadWithLock(void* arg)
{
  TransWithLock* trans = (TransWithLock*)arg;
  short inData[200];
  int   inLen = sizeof(inData);

  for (; ;)
  {
    if (WAIT_OBJECT_0 == WaitForSingleObject(trans->exit_ev, 0))
      break;
    getDataBuffer(200, inData);

    ls_lockOn(trans->llock);
    ls_bufferWrite(trans->lbuffer, inData, inLen);
    ls_lockOff(trans->llock);
  }

  return 0u;
}

static void __inner_mtWithLock(struct ls_Buffer* lbuffer)
{
  struct ls_Lock* llock = ls_lockCreate();
  TransWithLock trans;
  HANDLE        h;
  short         outData[200];
  int           outLen = sizeof(outData);
  int           i, j, retLen, count = 0;

  trans.lbuffer = lbuffer;
  trans.exit_ev = CreateEvent(NULL, TRUE, FALSE, NULL);
  trans.llock   = llock;

  h = (HANDLE)_beginthreadex(NULL, 0, __innerThreadWithLock, &trans, 0, NULL);
  Sleep(100);

  for (i = 0; i < 1000; ++i)
  {
    ls_lockOn(llock);
    retLen = ls_bufferRead(lbuffer, outLen, outData);
    ls_lockOff(llock);

    if (retLen == outLen)
    {
      for (j = 0; j < 200; ++j)
        fprintf(stdout, "[%03d] = %03d\t", j + 1, outData[j]);
      fprintf(stdout, "\n\n");
      ++count;
      Sleep(10);
    }
  }
  fprintf(stdout, "get success out data count is : %d/%d\n", count, i);

  SetEvent(trans.exit_ev);
  WaitForSingleObject(h, INFINITE);
  CloseHandle(h);
  CloseHandle(trans.exit_ev);
  
  ls_lockRelease(&llock);
}

/* test unit of ls_Buffer interfaces */
static void __inner_testFuncstions(struct ls_Buffer* lbuffer)
{
  short inData[200], outData[200];
  int   i, dataLen = sizeof(inData);

  getDataBuffer(200, inData);
  ls_bufferWrite(lbuffer, inData, dataLen);

  if (dataLen == ls_bufferDataLength(lbuffer))
    fprintf(stdout, "  ls_Buffer data length is : %d\n", dataLen);
  if (ls_bufferSize(lbuffer) - dataLen == ls_bufferFreeLength(lbuffer))
    fprintf(stdout, "  ls_Buffer free length is : %d\n", ls_bufferSize(lbuffer) - dataLen);

  for (i = 0; i < 5; ++i)
  {
    getDataBuffer(200, inData);
    ls_bufferWrite(lbuffer, inData, dataLen);
  }
  fprintf(stdout, "  after insert buffer 5 times ===>\n");
  fprintf(stdout, 
    "    now ls_Buffer data length is : %d\n"
    "    now ls_Buffer free length is : %d\n", 
    ls_bufferDataLength(lbuffer), ls_bufferFreeLength(lbuffer));

  fprintf(stdout, "  remove length is : %d ===>\n", ls_bufferRemove(lbuffer, 1000));
  fprintf(stdout, 
    "    now ls_Buffer data length is : %d\n"
    "    now ls_Buffer free length is : %d\n", 
    ls_bufferDataLength(lbuffer), ls_bufferFreeLength(lbuffer));

  ls_bufferClear(lbuffer);
  fprintf(stdout, "  after ls_bufferClear ===>\n");
  fprintf(stdout, 
    "    now ls_Buffer data length is : %d\n"
    "    now ls_Buffer free length is : %d\n", 
    ls_bufferDataLength(lbuffer), ls_bufferFreeLength(lbuffer));

  for (i = 0; i < 5; ++i)
  {
    getDataBuffer(200, inData);
    ls_bufferWrite(lbuffer, inData, dataLen);
  }
  fprintf(stdout, "  after ls_bufferWrite 5 times ...\n");
  fprintf(stdout, 
    "    now ls_Buffer data length is : %d\n"
    "    now ls_Buffer free length is : %d\n", 
    ls_bufferDataLength(lbuffer), ls_bufferFreeLength(lbuffer));

  ls_bufferRead(lbuffer, dataLen, outData);
  fprintf(stdout, "  after ls_bufferRead ===>\n");
  fprintf(stdout, 
    "    now ls_Buffer data length is : %d\n"
    "    now ls_Buffer free length is : %d\n", 
    ls_bufferDataLength(lbuffer), ls_bufferFreeLength(lbuffer));

  ls_bufferWrite(lbuffer, inData, dataLen);
  fprintf(stdout, "  after call ls_bufferWrite again ===>\n");
  fprintf(stdout, 
    "    now ls_Buffer data length is : %d\n"
    "    now ls_Buffer free length is : %d\n", 
    ls_bufferDataLength(lbuffer), ls_bufferFreeLength(lbuffer));
  
  ls_bufferWrite(lbuffer, inData, dataLen);
  fprintf(stdout, "  after call ls_bufferWrite again ===>\n");
  fprintf(stdout, 
    "    now ls_Buffer data length is : %d\n"
    "    now ls_Buffer free length is : %d\n", 
    ls_bufferDataLength(lbuffer), ls_bufferFreeLength(lbuffer));
  
  ls_bufferRemove(lbuffer, ls_bufferSize(lbuffer));
  fprintf(stdout, "  after ls_bufferRemove the ls_Buffer ===>\n");
  fprintf(stdout, 
    "    now ls_Buffer data length is : %d\n"
    "    now ls_Buffer free length is : %d\n", 
    ls_bufferDataLength(lbuffer), ls_bufferFreeLength(lbuffer));
}



void ls_test_buffer(void* arg)
{
  struct ls_Buffer* lbuffer = ls_bufferCreate(2048);
  char*  option = (char*)arg;

  fprintf(stdout, "test unit of ls_Buffer module\n");
  if (NULL == option)
    __inner_help();
  else if (0 == stricmp("-help", option))
    __inner_help();
  else if (0 == stricmp("-single", option))
    __inner_single(lbuffer);
  else if (0 == stricmp("-mtnolock", option))
    __inner_mtNoLock(lbuffer);
  else if (0 == stricmp("-mtlock", option))
    __inner_mtWithLock(lbuffer);
  else if (0 == stricmp("-funcs", option))
    __inner_testFuncstions(lbuffer);
  else
    __inner_help();


  ls_bufferRelease(&lbuffer);
}
