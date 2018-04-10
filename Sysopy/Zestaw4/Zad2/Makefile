GCC = gcc -Wall -std=c11 -ggdb
all: build clean

build:
	$(GCC) -o child child.c
	$(GCC) -o parent parent.c

clean:
	rm -f *.o
