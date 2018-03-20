GCC = gcc -Wall -std=c11 -ggdb
M_PATH = program
OUTPUT = wyniki.txt


all: build clean tests

build:
	$(GCC) -o $(M_PATH) main.c




clean:
	rm -f *.o

tests:
	#copy [source_file_name] [dest_file_name] [records] [length] [buffor_size] [sys or lib]
	#sort [file_name] [records] [length] [sys or lib]
	#generate [file_name] [records] [length]
	#s_generate [file_name] [records] [length]
	
	echo Testy dla sortowania: > $(OUTPUT)

	echo Ilosc rekordow: 800, rozmiar: 4 >> $(OUTPUT)
	./program s_generate sys_data 800 4 
	./program copy sys_data lib_data 800 4 1024 sys

	./program sort sys_data 800 4 sys >> $(OUTPUT)
	./program sort sys_data 800 4 lib >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo Ilosc rekordow: 800, rozmiar: 512 >> $(OUTPUT)
	./program generate sys_data 800 512
	./program copy sys_data lib_data 800 512 1024 sys

	./program sort sys_data 800 512 sys >> $(OUTPUT)
	./program sort sys_data 800 512 lib >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo Ilosc rekordow: 800, rozmiar: 4096 >> $(OUTPUT)
	./program generate sys_data 800 4096
	./program copy sys_data lib_data 800 4096 1024 sys

	./program sort sys_data 800 4096 sys >> $(OUTPUT)
	./program sort sys_data 800 4096 lib >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo Ilosc rekordow: 800, rozmiar: 8192 >> $(OUTPUT)
	./program generate sys_data 800 8192
	./program copy sys_data lib_data 800 8192 1024 sys

	./program sort sys_data 800 8192 sys >> $(OUTPUT)
	./program sort sys_data 800 8192 lib >> $(OUTPUT)
	

	echo Ilosc rekordow: 1200, rozmiar: 4 >> $(OUTPUT)
	./program generate sys_data 1200 4
	./program copy sys_data lib_data 1200 4 1024 sys

	./program sort sys_data 1200 4 sys >> $(OUTPUT)
	./program sort sys_data 1200 4 lib >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo Ilosc rekordow: 1200, rozmiar: 512 >> $(OUTPUT)
	./program generate sys_data 1200 512
	./program copy sys_data lib_data 1200 512 1024 sys

	./program sort sys_data 1200 512 sys >> $(OUTPUT)
	./program sort sys_data 1200 512 lib >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo Ilosc rekordow: 1200, rozmiar: 4096 >> $(OUTPUT)
	./program generate sys_data 1200 4096
	./program copy sys_data lib_data 1200 4096 1024 sys

	./program sort sys_data 1200 4096 sys >> $(OUTPUT)
	./program sort sys_data 1200 4096 lib >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo Ilosc rekordow: 1200, rozmiar: 8192 >> $(OUTPUT)
	./program generate sys_data 1200 8192
	./program copy sys_data lib_data 1200 8192 1024 sys

	./program sort sys_data 1200 8192 sys >> $(OUTPUT)
	./program sort sys_data 1200 8192 lib >> $(OUTPUT)
	echo >> $(OUTPUT)

	#A teraz kopiowanie test
	echo Czasy dla poszczegolnych rozmiarow buforow: >> $(OUTPUT)
	./program generate sys_data 8192 8192
	echo Plik 8192 rekordow x 8192 dlugosc >> $(OUTPUT)
	echo Bufor: 4 >> $(OUTPUT)
	./program copy sys_data lib_data 8192 8192 4 sys >> $(OUTPUT)
	./program copy sys_data lib_data 8192 8192 4 lib >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo Bufor: 128 >> $(OUTPUT)
	./program copy sys_data lib_data 8192 8192 128 sys >> $(OUTPUT)
	./program copy sys_data lib_data 8192 8192 128 lib >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo Bufor: 512 >> $(OUTPUT)
	./program copy sys_data lib_data 8192 8192 512 sys >> $(OUTPUT)
	./program copy sys_data lib_data 8192 8192 512 lib >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo Bufor: 1024 >> $(OUTPUT)
	./program copy sys_data lib_data 8192 8192 1024 sys >> $(OUTPUT)
	./program copy sys_data lib_data 8192 8192 1024 lib >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo Bufor: 4096 >> $(OUTPUT)
	./program copy sys_data lib_data 8192 8192 4096 sys >> $(OUTPUT)
	./program copy sys_data lib_data 8192 8192 4096 lib >> $(OUTPUT)
	echo >> $(OUTPUT)
	echo Bufor: 8192 >> $(OUTPUT)
	./program copy sys_data lib_data 8192 8192 8192 sys >> $(OUTPUT)
	./program copy sys_data lib_data 8192 8192 8192 lib >> $(OUTPUT)