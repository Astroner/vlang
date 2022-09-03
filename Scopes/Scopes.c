#include "Scopes.h"

#include <stdlib.h>
#include <stdio.h>

static Scope* createScope(Table* global) {
    Scope* scope = malloc(sizeof(Scope));
    if(global == NULL) {
        scope->global = HashTable.create();
    } else {
        scope->global = global;
    }

    scope->tables = LinkedList.createList();
    LinkedList.pushItem(scope->tables, scope->global);

    return scope;
}

static void addTable(Scope* scope, Table* table) {
    scope->tables = LinkedList.leftPush(scope->tables, table);
}

static void addGlobalItem(Scope* scope, char* key, void* value) {
    HashTable.set(scope->global, key, value);
}

static void setItem(Scope* scope, char* key, void* value) {
    HashTable.set(scope->tables->value, key, value);
}

static void* getItem(Scope* scope, char* key) {
    ListNode* current = scope->tables;
    while(1) {
        Table* table = current->value;

        void* value = HashTable.get(table, key);

        if(value != NULL) return value;

        if(!(current = current->next)) {
            return NULL;
        }
    }
}

static Table* shiftTable(Scope* scope) {
    if(scope->tables->next == NULL) {
        return NULL;
    }
    
    ListNode* current = scope->tables;

    ListNode* next = scope->tables->next;
    next->prev = NULL;
    scope->tables = next;
    
    Table* value = current->value;

    free(current);

    return value;
}

static BOOL hasItemInCurrentTable(Scope* scope, char* name) {
    if(HashTable.get(scope->tables->value, name) == NULL) return FALSE;
    return TRUE;
}

ScopesModule Scopes = {
    createScope,
    addTable,
    addGlobalItem,
    setItem,
    getItem,
    shiftTable,
    hasItemInCurrentTable
};