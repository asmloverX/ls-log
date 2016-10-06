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
#include "../inc/ls_common.h"
#if defined(LS_WINDOWS)
# include <Windows.h>
#else
# include <pthread.h>
#endif
#include <assert.h>
#include "ls_lock.h"

#if defined(LS_WINDOWS)
typedef CRITICAL_SECTION kern_mutex_t;

void kern_mutex_init(kern_mutex_t* mtx)
{
  InitializeCriticalSection(mtx);
}

void kern_mutex_destroy(kern_mutex_t* mtx)
{
  DeleteCriticalSection(mtx);
}

void kern_mutex_lock(kern_mutex_t* mtx)
{
  if ((DWORD)mtx->OwningThread != GetCurrentThreadId())
    EnterCriticalSection(mtx);
}

void kern_mutex_unlock(kern_mutex_t *mtx)
{
  LeaveCriticalSection(mtx);
}
#else
typedef pthread_mutex_t kern_mutex_t;

void kern_mutex_init(kern_mutex_t* mtx)
{
  pthread_mutex_init(mtx, NULL);
}

void kern_mutex_destroy(kern_mutex_t* mtx)
{
  pthread_mutex_destroy(mtx);
}

void kern_mutex_lock(kern_mutex_t* mtx)
{
  pthread_mutex_lock(mtx);
}

void kern_mutex_unlock(kern_mutex_t *mtx)
{
  pthread_mutex_unlock(mtx);
}
#endif

struct ls_Lock {
  kern_mutex_t mtx;
  int locked;
};

struct ls_Lock* ls_lockCreate(void)
{
  struct ls_Lock* self;
  size_t size = sizeof(struct ls_Lock);

  self = (struct ls_Lock*)calloc(size, sizeof(char));
  if (NULL != self)
    kern_mutex_init(&self->mtx);

  return self;
}

void ls_lockRelease(struct ls_Lock** self)
{
  if (NULL != *self)
  {
    if ((*self)->locked)
      kern_mutex_unlock(&(*self)->mtx);
    kern_mutex_destroy(&(*self)->mtx);
    free(*self);
    *self = NULL;
  }
}

void ls_lockOn(struct ls_Lock* self)
{
  if (NULL != self && !self->locked)
  {
    kern_mutex_lock(&self->mtx);
    self->locked = True;
  }
}

void ls_lockOff(struct ls_Lock* self)
{
  if (NULL != self && self->locked)
  {
    kern_mutex_unlock(&self->mtx);
    self->locked = False;
  }
}

int ls_locked(struct ls_Lock* self)
{
  return (NULL != self ? self->locked : False);
}
