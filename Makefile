main: main.o trycatch.o Makefile
	gcc -Wall -Wextra -Werror -Wfatal-errors -O3 main.o trycatch.o -lm -o main

trycatch.o: trycatch.c trycatch.h Makefile
	gcc -Wall -Wextra -Werror -Wfatal-errors -O3 -c trycatch.c

main.o: main.c trycatch.h Makefile
	gcc -Wall -Wextra -Werror -Wfatal-errors -O3 -c main.c

install: trycatch.o
	rm -rf /usr/local/include/TryCatchC
	mkdir /usr/local/include/TryCatchC
	cp trycatch.h /usr/local/include/TryCatchC/trycatchc.h
	ar -r /usr/local/lib/libtrycatchc.a trycatch.o

