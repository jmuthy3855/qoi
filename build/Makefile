CC=gcc -g
SRC=../src
INC=../include
INCSDL2=../libs/SDL2/include/SDL2
LIBSDL2=../libs/SDL2/lib
OBJ=.

qoi.x: main.o test.o qoi.o
	$(CC) -I$(INC) main.o test.o qoi.o -o qoi.x

sdl.x: sdltest.o
	$(CC) -I$(INCSDL2) -I$(INC) -L $(LIBSDL2) sdltest.o -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -o sdl.x

main.o: $(SRC)/main.c $(INC)/main.h
	$(CC) -I$(INC) -c $(SRC)/main.c

test.o: $(SRC)/test.c $(INC)/test.h
	$(CC) -I$(INC) -c $(SRC)/test.c

qoi.o: $(SRC)/qoi.c $(INC)/qoi.h
	$(CC) -I$(INC) -c $(SRC)/qoi.c

sdltest.o: $(SRC)/sdltest.c
	$(CC) -I$(INCSDL2) -c $(SRC)/sdltest.c

clean:
	rm *.o *.x