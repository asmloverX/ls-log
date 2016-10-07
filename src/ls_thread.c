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
# include <process.h>
#else
# include <pthread.h>
#endif
#include <assert.h>
#include "ls_thread.h"

struct ls_thread_t {
#if defined(LS_WINDOWS)
  HANDLE start_event;
  HANDLE thread;
#else
  pthread_t thread_id;
#endif
  thread_routine_t routine;
  void* argument;
};

#if defined(LS_WINDOWS)
static UINT WINAPI
ls_thread_routine(void* arg)
{
  struct ls_thread_t* thrd = (struct ls_thread_t*)arg;
  if (NULL == thrd)
    return 0;

  SetEvent(thrd->start_event);
  if (NULL != thrd->routine)
    thrd->routine(thrd->argument);

  return 0;
}

struct ls_thread_t*
ls_thread_create(thread_routine_t routine, void* arg)
{
  struct ls_thread_t* thrd = (struct ls_thread_t*)malloc(sizeof(struct ls_thread_t));
  if (NULL != thrd) {
    thrd->routine = routine;
    thrd->argument = arg;

    thrd->start_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    assert(NULL != thrd->start_event);
    thrd->thread = (HANDLE)_beginthreadex(NULL, 0, ls_thread_routine, thrd, 0, NULL);
    assert(NULL != thrd->thread);

    WaitForSingleObject(thrd->start_event, INFINITE);
    CloseHandle(thrd->start_event);
  }
  return thrd;
}

void
ls_thread_destroy(struct ls_thread_t* thrd)
{
  if (NULL != thrd) {
    ls_thread_join(thrd);
    free(thrd);
  }
}

void
ls_thread_join(struct ls_thread_t* thrd)
{
  if (NULL != thrd && NULL != thrd->thread) {
    WaitForSingleObject(thrd->thread, INFINITE);
    CloseHandle(thrd->thread);
    thrd->thread = NULL;
  }
}
#else
static void*
ls_thread_routine(void* arg)
{
  struct ls_thread_t* thrd = (struct ls_thread_t*)arg;
  if (NULL == thrd)
    return NULL;

  if (NULL != thrd->routine)
    thrd->routine(thrd->argument);

  return NULL;
}

struct ls_thread_t*
ls_thread_create(thread_routine_t routine, void* arg)
{
  struct ls_thread_t* thrd = (struct ls_thread_t*)malloc(sizeof(struct ls_thread_t));
  if (NULL != thrd) {
    thrd->routine = routine;
    thrd->argument = arg;

    pthread_create(&thrd->thread_id, NULL, ls_thread_routine, thrd);
  }
  return thrd;
}

void
ls_thread_destroy(struct ls_thread_t* thrd)
{
  if (NULL != thrd) {
    ls_thread_join(thrd);
    free(thrd);
  }
}

void
ls_thread_join(struct ls_thread_t* thrd)
{
  if (NULL != thrd && 0 != thrd->thread_id) {
    pthread_join(thrd->thread_id, 0);
    thrd->thread_id = 0;
  }
}
#endif
