#include "hash.h"


/*
 * Return an index by converting base4 value of DNA to base10 index to an array
 * A = 0
 * C = 1
 * G = 2
 * T = 3
 */
int hash(char *kmer, int length) {
    if (length != KMER_LENGTH) {
        return -1;
    }
    int hash_value = 0;
    for (int i = 0; i < length; i++) {
        char dna = kmer[i];
        int value = 0;
        if (dna == 'a' || dna == 'A') {
            continue;
        } else if (dna == 'c' || dna == 'C') {
            value = 1;
        } else if (dna == 'g' || dna == 'G') {
            value = 2;
        } else if (dna == 't' || dna == 'T') {
            value = 3;
        } else {
            return -1;
        }
        hash_value += power(4, 4-i) * value;
    }
    return hash_value;
}

/* Get the power by multiplying the base, exp times */
long power(int base, int exp) {
    if (exp < 0) {
        return -1;
    }
    long accumulator = 1;
    for (int i = 0; i < exp; i++) {
        accumulator *= base;
    }
    return accumulator;
}
