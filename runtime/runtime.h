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

typedef struct {
    AST_NODE_TYPE type;
    void* value;
} Declaration;

typedef struct {
    Scope* scope;
    Table* functions;
    Declaration* returnValue;
} RuntimeContext;

typedef struct {
    void (*run)(List* statements);
} RuntimeType;

RuntimeType Runtime;

#endif // RUNTIME
