#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <regex.h>
#include <ctype.h>
#include <math.h>

#include "parser.h"
#include "text.h"
#include "bf.h"
#include "ht.h"
#include "metric.h"
#define WORD "([a-zA-Z]+([-'][a-zA-Z]+)+)|([a-zA-Z]+)"

uint32_t noiselimit;

struct Text {
    HashTable *ht;
    BloomFilter *bf;
    uint32_t word_count;
};

//Constructor Function that builds a Text
//First Paramter: Pointer to file
//Second Parameter: Pointer to noise text
Text *text_create(FILE *infile, Text *noise) {
    Text *text = (Text *) malloc(sizeof(Text));
    if (text == NULL) { //Return NULL if text could not be allocated memory
        return NULL;
    }
    text->ht = ht_create(1 << 19);
    if (text->ht == NULL) { //Free text & return NULL if ht could not be allocated memory
        free(&text);
        return NULL;
    }
    text->bf = bf_create(1 << 21);
    if (text->bf == NULL) { //Free ht & text, return NULL if bf could not be allocated memory
        ht_delete(&(text)->ht);
        free(&text);
        return NULL;
    }

    regex_t re;
    if (regcomp(&re, WORD, REG_EXTENDED)) { //Compile the regular expression, return NULL if fails
        bf_delete(&(text)->bf);
        ht_delete(&(text)->ht);
        free(&text);
        fprintf(stderr, "Failed to compile regex.\n");
        return NULL;
    }

    text->word_count = 0;
    char *word;
    while ((word = next_word(infile, &re)) != NULL) { //Read words from file
        text->word_count += 1; //Increment word_count for every word read

        for (uint32_t i = 0; i <= strlen(word); i++) {
            word[i] = tolower(word[i]);
        }
        if (noise != NULL) {
            if ((text_contains(noise, word))) {
                text->word_count--;
            } else { //If the word isnt in noise file, insert to hashtable & bloomfilter
                ht_insert(text->ht, word);
                bf_insert(text->bf, word);
            }
        }
        if (noise == NULL) {
            ht_insert(text->ht, word); //Insert word to bloomfilert & hashtable
            bf_insert(text->bf, word);
            if (text->word_count == noiselimit) {
                break;
            }
        }
    }
    regfree(&re); //Free the compiled regular expression
    return text; //Return created text
    return NULL;
}

//Destructor Function that deletes a hashtable
//First parameter: Pointer to pointer of the Text
void text_delete(Text **text) {
    ht_delete(&(*text)->ht); //Free the hashtable
    bf_delete(&(*text)->bf); //Free the bloomfilter
    free(*text); //Free the Text ADT
    *text = NULL;
    return;
}

//Accessor function that returns the distance between to texts
//First parameter: Pointer to a text
//Second parameter: Pointer to a text
//Third parameter: Metric to be applied
double text_dist(Text *text1, Text *text2, Metric metric) {
    HashTableIterator *hti1 = hti_create(text1->ht); //Create hashtable iterators
    HashTableIterator *hti2 = hti_create(text2->ht);
    Node *man;
    double man_dist = 0.0;
    while ((man = ht_iter(hti1)) != NULL) { //Read words from first text
        double text1_freq = text_frequency(text1, man->word);
        double text2_freq = text_frequency(text2, man->word);
        if (metric == MANHATTAN) { //Calculate the MANHATTAN dist
            man_dist += fabs(text1_freq - text2_freq);
        }
        if (metric == EUCLIDEAN) { //Calculate the EUCLIDIAN dist
            double euc_dist = 0;
            euc_dist = text1_freq - text2_freq;
            euc_dist *= euc_dist; //pow(euc_dist, 2);
            man_dist += euc_dist;
        }
        if (metric == COSINE) { //Calculate the COSINE dist
            double cos_dist = 0;
            cos_dist = text1_freq * text2_freq;
            man_dist += cos_dist;
        }
    }
    while ((man = ht_iter(hti2)) != NULL) { //Read words from second text
        if ((text_contains(text1, man->word) == false)) { //Continue if word is not in first text
            double text1_freq = 0;
            double text2_freq = text_frequency(text2, man->word);
            if (metric == MANHATTAN) { //Calculate the MANHATTAN dist
                man_dist += fabs(text1_freq - text2_freq);
            }
            if (metric == EUCLIDEAN) { //Calculate the EUCLIDIAN dist
                double euc_dist = 0;
                euc_dist = text1_freq - text2_freq;
                euc_dist *= euc_dist;
                man_dist += euc_dist;
            }
            if (metric == COSINE) { //Calculate the COSINE dist
                double cos_dist = 0;
                cos_dist = text1_freq * text2_freq;
                man_dist += cos_dist;
            }
        }
    }
    hti_delete(&hti1); //Free hashtable iterators
    hti_delete(&hti2);
    if (metric == EUCLIDEAN) { //Return EUCLIDIAN value
        return sqrt(man_dist);
    }
    if (metric == COSINE) { //Return COSINE value
        man_dist = 1 - man_dist;
        return man_dist;
    } else {
        return man_dist; //Return MANHATTAN value
    }
}

//Accessor function that returns the frequency of the word in the text
//First parameter: Pointer to a text
//Second parameter: Pointer to a char
double text_frequency(Text *text, char *word) {
    if (text_contains(text, word)) { //Check to see if word is in the text
        Node *word_text = ht_lookup(text->ht, word);
        return ((double) word_text->count / (double) text->word_count); //Normalize the word count
    } else { //Return 0 if the word wasn't found in text
        return 0;
    }
}

//Accessor function that returns whether a word is in the text
//First parameter: Pointer to a text
//Second parameter: Pointer to a char
bool text_contains(Text *text, char *word) {
    if (word == NULL) {
        return false;
    }
    if (bf_probe(text->bf, word) == true) { //If the word is in the bloomfilter
        if (ht_lookup(text->ht, word) != NULL) { //Search the hashtable for word
            return true;
        }
    }
    return false;
}

void text_print(Text *text) {
    printf("Word Count: %u\n", text->word_count);
    ht_print(text->ht);
}
