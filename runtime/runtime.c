#include "runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../HashTable/HashTable.h"
#include "../ast/ast.h"

#define typeSize(type) typeSizes[type]

static size_t typeSizes[] = {
    0,
    sizeof(RUNTIME_DATA_TYPE_BINDING_NUMBER)
};

typedef struct {
    AST_NODE_TYPE type;
    void* value;
} Declaration;

Declaration* deepCopyDeclaration(Declaration* src) {
    Declaration* copy = malloc(sizeof(Declaration));
    copy->type = src->type;
    copy->value = malloc(typeSize(src->type));
    
    memcpy(copy->value, src->value, typeSize(src->type));

    return copy;
}

void freeDeclaration(Declaration* declaration) {
    free(declaration->value);
    free(declaration);
}

static Declaration* runNode(ASTNode* node, Table* globalScope, Table* scope);

static Declaration* getDeclarationFromScopes(char* name, Table* globalScope, Table* scope) {
    Declaration* declaration = HashTable.get(scope, name);
    if(declaration == NULL) declaration = HashTable.get(globalScope, name);
    if(declaration == NULL) {
        fprintf(stderr, "[ERROR][RUNTIME][f810c915c075] Undefined variable '%s'\n", name);
        exit(1);
    }

    return declaration;
}

static void runVariableDeclaration(VariableDeclarationValue* variable, Table* globalScope, Table* scope) {
    Declaration* declaration = HashTable.get(scope, variable->name->value);
    if(declaration != NULL) {
        fprintf(stderr, "[ERROR][RUNTIME][b9c3a7e5f23e] Variable '%s' already declared\n", variable->name->value);
        exit(1);
    }

    declaration = runNode(variable->value, globalScope, scope);

    if(declaration->type != variable->type) {
        fprintf(stderr, "[ERROR][RUNTIME][77d1d3121deb] cannot assign type '%s' to type '%s'\n", AST.NodeType[declaration->type], AST.NodeType[variable->type]);
        exit(1);
    }

    HashTable.set(scope, variable->name->value, declaration);
}

static void printDeclaration(Declaration* declaration) {
    if(declaration->type == AST_NODE_TYPE_NUMBER) {
        printf("%d", *((int*)declaration->value));
    } else {
        printf("CANNOT_LOG");
    }
}

static void runFunctionCall(FunctionCallValue* call, Table* globalScope, Table* scope) {
    ListNode* current = call->arguments;
    while(1) {
        ASTNode* node = current->value;

        Declaration* declaration = runNode(node, globalScope, scope);
        printDeclaration(declaration);
        freeDeclaration(declaration);

        if(current->next) printf(" ");

        if(!(current = current->next)) break;
    }
    printf("\n");
}

static Declaration* runUnaryExpression(UnaryExpressionValue* expression, Table* globalScope, Table* scope) {
    Declaration* declaration = runNode(expression->member, globalScope, scope);

    if(declaration->type != AST_NODE_TYPE_NUMBER) {
        fprintf(stderr, "[ERROR][RUNTIME][032952866739] Cannot get negative type = '%s'\n", AST.NodeType[declaration->type]);
        exit(1);
    }

    *((RUNTIME_DATA_TYPE_BINDING_NUMBER*)declaration->value) = - *((RUNTIME_DATA_TYPE_BINDING_NUMBER*)declaration->value);

    return declaration;
}

static Declaration* runBinaryExpression(BinomialExpressionValue* expression, Table* globalScope, Table* scope) {
    Declaration* result = malloc(sizeof(Declaration));

    Declaration* first = runNode(expression->first, globalScope, scope);
    Declaration* second = runNode(expression->second, globalScope, scope);

    if(first->type != second->type) {
        fprintf(
            stderr, 
            "[ERROR][RUNTIME][ff83e3b4de52] Cannot perform operation between type '%s' and type '%s'\n", 
            AST.NodeType[first->type], 
            AST.NodeType[second->type]
        );
    }

    result->type = first->type;
    result->value = malloc(typeSize(first->type));

    switch(result->type) {
        case AST_NODE_TYPE_NUMBER: {
            RUNTIME_DATA_TYPE_BINDING_NUMBER x = *((RUNTIME_DATA_TYPE_BINDING_NUMBER*)first->value);
            RUNTIME_DATA_TYPE_BINDING_NUMBER y = *((RUNTIME_DATA_TYPE_BINDING_NUMBER*)second->value);
            RUNTIME_DATA_TYPE_BINDING_NUMBER* z = result->value;

            switch(expression->type) {
                case AST_BINOMIAL_EXPRESSION_TYPE_SUM:
                    *z = x + y;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_DIVISION:
                    *z = x / y;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_MULTIPLICATION:
                    *z = x * y;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_SUBTRACTION:
                    *z = x - y;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_POWER:
                    *z = pow(x, y);
                    break;
                default:
                    fprintf(
                        stderr, 
                        "[ERROR][RUNTIME][0101d0b066ba] Unknown Operation '%s'\n", 
                        AST.BinomialExpressionType[expression->type]
                    );
                    exit(1);
            }
            break;
        }
        default:
            fprintf(
                stderr, 
                "[ERROR][RUNTIME][86ac208a7524] Unknown operand type '%s'\n", 
                AST.BinomialExpressionType[result->type]
            );
            exit(1);
    }

    freeDeclaration(first);
    freeDeclaration(second);

    return result;
}
 
// Every returned Declaration must be fully free-able(temporary data for example)
static Declaration* runNode(ASTNode* node, Table* globalScope, Table* scope) {
    switch (node->kind) {
        case AST_KIND_VARIABLE_DECLARATION:
            runVariableDeclaration(node->value, globalScope, scope);
            break;
        case AST_KIND_FUNCTION_CALL:
            runFunctionCall(node->value, globalScope, scope);
            break;
        case AST_KIND_UNARY_EXPRESSION: 
            return runUnaryExpression(node->value, globalScope, scope);
        case AST_KIND_IDENTIFIER: 
            return deepCopyDeclaration(getDeclarationFromScopes(node->value, globalScope, scope));
        case AST_KIND_NUMBER_LITERAL: {
            Declaration* declaration = malloc(sizeof(Declaration));
            declaration->type = AST_NODE_TYPE_NUMBER;
            declaration->value = malloc(sizeof(RUNTIME_DATA_TYPE_BINDING_NUMBER));
            memcpy(
                declaration->value,
                node->value,
                sizeof(RUNTIME_DATA_TYPE_BINDING_NUMBER)
            );

            return declaration;
        }
        case AST_KIND_BINOMIAL_EXPRESSION:
            return runBinaryExpression(node->value, globalScope, scope);
        default:
            break;
    }

    return NULL;
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