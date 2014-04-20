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
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <ls_common.h>
#include <ls_log.h>

#pragma comment(lib, "ls-log.lib")

#define UM_DEMO   (WM_USER + 1234)



static void getData(int size, short* data)
{
  int i, len = size / sizeof(short);
  for (i = 0; i < len; ++i)
    data[i] = (short)rand() % 1000;
}

static void showData(int index, short* data, int size)
{
  int i, len = size / sizeof(short);

  fprintf(stdout, "begin show data index [%d]\n", index);
  for (i = 0; i < len; ++i)
    fprintf(stdout, "[%03d] = %03d\t", i, data[i]);
  fprintf(stdout, "\nend show data index [%d]\n\n", index);
}

static void threadDemo(void)
{
  short data[200];
  int   i;

  for (i = 0; i < 1000; ++i)
  {
    getData(sizeof(data), data);
    showData(i, data, sizeof(data));
    Sleep(5);
  }
}

static UINT WINAPI sThread(void* arg)
{
  HANDLE start_ev = (HANDLE)arg;
  MSG    msg;

  PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
  SetEvent(start_ev);

  for (; ;)
  {
    GetMessage(&msg, NULL, 0, 0);

    switch (msg.message)
    {
    case WM_QUIT:
      ls_log(Display_File, Severity_Message, 1, "stop message thread\n");
      goto __quit;
    case UM_DEMO:
      ls_log(Display_File, Severity_Message, 1, "thread demo\n");
      threadDemo();
      break;
    }
  }

__quit:
  return 0u;
}

static HANDLE startMsgThread(UINT* pid)
{
  HANDLE start_ev = CreateEvent(NULL, TRUE, FALSE, NULL);
  HANDLE hThread  = NULL;

  if (NULL == start_ev)
  {
    ls_log(Display_File, Severity_Error, 0, "create message thread start event failed ...\n");
    return NULL;
  }
  hThread = (HANDLE)_beginthreadex(NULL, 0, sThread, start_ev, 0, pid);
  if (NULL != hThread)
    WaitForSingleObject(start_ev, INFINITE);
  CloseHandle(start_ev);

  return hThread;
}

static void stopMsgThread(HANDLE hThread, DWORD pid)
{
  if (NULL != hThread)
  {
    PostThreadMessage(pid, WM_QUIT, 0, 0);

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
  }
}

static int getCommand(char* cmd)
{
  int  i = 0;
  char c = 0;

  for (; ;)
  {
    c = (char)fgetc(stdin);
    if ('\r' == c || '\n' == c)
    {
      cmd[i] = 0;
      break;
    }
    cmd[i++] = c;
  }

  return (i > 0);
}

static void help(void)
{
  fprintf(stdout, 
    "  exit   - exit to application\n"
    "  help   - help menu\n"
    "  start  - run message thread\n"
    "  demo   - demo of threadDemo\n"
    );
}

int 
main(int argc, char* argv[], char* envp[])
{
  char   cmd[128] = {0};
  HANDLE hThread = NULL;
  UINT   pid     = 0;

  UNUSED_PARAM(argc)
  UNUSED_PARAM(argv)
  UNUSED_PARAM(envp)
  ls_init();

  fprintf(stdout, "Welcome to ls-log demo application, 'help' for help menu\n");
  for (; ;)
  {
    fprintf(stdout, "> ");
    if (0 == getCommand(cmd))
      continue;
    else if (0 == stricmp("exit", cmd))
    {
      stopMsgThread(hThread, pid);
      ls_log(Display_File, Severity_Message, 0, "exit to demo application\n");
      break;
    }
    else if (0 == stricmp("help", cmd))
    {
      ls_log(Display_File, Severity_Message, 0, "help --- %s\n", __FUNCSIG__);
      help();
    }
    else if (0 == stricmp("start", cmd))
    {
      if (NULL == hThread)
      {
        hThread = startMsgThread(&pid);
        if (NULL == hThread)
          ls_log(Display_File, Severity_Error, 0, "start message thread failed\n");
        else 
          ls_log(Display_File, Severity_Message, 0, "start message thread successed\n");
      }
    }
    else if (0 == stricmp("demo", cmd))
      PostThreadMessage(pid, UM_DEMO, 0, 0);
    else 
      break;
  }

  ls_uninit();
  return 0;
}
