CC = gcc
TARGET = pps
SRCS = pps.c stat.c statm.c process.c cmdline.c proc.c status.c tty.c dev.c
OPT = -Wall -pedantic -lncurses

all:
	$(CC) $(OPT) $(SRCS) -o ${TARGET}

run:
	./${TARGET}

test: all run

clean:
	rm ${TARGET};
	
debug:
	$(CC) -g $(SRCS) -o ${TARGET} -Wall -pedantic -lncurses
