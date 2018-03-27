GCC = gcc -Wall -std=c11 -ggdb
M_PATH = main

all: build clean tests

build:
	$(GCC) -o $(M_PATH) main.c
	$(GCC) -o cpu_limit_stress cpu_limit_stress.c
	$(GCC) -o data_limit_stress data_limit_stress.c

clean:
	rm -f *.o

tests: