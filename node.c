#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"

//Constructor function that creates the node
////First Parameter: The word
Node *node_create(char *word) {
    Node *nd = (Node *) malloc(sizeof(Node));
    nd->word = strdup(word);
    nd->count = 0;
    return nd;
}

//Destructor function that deletes the node
////First parameter: Pointer to pointer of the node
void node_delete(Node **n) {
    free((*n)->word);
    free(*n);
    *n = NULL;
    return;
}

//Debugging function
////First parameter: Node
void node_print(Node *n) {
    printf("Word:%s\n", n->word);
    printf("Count: %u\n", n->count);
    return;
}
