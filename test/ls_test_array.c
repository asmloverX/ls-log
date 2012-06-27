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
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../inc/ls_common.h"
#include "../src/ls_array.h"

#include "ls_test.h"


static void __elem_visitor(void* p, void* arg)
{
  double* d = (double*)p;

  UNUSED_PARAM(arg)

  fprintf(stdout, "  ===> the free object is : 0x%08x, value is : %.3lf\n", p, *d);
  free(p);
}

void ls_test_array(void* arg)
{
  int     i; 
  void*   elem;
  double* ld;
  struct ls_Array* larray = ls_arrayCreate(10);

  UNUSED_PARAM(arg)
  fprintf(stdout, "test unit of ls_Array module\n");

  fprintf(stdout, "  after create ls_Array the size of array is : %d\n\n", ls_arraySize(larray));

  fprintf(stdout, "  insert 10 elements into ls_Array\n");
  srand((unsigned int)time(0));
  for (i = 0; i < 10; ++i)
  {
    double* d = (double*)malloc(sizeof(double));
    *d = rand() % 1000 * 1.01;
    fprintf(stdout, "    ===> [%02d] insert object address is : 0x%08x\n", i, d);
    ls_arrayInsert(larray, d);
  }
  fprintf(stdout, "  after insert 10 elements the size of array is : %d\n", ls_arraySize(larray));
  for (i = 0; i < ls_arraySize(larray); ++i)
  {
    double* d = (double*)ls_arrayGet(larray, i);
    fprintf(stdout, "[%02d] = %.3lf\t", i, *d);
  }
  fprintf(stdout, "\n");

  elem = ls_arrayErase(larray, 0);
  fprintf(stdout, "  after erase [0] element the size of array is : %d\n", ls_arraySize(larray));
  for (i = 0; i < ls_arraySize(larray); ++i)
  {
    double* d = (double*)ls_arrayGet(larray, i);
    fprintf(stdout, "[%02d] = %.3lf\t", i, *d);
  }
  fprintf(stdout, "\n  ===> the free element address is : 0x%08x\n\n", elem);
  free(elem);

  ld = (double*)malloc(sizeof(double));
  *ld = 20120621.120;
  elem = ls_arrayGet(larray, 5);
  ls_arraySet(larray, 5, ld);
  fprintf(stdout, "  after set another element of ls_Array (free object is : 0x%08x [%.3lf], insert object is : 0x%08x)\n", elem, *(double*)elem, ld);
  free(elem);
  for (i = 0; i < ls_arraySize(larray); ++i)
  {
    double* d = (double*)ls_arrayGet(larray, i);
    fprintf(stdout, "[%02d] = %.3lf\t", i, *d);
  }
  fprintf(stdout, "\n");

  ls_arrayForeach(larray, __elem_visitor, NULL);

  fprintf(stdout, "\n  after clear the ls_Array\n");
  ls_arrayClear(larray);
  fprintf(stdout, "    length of ls_Array is : %d\n\n", ls_arraySize(larray));

  ls_arrayRelease(&larray);
}
