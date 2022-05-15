CC=gcc -g
SRC=../src
INC=../include
INCSDL2=../libs/SDL2/include/SDL2
LIBSDL2=../libs/SDL2/lib
OBJ=.

qoi.x: main.o qoi.o display_qoi.o
	$(CC) -I$(INCSDL2) -I$(INC) -L $(LIBSDL2) main.o qoi.o display_qoi.o -lmingw32 -lSDL2main -lSDL2 -o qoi.x

main.o: $(SRC)/main.c $(INC)/main.h
	$(CC) -I$(INC) -c $(SRC)/main.c

qoi.o: $(SRC)/qoi.c $(INC)/qoi.h
	$(CC) -I$(INC) -c $(SRC)/qoi.c

display_qoi.o: $(SRC)/display_qoi.c
	$(CC) -I$(INC) -I$(INCSDL2) -c $(SRC)/display_qoi.c

clean:
	rm *.o *.x