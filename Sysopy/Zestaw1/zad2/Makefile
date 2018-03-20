GCC = gcc -Wall -std=c11 -ggdb
M_PATH = main
OUTPUT = raport2.txt

all: static shared trivial test fresh

static:
	$(GCC) -c charray.c
	ar rcs charray.a charray.o
	make clean

shared:
	$(GCC) -fPIC -c charray.c
	$(GCC) -fPIC -shared charray.o -o charray.so
	make clean

trivial:
	$(GCC) main.c charray.c -o $(M_PATH)
	make clean

clean:
	rm -f *.o

fresh: clean
	rm -f *.a *.so $(M_PATH)

test:
	echo TESTY DLA TABLICY DYNAMICZNEJ:  > $(OUTPUT)
	./$(M_PATH) 0 1000 500 500 10 >> $(OUTPUT)
	./$(M_PATH) 0 5000 1000 300 13 >> $(OUTPUT)
	./$(M_PATH) 0 10000 10000 3000 3 >> $(OUTPUT)
	./$(M_PATH) 0 500 2000 300 20 >> $(OUTPUT)
	./$(M_PATH) 0 4000 20 3000 20 >> $(OUTPUT)
	./$(M_PATH) 0 300 8000 100 4 >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo TESTY DLA TABLICY STATYCZNEJ:  >> $(OUTPUT)
	./$(M_PATH) 1 1024 512 512 12 >> $(OUTPUT)
	./$(M_PATH) 1 5000 1000 300 13 >> $(OUTPUT)
	./$(M_PATH) 1 9999 9999 3000 3 >> $(OUTPUT)
	./$(M_PATH) 1 500 2000 300 20 >> $(OUTPUT)
	./$(M_PATH) 1 4000 20 3000 20 >> $(OUTPUT)
	./$(M_PATH) 1 300 8000 100 4 >> $(OUTPUT)
	cat $(OUTPUT)