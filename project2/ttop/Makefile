CC = gcc

all:
	$(CC) ttop.c stat.c statm.c -o ttop -Wall -pedantic -lncurses

run:
	./ttop

test: all run

clean:
	rm ttop;
	
debug:
	$(CC) -g ttop.c stat.c statm.c -o ttop -Wall -pedantic -lncurses
