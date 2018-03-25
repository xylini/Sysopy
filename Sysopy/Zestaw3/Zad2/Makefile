GCC = gcc -Wall -std=c11 -ggdb
M_PATH = main

all: build clean tests

build:
	$(GCC) -o $(M_PATH) main.c

clean:
	rm -f *.o

tests: