CC=gcc
CFLAGS=-Wall -std=gnu11

.PHONY: clean

all: main

main: main.c
	$(CC) $< -o $@ $(CFLAGS)

clean:
	rm -rf out main