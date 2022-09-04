#include "runtime-utils.h"

#include <stdlib.h>
#include <string.h>

static Declaration* createDeclaration(AST_NODE_TYPE type, void* value) {
    Declaration* declaration = malloc(sizeof(Declaration));
    declaration->type = type;
    declaration->value = value;

    return declaration;
}

static Declaration* deepCopyDeclaration(Declaration* src) {
    Declaration* copy = malloc(sizeof(Declaration));
    copy->type = src->type;
    copy->value = malloc(typeSize(src->type));
    
    memcpy(copy->value, src->value, typeSize(src->type));

    return copy;
}

static void freeDeclaration(Declaration* declaration) {
    if(declaration->type == AST_NODE_TYPE_NUMBER) {
        free(((int*)declaration->value));
    }
    free(declaration);
}

static void freeDeclarationValue(Declaration* declaration) {
    if(declaration->type == AST_NODE_TYPE_NUMBER) {
        free(((int*)declaration->value));
    }
}
static void clearDeclaration(char* key, void* value) {
    RuntimeUtils.freeDeclaration(value);
}

void freeDeclarationsTable(Table* variables) {
    HashTable.forEach(variables, clearDeclaration);
    HashTable.clear(variables);
}

RuntimeUtilsModule RuntimeUtils = {
    createDeclaration,
    deepCopyDeclaration,
    freeDeclaration,
    freeDeclarationValue,
    freeDeclarationsTable,
};

