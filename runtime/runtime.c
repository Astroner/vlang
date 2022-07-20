#include "runtime.h"

#include <stdio.h>
#include <stdlib.h>

#include "../HashTable/HashTable.h"
#include "../ast/ast.h"

typedef struct {
    AST_NODE_TYPE type;
    void* value;
} Declaration;

static Declaration* getDeclarationFromScopes(char* name, Table* globalScope, Table* scope) {
    Declaration* declaration = HashTable.get(scope, name);
    if(declaration == NULL) declaration = HashTable.get(globalScope, name);
    if(declaration == NULL) {
        fprintf(stderr, "[ERROR] Undefined variable '%s'\n", name);
        exit(1);
    }

    return declaration;
}

static void runVariableDeclaration(VariableDeclarationValue* variable, Table* globalScope, Table* scope) {
    Declaration* declaration = HashTable.get(scope, variable->name->value);
    if(declaration != NULL) {
        fprintf(stderr, "[ERROR] Variable '%s' already declared\n", variable->name->value);
        exit(1);
    }
    declaration = malloc(sizeof(Declaration));
    declaration->type = variable->type;

    Declaration* pointedValue;
    switch(variable->value->kind) {
        case AST_KIND_NUMBER_LITERAL: 
            declaration->value = variable->value->value;
            break;
        case AST_KIND_IDENTIFIER: 
            pointedValue = getDeclarationFromScopes(variable->value->value, globalScope, scope);
            if(pointedValue->type == variable->type) {
                declaration->value = pointedValue->value;
            } else {
                fprintf(stderr, "[ERROR] Cannot assign type '%s' to type '%s'\n", AST.NodeType[pointedValue->type], AST.NodeType[variable->type]);
                exit(1);
            }
            break;
        default:
            fprintf(stderr, "[ERROR] Unexpected AST type '%s'\n", AST.NodeKind[variable->value->kind]);
            exit(1);
            break;
    }

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
            Declaration* declaration = getDeclarationFromScopes(node->value, globalScope, scope);
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
            runVariableDeclaration(node->value, globalScope, scope);
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