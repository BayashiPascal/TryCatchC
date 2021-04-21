main: main.o trycatchc.o Makefile
	gcc -Wall -Wextra -Werror -Wfatal-errors -O3 main.o trycatchc.o -lm -o main

trycatchc.o: trycatchc.c trycatchc.h Makefile
	gcc -Wall -Wextra -Werror -Wfatal-errors -O3 -c trycatchc.c

main.o: main.c trycatchc.h Makefile
	gcc -Wall -Wextra -Werror -Wfatal-errors -O3 -c main.c

install: trycatchc.o
	rm -rf /usr/local/include/TryCatchC
	mkdir /usr/local/include/TryCatchC
	cp trycatchc.h /usr/local/include/TryCatchC/trycatchc.h
	ar -r /usr/local/lib/libtrycatchc.a trycatchc.o

