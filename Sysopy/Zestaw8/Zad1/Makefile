CC=gcc
CFLAGS=-Wall -std=gnu99 -lpthread -lm

.PHONY: clean

all: main

main: main.c
	$(CC) $< -o $@ $(CFLAGS)

test: main
	./main 1 ./20filtr.txt ./mountain.pgm ./filtered_mountain.pgm > Times.txt
	./main 2 ./20filtr.txt ./mountain.pgm ./filtered_mountain.pgm >> Times.txt
	./main 4 ./20filtr.txt ./mountain.pgm ./filtered_mountain.pgm >> Times.txt
	./main 8 ./20filtr.txt ./mountain.pgm ./filtered_mountain.pgm >> Times.txt

clean:
	rm -rf out main