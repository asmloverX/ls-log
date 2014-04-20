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
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/ls_common.h"

#include "ls_buffer.h"


/*!
 *! @struct ls_Buffer
 *! @brief 内存缓冲区操作接口类, 主要用来存放数据信息, 在该日志
 *!        系统的设计中减少频繁的操作IO, 提高效率
 *!
 *! 内存模型如下:
 *!     ===== 表示写入的数据
 *!     ----- 表示空闲的空间
 *!
 *! (readPos/writePos)
 *! |-------------------------------------------------------------------|
 *!
 *!                    (readPos)                     (writePos)
 *! |------------------==============================-------------------|
 *!
 *!            (writePos)                                (readPos)
 *! |==========------------------------------------------===============|
 *!
 *! (readPos)                                  (writePos)
 *! |==========================================-------------------------|
 *!
 *! (readPos)                                                           (writePos)
 *! |===================================================================|
 *!
 *!                                   (writePos/readPos)
 *! |=================================|=================================|
 */
struct ls_Buffer {
  char* buffer;     /*!< 存储的数据空间 */
  int   readPos;    /*!< 当前的read位置 */
  int   writePos;   /*!< 当前write位置  */
  int   bufLen;     /*!< buffer空间大小 */
  int   dataLen;    /*!< 数据空间的大小 */
  int   freeLen;    /*!< 空闲空间的大小 */
};



struct ls_Buffer* ls_bufferCreate(int bufLen)
{
  struct ls_Buffer* self;
  size_t size = sizeof(struct ls_Buffer);

  self = (struct ls_Buffer*)calloc(size, sizeof(char));
  if (NULL != self)
  {
    self->bufLen  = (bufLen < LS_BUFFERLEN_DEF ? LS_BUFFERLEN_DEF : bufLen);
    self->buffer  = (char*)calloc(self->bufLen, sizeof(char));
    self->freeLen = self->bufLen;

    assert(NULL != self->buffer);
    /*!< TODO: */
  }

  return self;
}

void ls_bufferRelease(struct ls_Buffer** self)
{
  if (NULL != *self)
  {
    if (NULL != (*self)->buffer)
      free((*self)->buffer);
    free(*self);
    *self = NULL;
  }
}

void ls_bufferClear(struct ls_Buffer* self)
{
  if (NULL != self)
  {
    if (NULL != self->buffer)
      memset(self->buffer, 0, self->bufLen);
    self->readPos = self->writePos = 0;
    self->dataLen = 0;
    self->freeLen = self->bufLen;
  }
}

int ls_bufferWrite(struct ls_Buffer* self, const void* inBuf, int inLen)
{
  if (NULL != self && NULL != self->buffer && NULL != inBuf && inLen > 0)
  {
    int freeLen  = ls_bufferFreeLength(self);
    int writeLen = (freeLen >= inLen ? inLen : freeLen);

    if (self->readPos > self->writePos)
    {
      memcpy(self->buffer + self->writePos, inBuf, writeLen);
      self->writePos += writeLen;
    }
    else
    {
      int leftLen = 0, tailLen = self->bufLen - self->writePos;
      tailLen = (tailLen >= writeLen ? writeLen : tailLen);
      leftLen = writeLen - tailLen;

      memcpy(self->buffer + self->writePos, inBuf, tailLen);
      self->writePos = (self->writePos + tailLen) % self->bufLen;
      if (leftLen > 0)
      {
        memcpy(self->buffer, (char*)inBuf + tailLen, leftLen);
        self->writePos += leftLen;
      }
    }
    self->dataLen += writeLen;
    self->freeLen -= writeLen;
    
    return writeLen;
  }
  return LS_ERROR_LEN;
}

int ls_bufferRead(struct ls_Buffer* self, int outLen, void* outBuf)
{
  if (NULL != self && NULL != self->buffer && outLen > 0)
  {
    int dataLen = ls_bufferDataLength(self);
    int readLen = (dataLen >= outLen ? outLen : dataLen);

    if (self->writePos > self->readPos)
    {
      memcpy(outBuf, self->buffer + self->readPos, readLen);
      self->readPos += readLen;
    }
    else
    {
      int leftLen = 0, tailLen = self->bufLen - self->readPos;
      tailLen = (tailLen >= readLen ? readLen : tailLen);
      leftLen = readLen - tailLen;

      memcpy(outBuf, self->buffer + self->readPos, tailLen);
      self->readPos = (self->readPos + tailLen) % self->bufLen;
      if (leftLen > 0)
      {
        memcpy((char*)outBuf + tailLen, self->buffer, leftLen);
        self->readPos += leftLen;
      }
    }
    self->dataLen -= readLen;
    self->freeLen += readLen;

    return readLen;
  }
  return LS_ERROR_LEN;
}

int ls_bufferRemove(struct ls_Buffer* self, int len)
{
  if (NULL != self && NULL != self->buffer)
  {
    int dataLen   = ls_bufferDataLength(self);
    int removeLen = (dataLen >= len ? len : dataLen);

    if (self->writePos > self->readPos)
      self->readPos += removeLen;
    else
    {
      int tailLen = self->bufLen - self->readPos;
      tailLen = (tailLen >= removeLen ? removeLen : tailLen);

      self->readPos = (self->readPos + tailLen) % self->bufLen;
      self->readPos += (removeLen - tailLen);
    }
    self->dataLen -= removeLen;
    self->freeLen += removeLen;

    return removeLen;
  }
  return LS_ERROR_LEN;
}

int ls_bufferSize(struct ls_Buffer* self)
{
  return (NULL != self ? self->bufLen : LS_ERROR_LEN);
}

int ls_bufferDataLength(struct ls_Buffer* self)
{
  return (NULL != self ? self->dataLen : LS_ERROR_LEN);
}

int ls_bufferFreeLength(struct ls_Buffer* self)
{
  return (NULL != self ? self->freeLen : LS_ERROR_LEN);
}
