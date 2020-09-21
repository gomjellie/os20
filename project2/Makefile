CC = gcc
TARGET = ssu_shell
SRCS = ssu_shell.c
OPT = -Wall -pedantic 

all:
	$(CC) $(OPT) $(SRCS) -o ${TARGET}

run:
	./${TARGET}

test: all run

clean:
	rm -f ${TARGET};
	
debug:
	$(CC) -g $(SRCS) -o ${TARGET} $(OPT)
