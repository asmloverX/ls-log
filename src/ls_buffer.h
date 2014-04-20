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
#ifndef __LS_BUFFER_HEADER_H__
#define __LS_BUFFER_HEADER_H__

struct ls_Buffer;

extern struct ls_Buffer* ls_bufferCreate(int bufLen);
extern void ls_bufferRelease(struct ls_Buffer** self);

extern void ls_bufferClear(struct ls_Buffer* self);
extern int ls_bufferWrite(struct ls_Buffer* self, const void* inBuf, int inLen);
extern int ls_bufferRead(struct ls_Buffer* self, int outLen, void* outBuf);
extern int ls_bufferRemove(struct ls_Buffer* self, int len);
extern int ls_bufferSize(struct ls_Buffer* self);
extern int ls_bufferDataLength(struct ls_Buffer* self);
extern int ls_bufferFreeLength(struct ls_Buffer* self);

#endif  /* __LS_BUFFER_HEADER_H__ */
