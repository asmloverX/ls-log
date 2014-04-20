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
#ifndef __LS_COMMON_HEADER_H__
#define __LS_COMMON_HEADER_H__

#define UNUSED_PARAM(arg) { (arg); }
#define countof(x)        (sizeof((x)) / sizeof(*(x)))

#define LS_ERROR_LEN      (-1)    /*!< error length return    */
#define LS_BUFFERLEN_DEF  (1024)  /*!< default buffer length  */


/*!
 *! @enum LS_ResultType
 *! @brief Result type
 */
enum LS_ResultType {
  LS_Success                    = 0, /*!< success                               */
  LS_Failed                     = 1, /*!< failed                                */
  LS_Failed_InvalidParameters   = 2, /*!< failed - invalid parameters           */
  LS_Failed_LoadConfig          = 3, /*!< failed - load configure failed        */
  LS_Failed_CreateSeverityArray = 4, /*!< failed - create severity array failed */
  LS_Failed_CreateModulesArray  = 5, /*!< failed - create modules array failed  */
  LS_Failed_CreateWriteLog      = 6, /*!< failed - create ls_WriteLog failed    */
};

#endif  /* __LS_COMMON_HEADER_H__ */
