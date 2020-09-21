CC = gcc
CC = gcc
TARGET = ~/ttop
SRCS = ttop.c stat.c statm.c
OPT = -Wall -pedantic 
LIB = -lncurses

all:
	$(CC) $(SRCS) -o $(TARGET) $(OPT) $(LIB)

run:
	./ttop

test: all run

clean:
	rm -f $(TARGET)
	
debug:
	$(CC) -g $(SRCS) -o $(TARGET) $(OPT) $(LIB)
