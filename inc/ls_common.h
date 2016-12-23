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
#ifndef LITESPHINX_COMMON_INCLUDED_H
#define LITESPHINX_COMMON_INCLUDED_H

#if defined(_WIN32) || defined(_WIN64) || defined(_WINDOWS_) || defined(_MSC_VER)
# define LS_WINDOWS
#elif defined(__linux__)
# define LS_LINUX
#elif defined(__APPLE__) || defined(__MACH__)
# define LS_DARWIN
#else
# error "Unsupport unknown platform !!!"
#endif

#if !defined(LS_WINDOWS)
# define LS_POSIX
#endif

#define UNUSED_PARAM(arg) (void)arg
#define countof(x)        (sizeof((x)) / sizeof(*(x)))

#define LS_ERROR_LEN      (-1)    /* error length return */
#define LS_BUFFERLEN_DEF  (1024)  /* default buffer length */

typedef enum ls_boolean_t {
  False = 0,
  True  = 1,
} ls_boolean_t;

enum ls_error_t {
  /* return code of functions call */
  LS_ELEN           = -1, /* error - error length */
  LS_OK             = 0,  /* function success */
  LS_ERROR          = 1,  /* function failed */
  LS_EINVALPARAM    = 2,  /* error - invalid parameters */
  LS_ELOADCONFIG    = 3,  /* error - load configure failed */
  LS_ECSEVERITYARR  = 4,  /* error - create severity array failed */
  LS_ECMODULESARR   = 5,  /* error - create modules array failed */
  LS_ECWRITELOG     = 6,  /* error - create object `ls_write_log_t` failed */
};

#endif /* LITESPHINX_COMMON_INCLUDED_H */
