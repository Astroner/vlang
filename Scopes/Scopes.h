#if !defined(SCOPES_H)
#define SCOPES_H

#include "../linkedList/linkedList.h"
#include "../HashTable/HashTable.h"

typedef struct {
    List* tables;
    Table* global;
} Scope;

typedef struct {
    Scope* (*createScope)(Table* global);
    void (*addTable)(Scope* scope, Table* table);
    void (*addGlobalItem)(Scope* scope, char* key, void* value);
    void (*setItem)(Scope* scope, char* key, void* value);
    void* (*getItem)(Scope* scope, char* key);
    Table* (*shiftTable)(Scope* scope);
} ScopesModule;

ScopesModule Scopes;

#endif // SCOPES_H
