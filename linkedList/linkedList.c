#include "linkedList.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../main.h"

static List* createList() {
    List *list = malloc(sizeof(ListNode));
    list->value = NULL;
    list->prev = NULL;
    list->next = NULL;

    return list;
}

static void pushItem(List *list, void* value) {
    ListNode *lastItem = list;

    if(!lastItem->value) {
        lastItem->value = value;
        return;
    }

    while(1) {
        if(!lastItem->next) break;
        lastItem = lastItem->next;
    }

    ListNode *newItem = malloc(sizeof(ListNode));
    newItem->prev = lastItem;
    newItem->value = value;
    newItem->next = NULL;

    lastItem->next = newItem;
}

static int length(List *list) {
    int count = 1;

    ListNode *current = list;

    while(current->next) {
        count++;
        current = current->next;
    };

    return count;
}

static void* itemAt(List *list, int index) {
    ListNode *current = list;
    int i = 0;

    while(1){
        if(i == index) {
            return current->value;
        }
        if(current->next) {
            current = current->next;
            i++;
            continue;
        }
        printf("Out of list length (%d)\n", index);
        exit(1);
    }
}

static List* map(List *list, void*(*mapper)(void* value, int index)) {
    List *newList = createList();

    ListNode *current = list;

    int index = 0;

    while(1){
        pushItem(newList, mapper(current->value, index));

        if(!current->next) break;
        
        current = current->next;
        index++;
    }

    return newList;
}

static void forEach(
    List *list, 
    void (*callback)(void* value, int index)
) {
        ListNode *current = list;

    int index = 0;

    while(1){
        callback(current->value, index);

        if(!current->next) break;
        
        current = current->next;
        index++;
    }
}

static void clear(List* list) {
    BOOL isFirst = list->prev == NULL;
    if(isFirst) {
        if(list->next) clear(list->next);
        list->next = NULL;
    } else {
        ListNode* next = list->next;
        
        free(list);
        if(next) clear(next);
    }
}

static List* leftPush(List* list, void* value) {
    ListNode* node = malloc(sizeof(ListNode));
    node->value = value;
    node->prev = NULL;
    node->next = list;
    list->prev = node;

    return node;
}

LinkedListModuleType LinkedList = {
    pushItem,
    length,
    itemAt,
    map,
    createList,
    forEach,
    clear,
    leftPush
};
