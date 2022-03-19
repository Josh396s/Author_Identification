#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "ht.h"
#include "salts.h"
#include "speck.h"
#include "node.h"

struct HashTable {
    uint64_t salt[2];
    uint32_t size;
    Node **slots;
};

struct HashTableIterator {
    HashTable *table;
    uint32_t slot;
};

//Constructor Function that builds a hashtable
//First Paramter: Number of slots the hashtable can index to
HashTable *ht_create(uint32_t size) {
    HashTable *ht = (HashTable *) malloc(sizeof(HashTable));
    Node **slots = (Node **) calloc(size, sizeof(Node *));
    ht->salt[0] = SALT_HASHTABLE_LO;
    ht->salt[1] = SALT_HASHTABLE_HI;
    ht->size = size;
    ht->slots = slots;
    return ht;
}

//Destructor Function that deletes a hashtable
//First parameter: Pointer to pointer of the hashtable
void ht_delete(HashTable **ht) {
    Node *n = NULL;
    HashTableIterator *hti = hti_create(*ht);
    while ((n = ht_iter(hti)) != NULL) {
        node_delete(&n);
    }
    free((*ht)->slots);
    free(*ht);
    hti_delete(&hti);
    *ht = NULL;
    return;
}

//Accessor function that returns the size of the hashtable
//First parameter: Pointer to hashtable
uint32_t ht_size(HashTable *ht) {
    return ht->size;
}

//Accessor function that returns the node
//First parameter: Pointer to a hashtable
//Second parameter: Pointer to a node
Node *ht_lookup(HashTable *ht, char *word) {
    uint32_t count = 0;
    uint64_t ind = hash(ht->salt, word);
    ind = (ind) % ht->size; //Cite: Hashing Lecture
    while (count < ht->size) {
        if (ht->slots[ind] == NULL) {
            return NULL;
        }
        char *word_ar = ht->slots[ind]->word;
        if (word_ar && strcmp(word_ar, word) == 0) {
            return ht->slots[ind];
        }
        ind = (ind + 1) % ht->size; //Cite: Hashing Lecture
        count++;
    }
    return NULL;
}

//Manipulator fucntion that inserts a
//First parameter: Pointer to a hashtable
//Second parameter: Pointer to a character
Node *ht_insert(HashTable *ht, char *word) {
    uint64_t count = 0;
    uint64_t ind = hash(ht->salt, word);
    ind = (ind) % ht_size(ht); //Cite: Hashing Lecture

    while (count < ht_size(ht)) {
        if (ht->slots[ind] == NULL) {
            ht->slots[ind] = node_create(word);
            ht->slots[ind]->count = 1;
            return ht->slots[ind];
        }
        char *word_ar = ht->slots[ind]->word;
        if (word_ar && strcmp(word_ar, word) == 0) {
            ht->slots[ind]->count += 1;
            return ht->slots[ind];
        }
        ind = (ind + 1) % ht->size;
        count++;
    }
    return NULL;
}

//Acessor function that outputs the hashtable
//First parameter: Pointer to hashtable
void ht_print(HashTable *ht) {
    HashTableIterator *hti = hti_create(ht);
    Node *n = NULL;
    while ((n = ht_iter(hti)) != NULL) {
        printf("%s: %u\n", n->word, n->count);
    }
    hti_delete(&hti);
}

//Accessor function that iterates over a hashtable
//First parameter: Pointer to a hashtable
HashTableIterator *hti_create(HashTable *ht) {
    HashTableIterator *hti = (HashTableIterator *) malloc(sizeof(HashTableIterator));
    hti->table = ht;
    hti->slot = 0;
    return hti;
}

//Destructor Function that deletes a hashtableIterator
//First parameter: Pointer to pointer of the hashtableIterator
void hti_delete(HashTableIterator **hti) {
    free(*hti);
    return;
}

//Accessor function that returns the node
//First parameter: Pointer to a hashtableIterator
Node *ht_iter(HashTableIterator *hti) {
    while (hti->slot < hti->table->size) {
        if (hti->slot > hti->table->size) {
            return NULL;
        }
        if (hti->table->slots[hti->slot] != NULL) {
            hti->slot += 1;
            return hti->table->slots[hti->slot - 1];
        }
        hti->slot += 1;
    }
    return NULL;
}
