FLAGS = -Wall -std=c99 -O2

all: kmercounter

kmercounter: hash.c parsefile.o FileListGenerator.o kmercounter.c
	$(CC) $(FLAGS) parsefile.o hash.c FileListGenerator.o kmercounter.c -o kmercounter

parsefile.o: parsefile.c
	$(CC) $(FLAGS) hash.c parsefile.c -c

FileListGenerator.o: FileListGenerator.c
	$(CC) $(FLAGS) FileListGenerator.c -c

make clean:
	rm *.o kmercounter
