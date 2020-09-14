GNU = gcc

all:
	$(GNU) main.c ttop.c -o ttop -Wall -pedantic -lncurses

run:
	./ttop

test: all run

clean:
	rm ttop;
	
debug:
	$(GNU) -g main.c ttop.c -o ttop -Wall -pedantic -lncurses
