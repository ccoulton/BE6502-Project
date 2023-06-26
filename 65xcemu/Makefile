OBJS = obj/cpu.o obj/6502cpu.o
CFLAGS = -std=c++11 -g
CC = g++
INCLUDE = 
LIBS = -lm
SOURCE = 

6502emu: src/cpu.cpp src/6502cpu.cpp
	$(CC) $(CFLAGS) $? -o $@

clean:
	-rm -f *.o core *.core

.cpp.o:
	${CC} ${CFLAGS} ${INCLUDES} -c $>