CC = gcc
FLAGS = -Wall -std=c11 -O2

all: freq5

freq5: hash.c parsefile.o FileListGenerator.o freq5.c
	$(CC) $(FLAGS) parsefile.o hash.c FileListGenerator.o freq5.c -o freq5

parsefile.o: parsefile.c
	$(CC) $(FLAGS) hash.c parsefile.c -c

FileListGenerator.o: FileListGenerator.c
	$(CC) $(FLAGS) FileListGenerator.c -c

make clean:
	rm *.o freq5
