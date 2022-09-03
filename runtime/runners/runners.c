#include "runners.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../native/native.h"
#include "../runtime-utils/runtime-utils.h"

#include "runNode.h"


static void runVariableDeclaration(VariableDeclarationValue* variable, RuntimeContext* ctx) {
    BOOL isDeclared = Scopes.hasItemInCurrentTable(ctx->scope, variable->name->value);
    if(isDeclared) {
        fprintf(stderr, "[ERROR][RUNTIME][b9c3a7e5f23e] Variable '%s' already declared\n", variable->name->value);
        exit(1);
    }

    Declaration* declaration = runNode(variable->value, ctx);

    if(declaration->type != variable->type) {
        fprintf(stderr, "[ERROR][RUNTIME][77d1d3121deb] cannot assign type '%s' to type '%s'\n", AST.NodeType[declaration->type], AST.NodeType[variable->type]);
        exit(1);
    }

    Scopes.setItem(ctx->scope, variable->name->value, declaration);
}

static void clearDeclaration(char* key, void* value) {
    RuntimeUtils.freeDeclaration(value);
}

static Declaration* runFunctionCall(FunctionCallValue* call, RuntimeContext* ctx) {

    if(strcmp(call->name->value, "log") == 0) {
        Native.log(call, ctx);
        return RuntimeUtils.createDeclaration(AST_NODE_TYPE_BLANK, NULL);
    }
    if(strcmp(call->name->value, "randomNumber") == 0) {
        int* randInt = malloc(sizeof(int));
        *randInt = Native.randomNumber(call, ctx);
        return RuntimeUtils.createDeclaration(AST_NODE_TYPE_NUMBER, randInt);
    }

    FunctionDefinitionValue* definition = HashTable.get(ctx->functions, call->name->value);

    if(definition == NULL) {
        fprintf(stderr, "[ERROR][RUNTIME][71h31d3121deb] Function '%s' is not defined\n", call->name->value);
        exit(1);
    }

    if(call->argumentsCount < definition->argumentsCount) {
        fprintf(stderr, "[ERROR][RUNTIME][71h31kgh21deb] Not INAfu arguments to call function '%s'\n", call->name->value);
        exit(1);
    }

    if(call->argumentsCount > definition->argumentsCount) {
        fprintf(stderr, "[ERROR][RUNTIME][71h31kgh21deb] Too much arguments to call function '%s'\n", call->name->value);
        exit(1);
    }
    
    Scope* functionScope = Scopes.createScope(ctx->scope->global);
    Scopes.addTable(functionScope, HashTable.create());

    RuntimeContext functionContext = {
        functionScope,
        ctx->functions
    };

    if(definition->argumentsCount > 0) {
        ListNode* currentArgument = definition->arguments;
        ListNode* currentExpression = call->arguments;
        while(1) {
            ASTNode* argument = currentArgument->value;
            FunctionArgumentValue* argumentDeclaration = argument->value;
            ASTNode* expression = currentExpression->value;

            Declaration* declaration = runNode(expression, ctx);
            if(declaration->type != argumentDeclaration->type) {
                fprintf(
                    stderr, 
                    "[ERROR][RUNTIME][71h31kgh28geb] Type '%s' does not match type '%s' of argument '%s' for function '%s'\n", 
                    AST.NodeType[declaration->type],
                    AST.NodeType[argumentDeclaration->type],
                    argumentDeclaration->name->value,
                    call->name->value
                );
                exit(1);
            }

            Scopes.setItem(functionScope, argumentDeclaration->name->value, declaration);

            currentExpression = currentExpression->next;
            if(!(currentArgument = currentArgument->next)) {
                break;
            }
        }
    }

    Declaration* returnValue = NULL;
    ListNode* current = definition->statements;
    while(1) {
        ASTNode* statement = current->value;
        if(statement->kind == AST_KIND_RETURN_STATEMENT) {
            ASTNode* returnStatementExpression = statement->value;
            returnValue = runNode(returnStatementExpression, &functionContext);
            break;
        } else {
            runNode(statement, &functionContext);
        }

        if(!(current = current->next)) {
            break;
        }
    }

    if(returnValue == NULL) {
        fprintf(stderr, "[ERROR][RUNTIME][71v21kgh21deb] Function '%s' should return something\n", call->name->value);
        exit(1);
    }

    Table* variablesTable = Scopes.shiftTable(functionScope);

    HashTable.forEach(variablesTable, clearDeclaration);
    HashTable.clear(variablesTable);

    if(functionScope->tables->next) {
        fprintf(stderr, "[ERROR][RUNTIME][125eb3c21bb7] Function scope is not clear. [%s]\n", call->name->value);
        exit(1);
    }

    free(functionScope->tables);
    free(functionScope);

    return returnValue;
}

static Declaration* runUnaryExpression(UnaryExpressionValue* expression, RuntimeContext* ctx) {
    Declaration* declaration = runNode(expression->member, ctx);

    if(declaration->type != AST_NODE_TYPE_NUMBER) {
        fprintf(stderr, "[ERROR][RUNTIME][032952866739] Cannot get negative type = '%s'\n", AST.NodeType[declaration->type]);
        exit(1);
    }

    *((int*)declaration->value) = - *((int*)declaration->value);

    return declaration;
}

static void initiateBinomialOperationResult(Declaration* declaration, AST_NODE_TYPE operandsType, AST_BINOMIAL_EXPRESSION_TYPE operationType){
    switch(operationType) {
        case AST_BINOMIAL_EXPRESSION_TYPE_AND:
        case AST_BINOMIAL_EXPRESSION_TYPE_OR:
        case AST_BINOMIAL_EXPRESSION_TYPE_EQUAL:
        case AST_BINOMIAL_EXPRESSION_TYPE_NOT_EQUAL:
        case AST_BINOMIAL_EXPRESSION_TYPE_GREATER:
        case AST_BINOMIAL_EXPRESSION_TYPE_GREATER_OR_EQUAL:
        case AST_BINOMIAL_EXPRESSION_TYPE_LESS:
        case AST_BINOMIAL_EXPRESSION_TYPE_LESS_OR_EQUAL: {
            declaration->type = AST_NODE_TYPE_BOOLEAN;
            declaration->value = malloc(typeSize(AST_NODE_TYPE_BOOLEAN));
            break;
        }
        default: {
            declaration->type = operandsType;
            declaration->value = malloc(typeSize(operandsType));
            break;
        }
    }
}

static Declaration* runBinomialExpression(BinomialExpressionValue* expression, RuntimeContext* ctx) {
    Declaration* result = malloc(sizeof(Declaration));

    Declaration* first = runNode(expression->first, ctx);
    Declaration* second = runNode(expression->second, ctx);

    if(first->type != second->type) {
        fprintf(
            stderr, 
            "[ERROR][RUNTIME][ff83e3b4de52] Cannot perform binomial operation between type '%s' and type '%s'\n", 
            AST.NodeType[first->type], 
            AST.NodeType[second->type]
        );
        exit(1);
    }

    initiateBinomialOperationResult(result, first->type, expression->type);

    switch(first->type) {
        case AST_NODE_TYPE_NUMBER: {
            int x = *((int*)first->value);
            int y = *((int*)second->value);

            switch(expression->type) {
                case AST_BINOMIAL_EXPRESSION_TYPE_SUM:
                    *((int*)result->value) = x + y;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_DIVISION:
                    *((int*)result->value) = x / y;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_MULTIPLICATION:
                    *((int*)result->value) = x * y;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_SUBTRACTION:
                    *((int*)result->value) = x - y;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_POWER:
                    *((int*)result->value) = pow(x, y);
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_EQUAL:
                    *((int*)result->value) = x == y;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_NOT_EQUAL:
                    *((int*)result->value) = x != y;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_GREATER:
                    *((int*)result->value) = x > y;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_GREATER_OR_EQUAL:
                    *((int*)result->value) = x >= y;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_LESS:
                    *((int*)result->value) = x > y;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_LESS_OR_EQUAL:
                    *((int*)result->value) = x >= y;
                    break;
                default:
                    fprintf(
                        stderr, 
                        "[ERROR][RUNTIME][0101d0b066ba] Cannot perform operation '%s' with numbers\n", 
                        AST.BinomialExpressionType[expression->type]
                    );
                    exit(1);
            }
            break;
        }
        case AST_NODE_TYPE_BOOLEAN: {
            int x = *((int*)first->value);
            int y = *((int*)second->value);
            
            switch(expression->type) {
                case AST_BINOMIAL_EXPRESSION_TYPE_OR:
                    *((int*)result->value) = x || y;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_AND:
                    *((int*)result->value) = x && y;
                    break;
                default:
                    fprintf(
                        stderr, 
                        "[ERROR][RUNTIME][9b567eaa0188] Cannot perform operation '%s' with booleans\n", 
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

    RuntimeUtils.freeDeclaration(first);
    RuntimeUtils.freeDeclaration(second);

    return result;
}

static void runFunctionDefinition(FunctionDefinitionValue* definition, RuntimeContext* ctx) {
    FunctionDefinitionValue* check = HashTable.get(ctx->functions, definition->name->value);

    if(check != NULL) {
        fprintf(stderr, "[ERROR][RUNTIME][0329947266739] Function '%s' is already defined\n", definition->name->value);
        exit(1);
    }

    HashTable.set(ctx->functions, definition->name->value, definition);
}

void runVariableAssignment(VariableAssignmentValue* assignment, RuntimeContext* ctx) {
    Declaration* declaration = Scopes.getItem(ctx->scope, assignment->name->value);

    if(declaration == NULL) {
        fprintf(stderr, "[ERROR][RUNTIME][032994726d7n9] Variable '%s' is not defined\n", assignment->name->value);
        exit(1);
    }

    Declaration* nextDeclaration = runNode(assignment->value, ctx);

    if(nextDeclaration->type != declaration->type) {
        fprintf(
            stderr, 
            "[ERROR][RUNTIME][032909726d7n9] Cannot assign type '%s' to type '%s' for variable '%s'\n",
            AST.NodeType[nextDeclaration->type],
            AST.NodeType[declaration->type],
            assignment->name->value
        );
        exit(1);
    }

    RuntimeUtils.freeDeclarationValue(declaration);

    declaration->value = nextDeclaration->value;

    free(nextDeclaration);
}

RunnersModule Runners = {
    runVariableDeclaration,
    runFunctionCall,
    runUnaryExpression,
    runBinomialExpression,
    runFunctionDefinition,
    runVariableAssignment
};
