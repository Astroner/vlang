#include "runNode.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "runners.h"
#include "../runtime-utils/runtime-utils.h"

// Every returned Declaration must be fully free-able(temporary data for example)
Declaration* runNode(ASTNode* node, RuntimeContext* ctx) {
    switch (node->kind) {
        case AST_KIND_VARIABLE_DECLARATION:
            Runners.runVariableDeclaration(node->value, ctx);
            break;
        case AST_KIND_FUNCTION_CALL:
            return Runners.runFunctionCall(node->value, ctx);
        case AST_KIND_UNARY_EXPRESSION: 
            return Runners.runUnaryExpression(node->value, ctx);
        case AST_KIND_IDENTIFIER: {
            Declaration* declaration = Scopes.getItem(ctx->scope, node->value);
            if(declaration == NULL) return NULL;
            return RuntimeUtils.deepCopyDeclaration(declaration);
        }
        case AST_KIND_BOOLEAN_LITERAL:
        case AST_KIND_NUMBER_LITERAL: {
            Declaration* declaration = malloc(sizeof(Declaration));

            if(node->kind == AST_KIND_NUMBER_LITERAL) {
                declaration->type = AST_NODE_TYPE_NUMBER;
                declaration->value = malloc(sizeof(RUNTIME_DATA_TYPE_BINDING_NUMBER));
            } else {
                declaration->type = AST_NODE_TYPE_BOOLEAN;
                declaration->value = malloc(sizeof(RUNTIME_DATA_TYPE_BINDING_BOOLEAN));
            }

            memcpy(
                declaration->value,
                node->value,
                sizeof(RUNTIME_DATA_TYPE_BINDING_NUMBER)
            );

            return declaration;
        }
        case AST_KIND_BINOMIAL_EXPRESSION:
            return Runners.runBinomialExpression(node->value, ctx);
        case AST_KIND_FUNCTION_DEFINITION:
            Runners.runFunctionDefinition(node->value, ctx);
            break;
        case AST_KIND_VARIABLE_ASSIGNMENT:
            Runners.runVariableAssignment(node->value, ctx);
            break;
        default:
            fprintf(stderr, "[ERROR][RUNTIME][95b789dec5eb] Cannot execute node '%s'\n", AST.NodeKind[node->kind]);
            exit(1);
            break;
    }

    return NULL;
}