#if !defined(RUNTIME)
#define RUNTIME

#include <stdlib.h>

#include "../linkedList/linkedList.h"
#include "../Scopes/Scopes.h"
#include "../ast/ast.h"

#define typeSize(type) typeSizes[type]

static size_t typeSizes[] = {
    0,
    sizeof(int),
    sizeof(int)
};

typedef union {
    int boolean;
    int number;
} DeclarationValue;

typedef struct {
    AST_NODE_TYPE type;
    DeclarationValue value;
} Declaration;

typedef struct {
    Scope* scope;
    Table* functions;
    Declaration* returnValue;
    Table* memoization; // Table<Table>
} RuntimeContext;

typedef struct {
    void (*run)(List* statements);
} RuntimeType;

RuntimeType Runtime;

#endif // RUNTIME
