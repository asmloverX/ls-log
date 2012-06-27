BIN 	= ls-log.exe
CC 		= cl -c -nologo
CFLAG = -O2 -W4 -MD -GS -Zi -Fd"vc.pdb" -DNDEBUG -D_CRT_SECURE_NO_DEPRECATE\
				-D_CRT_NONSTDC_NO_WARNINGS -DLS_MT -DLS_WIN32_MT -DLS_LOG_ON
LINK 	= link -nologo
LFLAG = -DEBUG -PDB:"ls-log.pdb" -manifest -manifestfile:$(BIN).manifest\
				-manifestuac:no
MT 		= mt -nologo
RM 		= rm
SRCS 	= ./src/ls_buffer.c ./src/ls_lock.c ./src/ls_circularbuffer.c\
				./src/ls_log.c ./src/ls_writelog.c ./src/ls_writedef.c\
				./src/ls_writefile.c ./src/ls_time.c ./src/ls_array.c\
				./src/ls_ini.c ./src/ls_writegui.c\
				\
				./test/ls_test.c ./test/ls_test_lock.c ./test/ls_test_buffer.c\
				./test/ls_test_circularbuffer.c ./test/ls_test_writedef.c\
				./test/ls_test_time.c ./test/ls_test_array.c ./test/ls_test_ini.c\
				./test/ls_test_writefile.c ./test/ls_test_log.c ./test/ls_test_writegui.c
OBJS 	= ls_buffer.obj ls_lock.obj ls_circularbuffer.obj ls_log.obj\
				ls_writelog.obj ls_writedef.obj ls_writefile.obj ls_time.obj\
				ls_array.obj ls_ini.obj ls_writegui.obj\
				\
				ls_test.obj ls_test_lock.obj ls_test_buffer.obj ls_test_circularbuffer.obj\
				ls_test_writedef.obj ls_test_time.obj ls_test_array.obj ls_test_ini.obj\
				ls_test_writefile.obj ls_test_log.obj ls_test_writegui.obj

$(BIN):$(OBJS)
	$(LINK) -out:$(BIN) $(OBJS) $(LFLAG)
	$(MT) -manifest $(BIN).manifest -outputresource:$(BIN);1

$(OBJS):$(SRCS)
	$(CC) $(CFLAG) $(SRCS)

clean:
	$(RM) $(BIN) *.obj *.manifest *.ilk *.pdb
