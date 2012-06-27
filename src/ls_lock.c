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
#if _WIN32 || _WIN64
  #include <windows.h>
#else
  #include <pthread.h>
#endif
#include <assert.h>

#include "ls_lock.h"


#if !defined(LS_MT)
  #define Locker            int
  #define Locker_Init(L)
  #define Locker_Uninit(L)
  #define Locker_On(L)
  #define Locker_Off(L)
#elif defined(LS_WIN32_MT)
  #define Locker            CRITICAL_SECTION
  #define Locker_Init(L)    InitializeCriticalSection(&(L))
  #define Locker_Uninit(L)  DeleteCriticalSection(&(L))
  #define Locker_On(L)      EnterCriticalSection(&(L))
  #define Locker_Off(L)     LeaveCriticalSection(&(L))
#elif defined(LS_POSIX_MT)
  #define Locker            pthread_mutex_t
  #define Locker_Init(L)    pthread_mutex_init(&(L), NULL)
  #define Locker_Uninit(L)  pthread_mutex_destroy(&(L))
  #define Locker_On(L)      pthread_mutex_lock(&(L))
  #define Locker_Off(L)     pthread_mutex_unlock(&(L))
#else
  #error only Windows and POSIX are supported!
#endif


struct ls_Lock {
  Locker L;
};


struct ls_Lock* ls_lockCreate(void)
{
  struct ls_Lock* self;
  size_t size = sizeof(struct ls_Lock);

  self = (struct ls_Lock*)calloc(size, sizeof(char));
  if (NULL != self)
    Locker_Init(self->L);

  return self;
}

void ls_lockRelease(struct ls_Lock** self)
{
  if (NULL != *self)
  {
    Locker_Uninit((*self)->L);
    free(*self);
    *self = NULL;
  }
}

void ls_lockOn(struct ls_Lock* self)
{
  if (NULL != self)
    Locker_On(self->L);
}

void ls_lockOff(struct ls_Lock* self)
{
  if (NULL != self)
    Locker_Off(self->L);
}