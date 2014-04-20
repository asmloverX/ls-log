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
#include <string.h>
#include "../inc/ls_common.h"

#include "ls_test.h"



/*!
 *! @struct ls_test
 *! @brief test unit infromation
 */
struct ls_test {
  char* option;         /*!< test unit command  */
  void (*unit)(void*);  /*!< test unit function */
};


static void ls_test_help(void* arg)
{
  UNUSED_PARAM(arg)

  fprintf(stdout, 
    "ls-log test unit version 0.1 - LoggingSystem - Copyright (c) 2012 ASMlover\n"
    "usage : ls-log [option]\n\n"
    "General options:\n"
    "  -help      display the document of the ls-log test unit\n"
    "  -lock      display the test unit of ls_Lock module\n"
    "  -buffer    display the test unit of ls_Buffer module\n"
    "  -cirbuf    display the test unit of ls_CircularBuffer module\n"
    "  -time      display the test unit of ls_LogTime module\n"
    "  -array     display the test unit of ls_Array module\n"
    "  -ini       display the test unit of ls_Ini module\n"
    "  -wdef      display the test unit of ls_WriteDef module\n"
    "  -wgui      display the test unit of ls_WriteGui module\n"
    "  -wfile     display the test unit of ls_WriteFile module\n"
    "  -log       display the test unit of ls-log\n"
    );
}


int 
main(int argc, char* argv[], char* envp[])
{
  /*!< test unit functions list */
  static struct ls_test units[] = {
    { "-help",   ls_test_help           }, 
    { "-lock",   ls_test_lock           }, 
    { "-buffer", ls_test_buffer         }, 
    { "-cirbuf", ls_test_circularBuffer }, 
    { "-time",   ls_test_time           }, 
    { "-array",  ls_test_array          }, 
    { "-ini",    ls_test_ini            }, 
    { "-wdef",   ls_test_writedef       }, 
    { "-wgui",   ls_test_writegui       }, 
    { "-wfile",  ls_test_writefile      }, 
    { "-log",    ls_test_log            }, 
  };

  UNUSED_PARAM(envp)
  if (argc < 2)
    ls_test_help(NULL);
  else 
  {
    int i, found = 0;
    for (i = 0; i < countof(units); ++i)
    {
      if (0 == stricmp(argv[1], units[i].option))
      {
        found = 1;
        units[i].unit((3 == argc) ? argv[2] : NULL );
        break;
      }
    }

    if (!found)
      ls_test_help(NULL);
  }

  return 0;
}
