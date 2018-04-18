GCC = cc -Wall -std=c11 -ggdb

all: build run

build:
	$(GCC) -o master master.c
	$(GCC) -o slave slave.c

clean:
	rm -f *.o