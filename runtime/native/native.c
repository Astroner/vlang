#include "native.h"

#include <stdio.h>
#include <stdlib.h>

#include "../runtime-utils/runtime-utils.h"
#include "../runners/runNode.h"

static void printDeclaration(Declaration* declaration) {
    if(declaration->type == AST_NODE_TYPE_NUMBER) {
        printf("%d", *((int*)declaration->value));
    } else {
        printf("CANNOT_LOG");
    }
}

static void log(FunctionCallValue* call, RuntimeContext* ctx) {
    ListNode* current = call->arguments;
    while(1) {
        ASTNode* node = current->value;

        Declaration* declaration = runNode(node, ctx);
        if(declaration == NULL && node->kind == AST_KIND_IDENTIFIER) {
            fprintf(stderr, "[ERROR][RUNTIME][77d1d3163deb] Variable '%s' is not defined\n", node->value);
            exit(1);
        }
        printDeclaration(declaration);
        RuntimeUtils.freeDeclaration(declaration);

        if(current->next) printf(" ");

        if(!(current = current->next)) break;
    }
    printf("\n");
}

NativeModule Native = {
    log,
};
