main: main.o trycatch.o Makefile
	gcc main.o trycatch.o -lm -o main

main.o: main.c trycatch.h Makefile
	gcc -c main.c

trycatch.o: trycatch.c trycatch.h Makefile
	gcc -c trycatch.c

