#if !defined(RUNTIME_UTILS_H)
#define RUNTIME_UTILS_H

#include "../runtime.h"

typedef struct {
    Declaration* (*createDeclaration)(AST_NODE_TYPE type, void* value);
    Declaration* (*deepCopyDeclaration)(Declaration* src);
    void (*freeDeclaration)(Declaration* declaration);
    void (*freeDeclarationValue)(Declaration* declaration);
    void (*freeDeclarationsTable)(Table* variables);
} RuntimeUtilsModule;

RuntimeUtilsModule RuntimeUtils;

#endif // RUNTIME_UTILS_H
