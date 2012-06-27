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
#include <time.h>
#include <sys/timeb.h>
#include "../inc/ls_common.h"

#include "ls_time.h"



int ls_getLogTime(struct ls_LogTime* t)
{
  if (NULL != t)
  {
    struct timeb tb = {0};
    struct tm* local = NULL;

    ftime(&tb);
    local = localtime(&tb.time);
    t->year = (unsigned short)(local->tm_year + 1900);
    t->mon  = (unsigned char)local->tm_mon + 1;
    t->day  = (unsigned char)local->tm_mday;
    t->hour = (unsigned char)local->tm_hour;
    t->min  = (unsigned char)local->tm_min;
    t->sec  = (unsigned char)local->tm_sec;
    t->millitm = tb.millitm;

    return LS_Success;
  }
  return LS_Failed_InvalidParameters;
}

int ls_timeDuration(struct ls_LogTime beg, struct ls_LogTime end)
{
  if (end.hour < beg.hour)
    end.hour += 24;

  return ((60 * end.hour + end.min) - (60 * beg.hour + beg.min));
}
