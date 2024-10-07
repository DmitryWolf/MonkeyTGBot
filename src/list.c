#include "list.h"

#include <sys/mman.h>

void list_init(List *list) {
    list->head_ = NULL;
    list->tail_ = NULL;
    list->size_ = 0;
}

void list_push_back(List *list, void *data) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data_ = data;
    newNode->next_ = NULL;

    if (list->head_ == NULL) {
        newNode->prev_ = NULL;
        list->head_ = newNode;
    } else {
        newNode->prev_ = list->tail_;
        list->tail_->next_ = newNode;
    }
    list->tail_ = newNode;
    list->size_++;
}

void* list_pop_front(List *list) {
    if (list->head_ == NULL) {
        return NULL;
    }

    Node *temp = list->head_;
    void *data = temp->data_;
    
    list->head_ = list->head_->next_;
    if (list->head_ != NULL) {
        list->head_->prev_ = NULL;
    } else {
        list->tail_ = NULL;
    }
    list->size_--;
    free(temp);
    return data;
}

size_t list_size(List *list) {
    return list->size_;
}


void list_clear(List *list) {
    while (list->head_ != NULL) {
        Node *temp = list->head_;
        list->head_ = list->head_->next_;
        free(temp->data_);
        free(temp);
    }
    list->tail_ = NULL;
    list->size_ = 0;
}