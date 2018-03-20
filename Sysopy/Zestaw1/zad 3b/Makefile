GCC = gcc -Wall -std=c11 -ggdb
M_PATH = main
OUTPUT = results3b.txt
O_LVL = 0
#3 lvl's of optimization: 1,2,3 = O_LVL


all: static shared clean make_static make_dynamic make_shared

static:
	$(GCC) -c charray.c
	ar rcs charray.a charray.o

shared:
	$(GCC) -c -fPIC charray.c
	$(GCC) -shared -fPIC -o charray.so charray.o

clean:
	rm -f *.o

make_static:
	make static
	$(GCC) -o $(M_PATH) main.c charray.a -I . -O$(O_LVL)
	echo +-----------------------------+ > $(OUTPUT)
	echo \|  OPTIMALIZATION LVL: $(O_LVL)      \| >> $(OUTPUT)
	echo +-----------------------------+ >> $(OUTPUT)
	echo STATIC_LIBRARY_TESTS! >> $(OUTPUT)
	make tests
	echo >> $(OUTPUT)

make_dynamic:
	make shared
	$(GCC) -o $(M_PATH) main_dynamic.c -I . -O$(O_LVL)
	echo DYNAMIC_LIBRARY_TESTS! >> $(OUTPUT)
	make tests
	echo >> $(OUTPUT)


make_shared:
	make shared
	$(GCC) -o $(M_PATH) main.c -L. charray.so -I . -O$(O_LVL)
	echo SHARED_LIBRARY_TESTS! >> $(OUTPUT)
	make tests


tests:
	echo TESTY DLA TABLICY DYNAMICZNEJ:  >> $(OUTPUT)
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