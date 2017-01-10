[![Build Status](https://travis-ci.org/Adriel-M/KmerCounter.svg?branch=master)](https://travis-ci.org/Adriel-M/KmerCounter)

# KmerCounter
Scan whole sequences (DNA bases for now) and count the number of reads per k-mer.

# Compiling
The associated make file will use your default C compiler via the set CC env.

`$ make all`

`$ make CC=clang all`

# Usage

`$ ./kmercounter -d <directory path> [-n] <number of processes to use> [-o] <output file>`

-d: Required to specify the folder path containing sequence reads.

-n: Optional to specify the number of parallel processes to use. If the number of processes is greater than the 
number of files, will set the number of processes to use to the number of files. Defaults to one if unused.

-o: Optional to specify the output file. Defaults to stdout if unused.

# TODO/Future Features
* Able to specify kmer length (current kmer length is 5)

* Able to use read amino acid kmers

* Create a multi thread branch


# Afterwords
This project was started with @DanielChanJA before it was uploaded to github.
