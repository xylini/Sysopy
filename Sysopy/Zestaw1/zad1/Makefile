CC = gcc -Wall -std=c11
all: static shared clean

static:
	$(CC) -c charray.c
	ar rcs libcharray.a charray.o

shared:
	$(CC) -c -fPIC charray.c
	$(CC) -shared -fPIC -o libcharray.so charray.o

clean:
	rm -f *.o