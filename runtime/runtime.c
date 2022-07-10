#include "runtime.h"

#include <stdio.h>
#include <stdlib.h>

#include "../HashTable/HashTable.h"
#include "../ast/ast.h"

typedef struct {
    AST_NODE_TYPE type;
    void* value;
} Declaration;

static void runVariableDeclaration(VariableDeclarationValue* variable, Table* scope) {
    Declaration* declaration = HashTable.get(scope, variable->name->value);
    if(declaration != NULL) {
        fprintf(stderr, "[ERROR] Variable '%s' already declared\n", variable->name->value);
        exit(1);
    }
    declaration = malloc(sizeof(Declaration));
    declaration->type = variable->type;
    declaration->value = variable->value;

    HashTable.set(scope, variable->name->value, declaration);
}

static void printDeclaration(Declaration* declaration) {
    if(declaration->type == AST_NODE_TYPE_NUMBER) {
        printf("%d", *((int*)declaration->value));
    }
}

static void runFunctionCall(FunctionCallValue* call, Table* globalScope, Table* scope) {
    ListNode* current = call->arguments;
    while(1) {
        ASTNode* node = current->value;

        if(node->kind == AST_KIND_NUMBER_LITERAL) {
            printf("%d", *((int*)node->value));
        }
        if(node->kind == AST_KIND_IDENTIFIER) {
            Declaration* declaration = HashTable.get(scope, node->value);
            if(declaration == NULL) declaration = HashTable.get(globalScope, node->value);
            if(declaration == NULL) {
                fprintf(stderr, "[ERROR] Undefined variable '%s'\n", node->value);
                exit(1);
            }
            printDeclaration(declaration);
        }
        if(current->next) printf(" ");

        if(!(current = current->next)) break;
    }
    printf("\n");
}

static void runNode(ASTNode* node, Table* globalScope, Table* scope) {
    switch (node->kind) {
        case AST_KIND_VARIABLE_DECLARATION:
            runVariableDeclaration(node->value, scope);
            break;
        case AST_KIND_FUNCTION_CALL:
            runFunctionCall(node->value, globalScope, scope);
            break;
        default:
            break;
    }
}

static void run(List* statements) {
    Table* globalScope = HashTable.create();

    ListNode* current = statements;
    while(1) {
        ASTNode* statement = current->value;
        
        runNode(statement, globalScope, globalScope);

        if(!(current = current->next)) break;
    }
}

RuntimeType Runtime = { run };