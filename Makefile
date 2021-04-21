main: main.o trycatchc_test.o Makefile
	gcc -Wall -Wextra -Werror -Wfatal-errors -O3 main.o trycatchc_test.o -lm -o main

trycatchc_test.o: trycatchc.c trycatchc.h Makefile
	gcc -Wall -Wextra -Werror -Wfatal-errors -O3 -DTryCatchMaxExcLvl=3 -c trycatchc.c; mv trycatchc.o trycatchc_test.o

trycatchc.o: trycatchc.c trycatchc.h Makefile
	gcc -Wall -Wextra -Werror -Wfatal-errors -O3 -c trycatchc.c

main.o: main.c trycatchc.h Makefile
	gcc -Wall -Wextra -Werror -Wfatal-errors -O3 -c main.c

install: trycatchc.o
	rm -rf /usr/local/include/TryCatchC
	mkdir /usr/local/include/TryCatchC
	cp trycatchc.h /usr/local/include/TryCatchC/trycatchc.h
	ar -r /usr/local/lib/libtrycatchc.a trycatchc.o

