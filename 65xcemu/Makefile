OBJS = obj/cpu.o obj/6502cpu.o
CFLAGS = -g
CC = C:\Qt\Qt5.15.0\Tools\mingw810_64\bin\g++.exe
INCLUDE = 
LIBS = -lm
SOURCE = 

6502emu: src/cpu.cpp src/6502cpu.cpp
	$(CC) $(CFLAGS) $? -o $@

clean:
	-rm -f *.o core *.core

.cpp.o:
	${CC} ${CFLAGS} ${INCLUDES} -c $>