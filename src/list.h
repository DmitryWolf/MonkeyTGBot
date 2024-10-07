#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>

typedef struct Node {
    void *data_;
    struct Node *prev_;
    struct Node *next_;
} Node;

typedef struct {
    Node* head_;
    Node* tail_;
    size_t size_;
} List;

// Constructor
void list_init(List *list);

// Destructor
void list_push_back(List *list, void *data);

void *list_pop_front(List *list);

size_t list_size(List *list);

void list_clear(List *list);

#endif // QUEUE_H