BIN		= ls-demo.exe
CC 		= cl -c -nologo
CFLAG	= -O2 -W4 -MDd -GS -Zi -Fd"vc.pdb" -DNDEBUG -D_CRT_SECURE_NO_DEPRECATE\
				-D_CRT_NONSTDC_NO_WARNINGS -DLS_LOG_ON -I"../inc"
LINK	= link -nologo
LFLAG	= -DEBUG -PDB:"ls-demo.pdb" -manifest -manifestfile:$(BIN).manifest\
				-manifestuac:no kernel32.lib user32.lib gdi32.lib
MT 		= mt -nologo
RM 		= rm
SRCS 	= main.c
OBJS 	= main.obj

$(BIN):$(OBJS)
	$(LINK) -out:$(BIN) $(OBJS) $(LFLAG)
	$(MT) -manifest $(BIN).manifest -outputresource:$(BIN);1

$(OBJS):$(SRCS)
	$(CC) $(CFLAG) $(SRCS)

clean:
	$(RM) $(BIN) *.obj *.manifest *.pdb *.ilk
