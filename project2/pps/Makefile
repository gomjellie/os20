CC = gcc
TARGET = pps
SRCS = pps.c stat.c statm.c process.c cmdline.c proc.c status.c
OPT = --heap, 4194304, --stack, 4194304

all:
	$(CC) $(SRCS) -o ${TARGET} -Wall -pedantic -lncurses

run:
	./${TARGET}

test: all run

clean:
	rm ${TARGET};
	
debug:
	$(CC) -g $(SRCS) -o ${TARGET} -Wall -pedantic -lncurses
