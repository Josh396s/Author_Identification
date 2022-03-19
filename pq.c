#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "pq.h"

typedef struct Node1 Node1;

void fix_heap(PriorityQueue *q, uint32_t min);

struct PriorityQueue {
    Node1 **array;
    uint32_t size;
    uint32_t elements;
};

struct Node1 {
    char *author;
    double distance;
};

Node1 *node1_create(char *author) {
    Node1 *nd = (Node1 *) malloc(sizeof(Node1));
    nd->author = author;
    nd->distance = 0;
    return nd;
}

void node1_delete(Node1 **n) {
    if (n) {
        free(*n);
    }
    *n = NULL;
    return;
}

void node1_print(Node1 *n) {
    printf("Author: %s\n", n->author);
    printf("Distance: %f\n", n->distance);
    return;
}

//Constructor fucntion that creates the priority queue
//First parameter: Priority queue's maximum capacity
PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *pq = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    if (pq == NULL) {
        return NULL;
    }
    Node1 **array = (Node1 **) calloc(capacity, sizeof(Node1 *));
    if (array == NULL) {
        free(pq);
        return NULL;
    }
    pq->array = array;
    pq->size = capacity;
    pq->elements = 0;
    return pq;
}

//Destructor function that deletes the priority queue
//First parameter: Pointer to a priority queue
void pq_delete(PriorityQueue **q) {
    if (*q) {
        free((*q)->array);
        free(*q);
    }
    *q = NULL;
    return;
}

//Accessor function that returns whether the priority queue is empty
//First parameter: Pointer to a priority queue
bool pq_empty(PriorityQueue *q) {
    if (pq_size(q) == 0) { //If # of elements in queue is 0 it is empty
        return true;
    } else {
        return false;
    }
}

//Acessor function that returns whether the priority queue is full
//First parameter: Pointer to a priority queue
bool pq_full(PriorityQueue *q) {
    if (pq_size(q) == q->size) {
        return true;
    } else {
        return false;
    }
}

//Accessor function that returns the size of the priority queue
//First parameter: Pointer to a priority queue
uint32_t pq_size(PriorityQueue *q) {
    return q->elements;
}

bool enqueue(PriorityQueue *q, char *author, double dist) {
    if (pq_full(q) == true) {
        return false;
    }
    q->elements = q->elements + 1; //Increase # of elements in queue
    Node1 *new = node1_create(author);
    new->distance = dist;
    q->array[pq_size(q) - 1] = new;
    uint32_t i = pq_size(q) - 1;

    while (i > 0
           && q->array[i / 2]->distance
                  > q->array[i]->distance) { //Cite: Intro to Algorithms & Geeksforgeeks.org
        Node1 *temp = q->array[i];
        q->array[i] = q->array[i / 2];
        q->array[i / 2] = temp; //Swap current node and parent node
        i = i / 2;
    }
    return true;
}

//Manipulator function that dequeues a node, passes back the author and distance
//First parameter: Pointer to a priority queue
//Second parameter: Double pointer to an author
//Third parameter: Pointer to a distance
bool dequeue(PriorityQueue *q, char **author, double *dist) {
    if (pq_empty(q) == true) {
        return false;
    }
    *author = q->array[0]->author;
    *dist = q->array[0]->distance;

    node1_delete(&q->array[0]);

    q->array[0] = q->array[pq_size(q) - 1];
    q->elements = q->elements - 1;
    fix_heap(q, 0);
    return true;
}

//Debug fuction for printing the priority queue
void pq_print(PriorityQueue *q) {
    printf("EMPTY: %d\n", pq_empty(q));
    printf("FULL: %d\n", pq_full(q));
    printf("SIZE: %u\n", pq_size(q));
    for (uint32_t i = 0; i < pq_size(q); i++) {
        node1_print(q->array[i]);
        printf("\n");
    }
    return;
}

//Helper function used to fix the heap after a node is dequeued
void fix_heap(PriorityQueue *q, uint32_t i) { //Cite: Intro to Algorithms/geeksforgeeks.org
    uint32_t min;

    if ((2 * i) < pq_size(q) && q->array[(2 * i)]->distance < q->array[i]->distance) {
        min = (2 * i);
    } else {
        min = i;
    }

    if (((2 * i) + 1) < pq_size(q) && q->array[(2 * i) + 1]->distance < q->array[min]->distance) {
        min = ((2 * i) + 1);
    }

    if (min != i) {
        Node1 *temp = q->array[min];
        q->array[min] = q->array[i];
        q->array[i] = temp;
        fix_heap(q, min);
    }
}
