CC = gcc
TARGET = pps

all:
	$(CC) pps.c stat.c statm.c -o ${TARGET} -Wall -pedantic -lncurses

run:
	./${TARGET}

test: all run

clean:
	rm ${TARGET};
	
debug:
	$(CC) -g pps.c stat.c statm.c -o ${TARGET} -Wall -pedantic -lncurses
