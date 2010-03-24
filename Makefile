CC=grc gcc

CFLAGS= -g -O2 -Wall --pedantic -Werror
LDFLAGS= -lSDL -lm -lSDL_image
EXE=scrpg
#CFLAGS= -O9

all: compile 	

compile:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(EXE) main.c
	
run: compile
	./$(EXE)
clean:
	rm -f scrpg *.o *.c.bak *.c# *.c~
tar:
	tar -czvf teh_tar.tar.gz *.c data Makefile
