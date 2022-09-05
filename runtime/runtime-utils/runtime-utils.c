#include "runtime-utils.h"

#include <stdlib.h>
#include <string.h>

Declaration* createDeclaration(AST_NODE_TYPE type, DeclarationValue value) {
    Declaration* declaration = malloc(sizeof(Declaration));

    declaration->type = type;
    declaration->value = value;

    return declaration;
}

static Declaration* deepCopyDeclaration(Declaration* src) {
    Declaration* copy = malloc(sizeof(Declaration));
    copy->type = src->type;
    copy->value = src->value;

    return copy;
}

static void clearDeclaration(char* key, void* value) {
    free(value);
}

void freeDeclarationsTable(Table* variables) {
    HashTable.forEach(variables, clearDeclaration);
    HashTable.clear(variables);
}

RuntimeUtilsModule RuntimeUtils = {
    createDeclaration,
    deepCopyDeclaration,
    freeDeclarationsTable,
};

