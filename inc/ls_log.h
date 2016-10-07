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
#ifndef LITESPHINX_LOG_INCLUDED_H
#define LITESPHINX_LOG_INCLUDED_H

#ifdef __cplusplus
extern "C" {
#endif

enum ls_display_t {
  /* log信息输出的类型 */
  DISPLAY_CONSOLE = 0, /* 输出到shell(控制台窗口) */
  DISPLAY_GUI     = 1, /* 输出到GUI窗口中 */
  DISPLAY_FILE    = 2, /* 输出到指定文件中 */
  DISPLAY_DB      = 3, /* 输出到指定数据库看中 */
  DISPLAY_NETWORK = 4, /* 输出到网络的另一端 */

  DISPLAY_COUNT,
};

/*!
 *! @enum LS_DisplayType
 *! @brief display type of log information
 */
enum LS_DisplayType {
  Display_Console   = 0, /*!< display log information to console window            */
  Display_GUI       = 1, /*!< display log information to gui debug window          */
  Display_File      = 2, /*!< display log information to files                     */
  Display_Database  = 3, /*!< display log information to databases                 */
  Display_Network   = 4, /*!< display log information to other terminal by network */

  Display_Count     = 5,
};

enum ls_severity_t {
  /* 日志信息的类型 */
  SEVERITY_DEBUG  = 0,
  SEVERITY_MSG    = 1,
  SEVERITY_WARN   = 2,
  SEVERITY_ERROR  = 3,

  SEVERITY_COUNT,
};

/*!
 *! @enum LS_SeverityType
 *! @brief severity type of log information
 */
enum LS_SeverityType {
  Severity_Debug    = 0, /*!< debug informations   */
  Severity_Message  = 1, /*!< normal messages      */
  Severity_Warning  = 2, /*!< warning informations */
  Severity_Error    = 3, /*!< error informations   */

  Severity_Count    = 4,
};

extern int ls_log_init(void);
extern void ls_log_destroy(void);

/*!
 *! Usage:
 *!   ls_init - 在系统启动的时候初始化执行一些操作, 加载对应的资源
 *!   ls_uninit - 在系统退出之前用来回收资源
 */
extern int ls_init(void);     /*!< initiliaze logging system   */
extern void ls_uninit(void);  /*!< uninitialize logging system */

/*!
 *! 记录日志信息的操作接口, 使用方式类似printf系列的函数
 *!
 *! Usage:
 *!   ls_log(display, severity, module, fmt, ...)
 *!   @param [in] display: 该日志信息的显示模式, 取值见LS_DisplayType
 *!   @param [in] severity: 该日志信息的等级, 取值见LS_SeverityType
 *!   @param [in] module: 该日志信息所属的模块, 具体的模块ID取决于用户自己配置的配置文件
 *!   @param [in] fmt/...: 后面的参数使用类似于printf
 *!
 *! Example:
 *!   ls_log(Display_File, Severity_Message, 0, "This is log message test, value = %d\n", value);
 */
extern int ls_logx(int display, int severity, int module, char* file, char* func, int line, const char* fmt, ...);
extern int ls_logx_normal(int display, int severity, int module, const char* fmt, ...);
#if !defined(__VA_ARGS__) || (defined(_MSC_VER) && (_MSC_VER < 1400))
# if defined(LS_LOG_ON)
#   define ls_log  ls_logx_normal
# else
#   define ls_log
# endif
#else
# if defined(LS_WINDOWS)
#   if defined(LS_LOG_ON)
#     define ls_log(display, severity, module, fmt, ...)   ls_logx((display), (severity), (module), __FILE__, __FUNCTION__, __LINE__, (fmt), __VA_ARGS__)
#   else
#     define ls_log(display, severity, module, fmt, ...)
#   endif
# else
#   if defined(LS_LOG_ON)
#     define ls_log(display, severity, module, fmt, ...)   ls_logx((display), (severity), (module), __FILE__, __func__, __LINE__, (fmt), ##__VA_ARGS__)
#   else
#     define ls_log(display, severity, module, fmt, ...)
#   endif
# endif
#endif

#ifdef __cplusplus
}
#endif

#endif  /* LITESPHINX_LOG_INCLUDED_H */
