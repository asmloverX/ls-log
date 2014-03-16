BIN	= ls-log.lib
CC	= cl -c -nologo
CFLAG	= -O2 -W4 -MT -GS -Zi -Fd"vc.pdb" -DNDEBUG -D_CRT_SECURE_NO_DEPRECATE\
	-D_CRT_NONSTDC_NO_WARNINGS -DLS_MT -DLS_WIN32_MT -DLS_LOG_ON
LIB	= lib -nologo
RM	= rm
SRCS	= ./src/ls_array.c ./src/ls_buffer.c ./src/ls_circularbuffer.c\
	./src/ls_ini.c ./src/ls_lock.c ./src/ls_log.c ./src/ls_time.c\
	./src/ls_writedef.c ./src/ls_writefile.c ./src/ls_writegui.c\
	./src/ls_writelog.c
OBJS	= ls_array.obj ls_buffer.obj ls_circularbuffer.obj ls_ini.obj\
	ls_lock.obj ls_log.obj ls_time.obj ls_writedef.obj\
	ls_writefile.obj ls_writegui.obj ls_writelog.obj

$(BIN):$(OBJS)
	$(LIB) -out:$(BIN) $(OBJS)

$(OBJS):$(SRCS)
	$(CC) $(CFLAG) $(SRCS)


clean:
	$(RM) $(BIN) *.obj *.pdb
