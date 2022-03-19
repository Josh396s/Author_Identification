#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "bf.h"
#include "bv.h"
#include "speck.h"
#include "salts.h"

struct BitVector {
    uint32_t length;
    uint8_t *vector;
};

struct BloomFilter {
    uint64_t primary[2];
    uint64_t secondary[2];
    uint64_t tertiary[2];
    BitVector *filter;
};

//Constructor Function that builds a bloomfilter
//First Paramter: Number of slots the bloomfilter can index to
BloomFilter *bf_create(uint32_t size) {
    BloomFilter *bf = (BloomFilter *) malloc(sizeof(BloomFilter));
    bf->filter = bv_create(size * 8);
    bf->primary[0] = SALT_PRIMARY_LO;
    bf->primary[1] = SALT_PRIMARY_HI;
    bf->secondary[0] = SALT_SECONDARY_LO;
    bf->secondary[1] = SALT_SECONDARY_HI;
    bf->tertiary[0] = SALT_TERTIARY_LO;
    bf->tertiary[1] = SALT_TERTIARY_HI;
    return bf;
}

//Destructor function that deletes the bloomfilter
//First parameter: Pointer to a bloomfilter
void bf_delete(BloomFilter **bf) {
    bv_delete(&(*bf)->filter);
    free(*bf);
    (*bf) = NULL;
}

//Accessor function that returns the size of the bloomfilter
//First parameter: Pointer to bloomfilter
uint32_t bf_size(BloomFilter *bf) {
    return (bf->filter->length * 8);
}

//Manipulator function that inserts a word into a bloomfilter
//First parameter: Pointer to the bloomfilter
//Second parameter: Pointer to a word
void bf_insert(BloomFilter *bf, char *word) {
    uint64_t prim_ind = hash(bf->primary, word);
    uint64_t sec_ind = hash(bf->secondary, word);
    uint64_t ter_ind = hash(bf->tertiary, word);
    prim_ind = (prim_ind + 1) % bf_size(bf);
    bv_set_bit(bf->filter, prim_ind);
    sec_ind = (sec_ind + 1) % bf_size(bf);
    bv_set_bit(bf->filter, sec_ind);
    ter_ind = (ter_ind + 1) % bf_size(bf);
    bv_set_bit(bf->filter, ter_ind);
}

//Accessor function that returns if a word is in the bloom filter or not
//First parameter: Pointer to bloomfilter
//Second parameter: Pointer to a word
bool bf_probe(BloomFilter *bf, char *word) {
    uint64_t prim_ind = (hash(bf->primary, word) + 1) % bf_size(bf);
    uint64_t sec_ind = (hash(bf->secondary, word) + 1) % bf_size(bf);
    uint64_t ter_ind = (hash(bf->tertiary, word) + 1) % bf_size(bf);
    int hash1 = bv_get_bit(bf->filter, prim_ind);
    int hash2 = bv_get_bit(bf->filter, sec_ind);
    int hash3 = bv_get_bit(bf->filter, ter_ind);
    if (hash1 && hash2 && hash3 == 1) {
        return true;
    } else {
        return false;
    }
}

//Accessor function that prints out the bloomfilter
//First parameter: Pointer to a bloomfilter
void bf_print(BloomFilter *bf) {
    uint64_t i = 0;
    uint64_t ind = 0;
    while (i < bf_size(bf) * 8) {
        printf("%c", (bf->filter->vector[i / 8] & (1 << ind) ? '1' : '0'));
        i++;
        ind++;
        if (ind > 7) {
            ind = 0;
        }
    }
}
