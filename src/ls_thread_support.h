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
#ifndef LITESPHINX_THREADSUPPORT_INCLUDED_H
#define LITESPHINX_THREADSUPPORT_INCLUDED_H

#include "../inc/ls_common.h"
#include <stdint.h>
#if defined(LS_WINDOWS)
# include <Windows.h>

  struct __libsl_thread_imp {
    HANDLE __h;
    unsigned int __id;
  };

  typedef CRITICAL_SECTION          __libsl_mutex_t;
  typedef CONDITION_VARIABLE        __libsl_condvar_t;
  typedef struct __libsl_thread_imp __libsl_thread_t;
  typedef int                       __libsl_pid_t;
  typedef DWORD                     __libsl_tls_key;
#else
# include <pthread.h>

  typedef pthread_mutex_t           __libsl_mutex_t;
  typedef pthread_cond_t            __libsl_condvar_t;
  typedef pthread_t                 __libsl_thread_t;
  typedef pid_t                     __libsl_pid_t;
  typedef pthread_key_t             __libsl_tls_key;
#endif

#undef NANOSEC
#define NANOSEC ((uint64_t)1e9)

extern __libsl_pid_t __libsl_gettid(void);

extern int __libsl_mutex_init(__libsl_mutex_t* m);
extern int __libsl_mutex_destroy(__libsl_mutex_t* m);
extern int __libsl_mutex_lock(__libsl_mutex_t* m);
extern int __libsl_mutex_trylock(__libsl_mutex_t* m);
extern int __libsl_mutex_unlock(__libsl_mutex_t* m);

extern int __libsl_condvar_init(__libsl_condvar_t* cv);
extern int __libsl_condvar_destroy(__libsl_condvar_t* cv);
extern int __libsl_condvar_signal(__libsl_condvar_t* cv);
extern int __libsl_condvar_broadcast(__libsl_condvar_t* cv);
extern int __libsl_condvar_wait(__libsl_condvar_t* cv, __libsl_mutex_t* m);
extern int __libsl_condvar_timedwait(__libsl_condvar_t* cv, __libsl_mutex_t* m, uint64_t nanosec);

extern int __libsl_thread_create(__libsl_thread_t* t, void (*closure)(void*), void* arg);
extern int __libsl_thread_join(__libsl_thread_t* t);
extern int __libsl_thread_detach(__libsl_thread_t* t);

extern int __libsl_tls_create(__libsl_tls_key* key, void (*closure)(void*));
extern int __libsl_tls_delete(__libsl_tls_key key);
extern int __libsl_tls_set(__libsl_tls_key key, void* p);
extern void* __libsl_tls_get(__libsl_tls_key key);

#endif  /* LITESPHINX_THREADSUPPORT_INCLUDED_H */
