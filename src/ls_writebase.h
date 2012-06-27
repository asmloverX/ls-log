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
#ifndef __LS_WRITEBASE_HEADER_H__
#define __LS_WRITEBASE_HEADER_H__


#define LS_MODULENAME_LEN   (64)  /*!< module name length */



struct ls_Array;
/*!
 *! @struct ls_Module
 *! @brief information of module
 */
struct ls_Module {
  int   id;                       /*!< id of module   */
  char  name[LS_MODULENAME_LEN];  /*!< name of module */
};
/*!
 *! @struct ls_SeverityBuffer
 *! @brief severity buffer information
 */
struct ls_SeverityBuffer {
  int                       id;     /*!< id of severity buffer              */
  struct ls_CircularBuffer* buffer; /*!< circular buffer of severity buffer */
};


/*!
 *! @struct ls_WriteBase
 *! @brief base interfaces of ls_WriteBase
 */
struct ls_WriteBase {
  void* object; /*!< object of ls_WriteBase */

  void (*init)(struct ls_WriteBase*, struct ls_Array* logModules);
  void (*uninit)(struct ls_WriteBase*);

  /*!
   *! struct ls_WriteBase*  - ls_WriteBase object
   *! int                   - message severity level
   *! int                   - module index
   *! const char*           - log message
   *! int                   - length of log message
   */
  void (*writeMessage)(struct ls_WriteBase*, int, int, const char*, int); /*!< write message interface */
  void (*writeFlush)(struct ls_WriteBase*);                               /*!< flush message interface */
};


/*!
 *! [Display_Console]
 *!   default display mode interfaces create & release
 */
extern struct ls_WriteBase* ls_writeDefCreate(void);
extern void ls_writeDefRelease(struct ls_WriteBase** p);

/*!
 *! [Display_GUI]
 *!   display log message to gui window interfaces create & release
 */
extern struct ls_WriteBase* ls_writeGuiCreate(void);
extern void ls_writeGuiRelease(struct ls_WriteBase** p);

/*!
 *! [Display_File]
 *!   display log message to files interfaces create & release
 */
extern struct ls_WriteBase* ls_writeFileCreate(void);
extern void ls_writeFileRelease(struct ls_WriteBase** p);



#endif  /* __LS_WRITEBASE_HEADER_H__ */
