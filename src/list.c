#include "list.h"

#include <sys/mman.h>

void list_init(List *list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void list_push_back(List *list, void *data) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;

    if (list->head == NULL) {
        newNode->prev = NULL;
        list->head = newNode;
    } else {
        newNode->prev = list->tail;
        list->tail->next = newNode;
    }
    list->tail = newNode;
    list->size++;
}

void* list_pop_front(List *list) {
    if (list->head == NULL) {
        return NULL;
    }

    Node *temp = list->head;
    void *data = temp->data;
    
    list->head = list->head->next;
    if (list->head != NULL) {
        list->head->prev = NULL;
    } else {
        list->tail = NULL;
    }
    list->size--;
    free(temp);
    return data;
}

size_t list_size(List *list) {
    return list->size;
}


void list_clear(List *list) {
    while (list->head != NULL) {
        Node *temp = list->head;
        list->head = list->head->next;
        free(temp->data);
        free(temp);
    }
    list->tail = NULL;
    list->size = 0;
}