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
#include "../inc/ls_log.h"

#include "ls_writebase.h"

#define ls_WriteDef ls_WriteBase

static void ls_writeDefMessage(struct ls_WriteBase* p, int severity, int module, const char* msg, int len)
{
  UNUSED_PARAM(severity)
  UNUSED_PARAM(module)

  if (NULL != p && p == p->object && NULL != msg && len > 0)
    fprintf(stdout, "%s", msg);
}

struct ls_WriteBase* ls_writeDefCreate(void)
{
  struct ls_WriteDef* self;
  size_t size = sizeof(struct ls_WriteDef);

  self = (struct ls_WriteDef*)calloc(size, sizeof(char));
  if (NULL != self)
  {
    self->object = self;
    self->writeMessage = ls_writeDefMessage;
  }

  return self;
}

void ls_writeDefRelease(struct ls_WriteBase** p)
{
  if (NULL != *p)
  {
    free(*p);
    *p = NULL;
  }
}
