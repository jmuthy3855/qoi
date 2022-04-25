CC=gcc -g
SRC=../src
INC=../include
INCSDL2=../libs/SDL2/include/SDL2
LIBSDL2=../libs/SDL2/lib
OBJ=.

qoi.x: main.o test.o qoi.o display_qoi.o
	$(CC) -I$(INCSDL2) -I$(INC) -L $(LIBSDL2) main.o test.o qoi.o display_qoi.o -lmingw32 -lSDL2main -lSDL2 -o qoi.x

sdl.x: sdltest.o
	$(CC) -I$(INCSDL2) -I$(INC) -L $(LIBSDL2) sdltest.o -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -o sdl.x

main.o: $(SRC)/main.c $(INC)/main.h
	$(CC) -I$(INC) -c $(SRC)/main.c

test.o: $(SRC)/test.c $(INC)/test.h
	$(CC) -I$(INC) -c $(SRC)/test.c

qoi.o: $(SRC)/qoi.c $(INC)/qoi.h
	$(CC) -I$(INC) -c $(SRC)/qoi.c

display_qoi.o: $(SRC)/display_qoi.c
	$(CC) -I$(INC) -I$(INCSDL2) -c $(SRC)/display_qoi.c

clean:
	rm *.o *.x