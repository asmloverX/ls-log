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
#include <string.h>
#include "../inc/ls_common.h"
#include "ls_array.h"

#define LS_ARRAYLEN_DEF (8) /*!< default array length */

/*!
 *! @struct ls_array_t
 *! @brief Array内部结构
 *!        该Array的设计是内部存储的时对象的指针, 同时
 *!        Array内部不负责对象的创建和销毁, 由使用者自
 *!        行控制
 *! @attention 该Array只是简单的存数数据对象, 其内部并不保证线程安全
 *!
 *! storage - 用来存储该Array可以存储的对象数量
 *! begin/end - 用来计算当前的array存储对象的最后位置, 以及计算存储的对象数
 */
struct ls_array_t {
  void** elements;  /*!< elements of Array */
  int    storage;   /*!< storage of Array  */
  void** begin;     /*!< begin address     */
  void** end;       /*!< end address       */
};

/*!
 *! Array 自增长函数, 当storage空间不足的时候, 自动增长Array空间
 *! 以便存储新的Array元素
 *!
 *! 这里设计使用的是一种比较简单的方法, 当存储空间满了之后, 就放大
 *! 为原来存储空间的2倍, 并把原来的元素拷贝到新的空间当中~~~
 */
static int
ls_array_regrow(struct ls_array_t* self)
{
  int old_size = self->end - self->begin;
  int new_storage = self->storage * 2;
  void** new_elements = (void**)malloc(new_storage * sizeof(void*));
  if (NULL == new_elements)
    return LS_ERROR;

  if (NULL != self->elements) {
    memcpy(new_elements, self->elements, sizeof(void*) * old_size);
    free(self->elements);
  }
  self->elements = new_elements;
  self->begin = self->elements;
  self->end = self->elements + old_size;
  self->storage = new_storage;

  return LS_OK;
}

struct ls_array_t*
ls_array_create(int storage)
{
  struct ls_array_t* self;
  size_t size = sizeof(struct ls_array_t);

  self = (struct ls_array_t*)calloc(size, sizeof(char));
  if (NULL != self) {
    self->storage = (storage > LS_ARRAYLEN_DEF ? storage : LS_ARRAYLEN_DEF);
    self->elements = (void**)calloc(self->storage * sizeof(void*), sizeof(char));
    if (NULL == self->elements) {
      ls_array_destroy(self);
      self = NULL;
    }
    else {
      self->begin = self->end = self->elements;
    }
  }
  return self;
}

void
ls_array_destroy(struct ls_array_t* self)
{
  if (NULL != self) {
    if (NULL != self->elements)
      free(self->elements);
    free(self);
  }
}

void ls_array_clear(struct ls_array_t* self)
{
  if (NULL != self) {
    memset(self->elements, 0, sizeof(void*) * self->storage);
    self->begin = self->end = self->elements;
  }
}

int
ls_array_size(struct ls_array_t* self)
{
  return (NULL != self ? (self->end - self->begin) : LS_ERROR_LEN);
}

int
ls_array_insert(struct ls_array_t* self, void* value)
{
  if (NULL != self) {
    if (self->end >= self->begin + self->storage) {
      if (LS_OK != ls_array_regrow(self))
        return LS_ERROR;
    }
    *self->end = value;
    ++self->end;

    return LS_OK;
  }
  return LS_EINVALPARAM;
}

void*
ls_array_erase(struct ls_array_t* self, int index)
{
  void* ret = NULL;
  if (NULL != self && 0 <= index && index < (self->end - self->begin)) {
    ret = self->elements[index];
    memmove(self->begin + index, self->begin + index + 1, (self->end - self->begin - index - 1) * sizeof(void*));
    --self->end;
  }

  return ret;
}

void*
ls_array_get(struct ls_array_t* self, int index)
{
  if (NULL != self && 0 <= index && index < (self->end - self->begin))
    return self->elements[index];
  return NULL;
}

void
ls_array_set(struct ls_array_t* self, int index, void* value)
{
  if (NULL != self && 0 <= index && index < (self->end - self->begin))
    self->elements[index] = value;
}

int
ls_array_foreach(struct ls_array_t* self, void (*visitor)(void*, void*), void* arg)
{
  if (NULL != self && NULL != visitor) {
    int i, size = self->end - self->begin;
    for (i = 0; i < size; ++i)
      visitor(self->elements[i], arg);

    return LS_OK;
  }
  return LS_EINVALPARAM;
}
