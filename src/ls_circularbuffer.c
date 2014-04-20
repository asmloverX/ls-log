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
#include <stdio.h>
#include <stdlib.h>
#include "../inc/ls_common.h"
#include "ls_buffer.h"
#include "ls_lock.h"

#include "ls_circularbuffer.h"



/*!
 *! @struct ls_CircularBuffer
 *! @brief  内存循环缓冲区, 主要是对ls_Buffer的简易封装, 该封装主要是对
 *!         数据ls_LogBuffer的操作, 同时该对象是对数据进行了数据同步操作
 *!         这样将保证了多线程操作下的数据安全性
 */
struct ls_CircularBuffer {
  struct ls_Lock*   llock;    /*!< 多线程操作锁 */
  struct ls_Buffer* lbuffer;  /*!< 内存缓冲区   */
};

static int pushBuffer(struct ls_CircularBuffer* self, const void* buffer, int len)
{
  if (NULL != self && NULL != self->lbuffer && NULL != self->llock && NULL != buffer && len > 0)
  {
    int pushLen, ret;

    ls_lockOn(self->llock);
    if (len > ls_bufferSize(self->lbuffer))
      ret = LS_Failed_InvalidParameters;
    else
    {
      while (ls_bufferFreeLength(self->lbuffer) < len)
        ls_bufferRemove(self->lbuffer, sizeof(struct ls_LogBuffer));
      pushLen = ls_bufferWrite(self->lbuffer, buffer, len);

      ret = (pushLen == len ? LS_Success : LS_Failed);
    }
    ls_lockOff(self->llock);

    return ret;
  }
  return LS_Failed_InvalidParameters;
}




struct ls_CircularBuffer* ls_circularBufferCreate(int bufLen)
{
  struct ls_CircularBuffer* self;
  size_t size = sizeof(struct ls_CircularBuffer);

  self = (struct ls_CircularBuffer*)calloc(size, sizeof(char));
  if (NULL != self)
  {
    self->llock   = ls_lockCreate();
    self->lbuffer = ls_bufferCreate(bufLen);

    if (NULL == self->lbuffer || NULL == self->llock)
      ls_circularBufferRelease(&self);
  }
  return self;
}

void ls_circularBufferRelease(struct ls_CircularBuffer** self)
{
  if (NULL != *self)
  {
    ls_bufferRelease(&(*self)->lbuffer);
    ls_lockRelease(&(*self)->llock);

    free(*self);
    *self = NULL;
  }
}

void ls_circularBufferClear(struct ls_CircularBuffer* self)
{
  if (NULL != self && NULL != self->lbuffer && NULL != self->llock)
  {
    ls_lockOn(self->llock);
    ls_bufferClear(self->lbuffer);
    ls_lockOff(self->llock);
  }
}

int ls_circularBufferDataLength(struct ls_CircularBuffer* self)
{
  int dataLength = LS_ERROR_LEN;

  if (NULL != self && NULL != self->lbuffer && NULL != self->llock)
  {
    ls_lockOn(self->llock);
    dataLength = ls_bufferDataLength(self->lbuffer);
    ls_lockOff(self->llock);
  }
  return dataLength;
}

int ls_circularBufferPush(struct ls_CircularBuffer* self, struct ls_LogBuffer* buffer)
{
  return pushBuffer(self, buffer, sizeof(struct ls_LogBuffer));
}

int ls_circularBufferPop(struct ls_CircularBuffer* self, struct ls_LogBuffer* buffer)
{
  if (NULL != self && NULL != self->llock && NULL != self->lbuffer && NULL != buffer)
  {
    int popLen;

    ls_lockOn(self->llock);
    popLen = ls_bufferRead(self->lbuffer, sizeof(struct ls_LogBuffer), buffer);
    ls_lockOff(self->llock);

    return (popLen == sizeof(struct ls_LogBuffer) ? LS_Success : LS_Failed);
  }
  return LS_Failed_InvalidParameters;
}

int ls_circularBufferPopForeach(struct ls_CircularBuffer* self, void (*vistor)(struct ls_LogBuffer*, void*), void* arg)
{
  if (NULL != self && NULL != self->llock && NULL != self->lbuffer)
  {
    struct ls_LogBuffer buffer;
    int popLen = sizeof(struct ls_LogBuffer);

    ls_lockOn(self->llock);
    while (popLen == ls_bufferRead(self->lbuffer, popLen, &buffer))
    {
      if (NULL != vistor)
        vistor(&buffer, arg);
    }
    ls_lockOff(self->llock);

    return LS_Success;
  }
  return LS_Failed_InvalidParameters;
}
