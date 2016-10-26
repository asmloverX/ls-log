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
#include <process.h>
#include "ls_thread_support.h"

typedef struct __libsl_thread_invoker {
  void (*__fn)(void*);
  void* __arg;
  __libsl_condvar_t* __cv;
  __libsl_mutex_t* __m;
  int* __started;
} __libsl_thread_invoker;


__libsl_pid_t __libsl_gettid(void)
{
  return (__libsl_pid_t)GetCurrentThreadId();
}

int __libsl_mutex_init(__libsl_mutex_t* m)
{
  return InitializeCriticalSection(m), 0;
}

int __libsl_mutex_destroy(__libsl_mutex_t* m)
{
  return DeleteCriticalSection(m), 0;
}

int __libsl_mutex_lock(__libsl_mutex_t* m)
{
  if ((__libsl_pid_t)m->OwningThread != __libsl_gettid())
    EnterCriticalSection(m);
  return 0;
}

int __libsl_mutex_trylock(__libsl_mutex_t* m)
{
  if ((__libsl_pid_t)m->OwningThread != __libsl_gettid())
    return TryEnterCriticalSection(m) ? 0 : -1;
  return 0;
}

int __libsl_mutex_unlock(__libsl_mutex_t* m)
{
  return LeaveCriticalSection(m), 0;
}

int __libsl_condvar_init(__libsl_condvar_t* cv)
{
  return InitializeConditionVariable(cv), 0;
}

int __libsl_condvar_destroy(__libsl_condvar_t* cv)
{
  return (void)cv, 0;
}

int __libsl_condvar_signal(__libsl_condvar_t* cv)
{
  return WakeConditionVariable(cv), 0;
}

int __libsl_condvar_broadcast(__libsl_condvar_t* cv)
{
  return WakeAllConditionVariable(cv), 0;
}

int __libsl_condvar_wait(__libsl_condvar_t* cv, __libsl_mutex_t* m)
{
  return SleepConditionVariableCS(cv, m, INFINITE), 0;
}

int __libsl_condvar_timedwait(__libsl_condvar_t* cv, __libsl_mutex_t* m, uint64_t nanosec)
{
  return SleepConditionVariableCS(cv, m, (DWORD)(nanosec / 1e6)), 0;
}

static uint32_t __stdcall __libsl_thread_closure(void* arg)
{
  __libsl_thread_invoker* invoker = (__libsl_thread_invoker*)arg;
  if (NULL == invoker)
    return 0;

  __libsl_mutex_lock(invoker->__m);
  *invoker->__started = 1;
  __libsl_condvar_signal(invoker->__cv);
  __libsl_mutex_unlock(invoker->__m);

  if (NULL != invoker->__fn)
    invoker->__fn(invoker->__arg);

  free(invoker);
  return 0;
}

int __libsl_thread_create(__libsl_thread_t* t, void (*closure)(void*), void* arg)
{
  int started = 0;
  __libsl_condvar_t cv;
  __libsl_mutex_t m;
  __libsl_thread_invoker* invoker;

  __libsl_condvar_init(&cv);
  __libsl_mutex_init(&m);

  invoker = (__libsl_thread_invoker*)malloc(sizeof(*invoker));
  invoker->__fn = closure;
  invoker->__arg = arg;
  invoker->__cv = &cv;
  invoker->__m = &m;
  invoker->__started = &started;
  __libsl_mutex_lock(&m);
  t->__h = (HANDLE)_beginthreadex(NULL, 0, __libsl_thread_closure, invoker, 0, &t->__id);
  if (NULL != t->__h) {
    while (!started)
      __libsl_condvar_wait(&cv, &m);
  }
  else {
    free(invoker);
  }
  __libsl_mutex_unlock(&m);

  __libsl_mutex_destroy(&m);
  __libsl_condvar_destroy(&cv);

  return 0;
}

int __libsl_thread_join(__libsl_thread_t* t)
{
  DWORD r;
  if (WAIT_FAILED == WaitForSingleObjectEx(t->__h, INFINITE, FALSE)
      || 0 == GetExitCodeThread(t->__h, &r))
    return -1;
  return CloseHandle(t->__h) ? 0 : -1;
}

int __libsl_thread_detach(__libsl_thread_t* t)
{
  return CloseHandle(t->__h) ? 0 : -1;
}

int __libsl_tls_create(__libsl_tls_key* key, void (*closure)(void*))
{
  return *key = FlsAlloc((PFLS_CALLBACK_FUNCTION)closure), 0;
}

int __libsl_tls_delete(__libsl_tls_key key)
{
  return FlsFree(key), 0;
}

int __libsl_tls_set(__libsl_tls_key key, void* p)
{
  return FlsSetValue(key, p) ? 0 : -1;
}

void* __libsl_tls_get(__libsl_tls_key key)
{
  return FlsGetValue(key);
}
