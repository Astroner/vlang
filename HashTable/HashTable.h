#include "../main.h"
#include "../linkedList/linkedList.h"

#if !defined(HASH_TABLE_H)
#define HASH_TABLE_H

typedef struct {
    char* name;
    void* value;
} TableItem;

typedef struct {
    List** items;
} Table;

typedef struct {
    Table* (*create)();
    void (*set)(Table* table, char* name, void* value);
    void* (*get)(Table* table, char* name);
} HashTableModuleType;

HashTableModuleType HashTable;

#endif // HASH_TABLE_H
