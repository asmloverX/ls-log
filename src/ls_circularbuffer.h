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
#ifndef __LS_CIRCULARBUFFER_HEADER_H__
#define __LS_CIRCULARBUFFER_HEADER_H__

#define LS_MESSAGEBUF (1020)  /*!< Ϊ�˷���1024�洢һ��log��Ϣ, 4���ֽڱ�ʶ��־��ʾ��ʽ/�ȼ�/ģ��id */

/*!
 *! @struct ls_LogBuffer
 *! @brief CircularBuffer�洢��log��Ϣ
 */
struct ls_LogBuffer {
  unsigned char  displayType;   /*!< display module - see enum LS_DisplayType   */
  unsigned char  level;         /*!< severity level - see enum LS_SeverityType  */
  unsigned short moduleIndex;   /*!< index of application module                */
  char  message[LS_MESSAGEBUF]; /*!< logging message                            */
}; 

struct ls_CircularBuffer;

extern struct ls_CircularBuffer* ls_circularBufferCreate(int bufLen);
extern void ls_circularBufferRelease(struct ls_CircularBuffer** self);

extern void ls_circularBufferClear(struct ls_CircularBuffer* self);
extern int ls_circularBufferDataLength(struct ls_CircularBuffer* self);
extern int ls_circularBufferPush(struct ls_CircularBuffer* self, struct ls_LogBuffer* buffer);
extern int ls_circularBufferPop(struct ls_CircularBuffer* self, struct ls_LogBuffer* buffer);
extern int ls_circularBufferPopForeach(struct ls_CircularBuffer* self, void (*vistor)(struct ls_LogBuffer*, void*), void* arg);

#endif  /* __LS_CIRCULARBUFFER_HEADER_H__ */
