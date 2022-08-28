#if !defined(RUNTIME)
#define RUNTIME

#include <stdlib.h>

#include "../linkedList/linkedList.h"
#include "../Scopes/Scopes.h"
#include "../ast/ast.h"

#define RUNTIME_DATA_TYPE_BINDING_NUMBER int

#define typeSize(type) typeSizes[type]

static size_t typeSizes[] = {
    0,
    sizeof(RUNTIME_DATA_TYPE_BINDING_NUMBER)
};

typedef struct {
    AST_NODE_TYPE type;
    void* value;
} Declaration;

typedef struct {
    Scope* scope;
    Table* functions;
} RuntimeContext;

typedef struct {
    void (*run)(List* statements);
} RuntimeType;

RuntimeType Runtime;

#endif // RUNTIME
