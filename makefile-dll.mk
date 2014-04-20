# Copyright (c) 2014 ASMlover. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list ofconditions and the following disclaimer.
#
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materialsprovided with the
#    distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

BIN	= ls-log.dll
IMPLIB	= ls-log.lib
CC	= cl -c -nologo
CFLAG	= -O2 -W4 -MD -GS -Zi -Fd"vc.pdb" -DNDEBUG -D_CRT_SECURE_NO_DEPRECATE\
	-D_CRT_NONSTDC_NO_WARNINGS -DLS_MT -DLS_WIN32_MT -DLS_LOG_ON
DEFFILE	= ls-log.def
LINK	= link -nologo -dll -def:$(DEFFILE)
LFLAG = -DEBUG -PDB:"ls-log.pdb" -manifest -manifestfile:$(BIN).manifest\
	-manifestuac:no
MT	= mt -nologo
RC	= rc
RM	= rm
SRCS	= ./src/ls_array.c ./src/ls_buffer.c ./src/ls_circularbuffer.c\
	./src/ls_ini.c ./src/ls_lock.c ./src/ls_log.c ./src/ls_time.c\
	./src/ls_writedef.c ./src/ls_writefile.c ./src/ls_writegui.c\
	./src/ls_writelog.c
OBJS	= ls_array.obj ls_buffer.obj ls_circularbuffer.obj ls_ini.obj\
	ls_lock.obj ls_log.obj ls_time.obj ls_writedef.obj\
	ls_writefile.obj ls_writegui.obj ls_writelog.obj
RES	= ls-log.res

$(BIN):$(OBJS) $(RES)
	$(LINK) -out:$(BIN) $(OBJS) $(RES) $(LFLAG)
	$(MT) -manifest $(BIN).manifest -outputresource:$(BIN);1

$(OBJS):$(SRCS)
	$(CC) $(CFLAG) $(SRCS)

$(RES): ./res/ls-log.rc
	$(RC) -fo$(RES) ./res/ls-log.rc

clean:
	$(RM) $(BIN) *.obj *.manifest *.ilk *.exp *.lib *.pdb *.res
