CC = gcc
TARGET = pps
SRCS = pps.c stat.c statm.c process.c cmdline.c proc.c

all:
	$(CC) $(SRCS) -o ${TARGET} -Wall -pedantic -lncurses

run:
	./${TARGET}

test: all run

clean:
	rm ${TARGET};
	
debug:
	$(CC) -g $(SRCS) -o ${TARGET} -Wall -pedantic -lncurses
