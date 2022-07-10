#include "HashTable.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define HASH_TABLE_SIZE 100

static unsigned long hash(char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

static Table* create() {
    Table* table = malloc(sizeof(Table));

    List** items = malloc(sizeof(List*) * HASH_TABLE_SIZE);
    table->items = items;

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        items[i] = (List*)NULL;
    }
    return table;
}

static void set(Table* table, char* name, void* value) {
    unsigned long nameHash = hash(name);
    
    int index = nameHash % HASH_TABLE_SIZE;

    List* list = table->items[index];

    if(list == NULL) {
        list = LinkedList.createList();
        table->items[index] = list;
    }

    ListNode* current = list;
    TableItem* item;
    while(1){
        if(!current->value) {
            item = malloc(sizeof(TableItem));
            item->name = name;
            item->value = value;

            current->value = item;
            break;
        }
        item = current->value;
        if(strcmp(item->name, name) == 0) {
            item->value = value;
            break;
        }
        if(!(current = current->next)) {
            item = malloc(sizeof(TableItem));
            item->name = name;
            item->value = value;

            LinkedList.pushItem(list, item);
            break;
        }
    }
}

static void* get(Table* table, char* name) {
    unsigned long nameHash = hash(name);

    int index = nameHash % HASH_TABLE_SIZE;

    List* list = table->items[index];

    if(list == NULL) return NULL;

    ListNode* current = list;
    while(1) {
        TableItem* item = current->value;

        if(strcmp(item->name, name) == 0) return item->value;

        if(!(current = current->next)) break;
    }

    return NULL;
}


HashTableModuleType HashTable = { create, set, get };