#include "native.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../runtime-utils/runtime-utils.h"
#include "../runners/runNode.h"

static void printDeclaration(Declaration* declaration) {
    if(declaration->type == AST_NODE_TYPE_NUMBER) {
        printf("%d", declaration->value.number);
    } else if(declaration->type == AST_NODE_TYPE_BOOLEAN) {
        if(declaration->value.boolean) {
            printf("true");
        } else {
            printf("false");
        }
    } else {
        printf("CANNOT_LOG");
    }
}

static void log(FunctionCallValue* call, RuntimeContext* ctx) {
    ListNode* current = call->arguments;
    while(1) {
        ASTNode* node = current->value;

        Declaration* declaration = runNode(node, ctx);
        printDeclaration(declaration);
        free(declaration);

        if(current->next) printf(" ");

        if(!(current = current->next)) break;
    }
    printf("\n");
}

BOOL isRandomInitialized = FALSE;

int randomNumber(FunctionCallValue* call, RuntimeContext* ctx) {
    if(call->argumentsCount != 2) {
        fprintf(stderr, "[ERROR][RUNTIME][fddd95a60755] randomNumber() requires 2 arguments, but got %d\n", call->argumentsCount);
        exit(1);
    }
    Declaration* min = runNode(call->arguments->value, ctx);
    Declaration* max = runNode(call->arguments->next->value, ctx);

    if(min->type != AST_NODE_TYPE_NUMBER || max->type != AST_NODE_TYPE_NUMBER) {
        fprintf(stderr, "[ERROR][RUNTIME][917523199130] min and max for randomNumber() must be of type number %d\n", call->argumentsCount);
        exit(1);
    }

    if(!isRandomInitialized) {
        isRandomInitialized = TRUE;
        srand(time(NULL));
    }


    int value = (
        (
            rand() % (
                max->value.number
                - 
                min->value.number + 1
            )
        ) 
        + 
        min->value.number
    );

    free(min);
    free(max);

    return value;
}

NativeModule Native = {
    log,
    randomNumber,
};
