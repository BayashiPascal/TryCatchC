main: main.o trycatch.o Makefile
	gcc-7 main.o trycatch.o -lm -o main

main.o: main.c trycatch.h Makefile
	gcc-7 -c main.c

trycatch.o: trycatch.c trycatch.h Makefile
	gcc-7 -c trycatch.c

