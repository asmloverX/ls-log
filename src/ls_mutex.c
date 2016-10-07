/*
 * Copyright (c) 2016 ASMlover. All rights reserved.
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

  typedef CRITICAL_SECTION kern_mutex_t;
# define kern_mutex_init InitializeCriticalSection
# define kern_mutex_destroy DeleteCriticalSection
# define kern_mutex_lock EnterCriticalSection
# define kern_mutex_unlock LeaveCriticalSection
#else
# include <pthread.h>

  typedef pthread_mutex_t kern_mutex_t;
# define kern_mutex_init(m) pthread_mutex_init(m, NULL)
# define kern_mutex_destroy pthread_mutex_destroy
# define kern_mutex_lock pthread_mutex_lock
# define kern_mutex_unlock pthread_mutex_unlock
#endif
#include <stdlib.h>
#include "ls_mutex.h"

struct ls_mutex_t {
  kern_mutex_t mtx;
  ls_boolean_t locked;
};

struct ls_mutex_t*
ls_mutex_create(void)
{
  struct ls_mutex_t* mtx = (struct ls_mutex_t*)malloc(sizeof(struct ls_mutex_t));
  if (NULL != mtx) {
    kern_mutex_init(&mtx->mtx);
    mtx->locked = False;
  }

  return mtx;
}

void
ls_mutex_destroy(struct ls_mutex_t* mtx)
{
  if (NULL != mtx) {
    if (mtx->locked)
      kern_mutex_unlock(&mtx->mtx);

    kern_mutex_destroy(&mtx->mtx);
    free(mtx);
  }
}

void
ls_mutex_lock(struct ls_mutex_t* mtx)
{
  if (NULL != mtx && !mtx->locked) {
    kern_mutex_lock(&mtx->mtx);
    mtx->locked = True;
  }
}

void
ls_mutex_unlock(struct ls_mutex_t* mtx)
{
  if (NULL != mtx && mtx->locked) {
    kern_mutex_unlock(&mtx->mtx);
    mtx->locked = False;
  }
}

int
ls_mutex_locked(struct ls_mutex_t* mtx)
{
  return (NULL != mtx ? mtx->locked : False);
}
