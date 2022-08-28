#if !defined(HASH_TABLE_H)
#define HASH_TABLE_H

#include "../main.h"
#include "../linkedList/linkedList.h"

#define HASH_TABLE_SIZE 100
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
    void (*forEach)(Table* table, void (*cb)(char* name, void* value));
    void (*clear)(Table* table);
} HashTableModuleType;

HashTableModuleType HashTable;

#endif // HASH_TABLE_H
