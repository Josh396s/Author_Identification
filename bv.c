#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "bv.h"

struct BitVector {
    uint32_t length;
    uint8_t *vector;
};

//Constructor function that creates the bitvector
//First parameter: Length of the bitvector
BitVector *bv_create(uint32_t length) {
    BitVector *bv = (BitVector *) malloc(sizeof(BitVector));
    bv->length = (length + 7) / 8; //Cieling Division Cite: Geeksforgeeks
    bv->vector = (uint8_t *) calloc(bv->length, sizeof(uint8_t));
    return bv;
}

//Destructor function that deletes the bitvector
//First parameter: Pointer to a bitvector
void bv_delete(BitVector **bv) {
    if (bv) {
        free((*bv)->vector);
        free(*bv);
    }
    *bv = NULL;
    return;
}

//Accessor function that returns the length of the bitvector
//First parameter: Pointer to bitvector
uint32_t bv_length(BitVector *bv) {
    return (bv)->length;
}

//Manipulator function that sets a bit at index i to 1
//First parameter: Pointer to the bitvector
//Second parameter: Index of the bit
bool bv_set_bit(BitVector *bv, uint32_t i) {
    if (i < 0 || i > ((bv)->length) * 8) { //If i is out of range, return false
        return false;
    }
    bv->vector[i / 8] |= (0x00000001 << (i % 8)); //Cite: set.h asgn3
    return true;
}

//Manipulator function that clears a bit at index i
//First parameter: Pointer to the bitvector
//Second parameter: Index of the bit
bool bv_clr_bit(BitVector *bv, uint32_t i) {
    if (i < 0 || i > ((bv)->length) * 8) { //If i is out of range, return false
        return false;
    }
    bv->vector[i / 8] &= ~(0x1 << ((i % 8) & 0xFFFFFFFF)); //Cite: set.h asgn3
    return true;
}

//Acessor function that returns the bit at index i
//First parameter: Pointer to the bitvector
//Second parameter: Index of the bit
bool bv_get_bit(BitVector *bv, uint32_t i) {
    if (i < 0 || i > ((bv)->length) * 8) { //If i is out of range, return false
        return false;
    }
    uint32_t bitval
        = (bv->vector[i / 8] & (0x00000001 << (i % 8)))
          != 0; //Cite: https://stackoverflow.com/questions/44978126/structure-for-an-array-of-bits-in-c
    if (bitval == 1) { //If the bits value is 1, return true
        return true;
    } else {
        return false;
    }
}

void bv_print(BitVector *bv) {
    for (uint32_t i = 0; i < bv->length; i++) {
        printf("%u", bv->vector[i / 8]);
    }
}
