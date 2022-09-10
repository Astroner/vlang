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

static Declaration* runFunctionCall(FunctionCallValue* call, RuntimeContext* ctx) {

    if(strcmp(call->name->value, "log") == 0) {
        Native.log(call, ctx);
        DeclarationValue emptyValue;
        return RuntimeUtils.createDeclaration(AST_NODE_TYPE_BLANK, emptyValue);
    }
    if(strcmp(call->name->value, "randomNumber") == 0) {
        DeclarationValue value;
        value.number = Native.randomNumber(call, ctx);
        return RuntimeUtils.createDeclaration(AST_NODE_TYPE_NUMBER, value);
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
    
    Table* declarationsTable = HashTable.create();

    char* argumentsBytes = NULL;

    int counter = 0;
    if(definition->argumentsCount > 0) {

        if(definition->memoized) {
            argumentsBytes = malloc(sizeof(DeclarationValue) * definition->argumentsCount + 1);
            argumentsBytes[sizeof(DeclarationValue) * definition->argumentsCount] = '\0';
        }

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

            if(definition->memoized) {
                for(int i = sizeof(DeclarationValue) * counter; i < sizeof(DeclarationValue) * (counter + 1); i++) {
                    argumentsBytes[i] = ((char*)(&declaration->value))[i] + 1;
                }
                counter++;
            }

            HashTable.set(declarationsTable, argumentDeclaration->name->value, declaration);

            currentExpression = currentExpression->next;
            if(!(currentArgument = currentArgument->next)) {
                break;
            }
        }
    }

    Table* memoizationTable;
    if(argumentsBytes != NULL) {
        memoizationTable = HashTable.get(ctx->memoization, call->name->value);
        if(memoizationTable == NULL) {
            memoizationTable = HashTable.create();
            HashTable.set(ctx->memoization, call->name->value, memoizationTable);
        }
        Declaration* memoizedResult = HashTable.get(memoizationTable, argumentsBytes);
        if(memoizedResult != NULL) {
            RuntimeUtils.freeDeclarationsTable(declarationsTable);
            return RuntimeUtils.deepCopyDeclaration(memoizedResult);
        }
    }

    Scope* functionScope = Scopes.createScope(ctx->scope->global);
    Scopes.addTable(functionScope, declarationsTable);

    RuntimeContext functionContext = {
        functionScope,
        ctx->functions,
        NULL,
        ctx->memoization
    };

    ListNode* current = definition->statements;
    while(1) {
        ASTNode* statement = current->value;
        runNode(statement, &functionContext);

        if(functionContext.returnValue != NULL || !(current = current->next)) {
            break;
        }
    }

    if(functionContext.returnValue == NULL) {
        fprintf(stderr, "[ERROR][RUNTIME][71v21kgh21deb] Function '%s' should return something\n", call->name->value);
        exit(1);
    }

    RuntimeUtils.freeDeclarationsTable(Scopes.shiftTable(functionScope));

    if(functionScope->tables->next) {
        fprintf(stderr, "[ERROR][RUNTIME][125eb3c21bb7] Function scope is not clear. [%s]\n", call->name->value);
        exit(1);
    }

    free(functionScope->tables);
    free(functionScope);

    if(argumentsBytes != NULL) {
        HashTable.set(
            memoizationTable, 
            argumentsBytes, 
            RuntimeUtils.deepCopyDeclaration(functionContext.returnValue)
        );
    }

    return functionContext.returnValue;
}

static Declaration* runUnaryExpression(UnaryExpressionValue* expression, RuntimeContext* ctx) {
    Declaration* declaration = runNode(expression->member, ctx);

    if(expression->type == AST_UNARY_EXPRESSION_TYPE_NEGATIVE) {
        if(declaration->type != AST_NODE_TYPE_NUMBER) {
            fprintf(stderr, "[ERROR][RUNTIME][032952866739] Cannot get negative type = '%s'\n", AST.NodeType[declaration->type]);
            exit(1);
        }
        declaration->value.number = -declaration->value.number;
    } else if(expression->type == AST_UNARY_EXPRESSION_TYPE_NOR) {
        if(declaration->type != AST_NODE_TYPE_BOOLEAN) {
            fprintf(stderr, "[ERROR][RUNTIME][432fe7c9503e] Cannot execute nor for type = '%s'\n", AST.NodeType[declaration->type]);
            exit(1);
        }
        declaration->value.boolean = !declaration->value.boolean;
    }


    return declaration;
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

    switch(first->type) {
        case AST_NODE_TYPE_NUMBER: {
            switch(expression->type) {
                case AST_BINOMIAL_EXPRESSION_TYPE_SUM:
                    result->type = AST_NODE_TYPE_NUMBER;
                    result->value.number = first->value.number + second->value.number;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_DIVISION:
                    result->type = AST_NODE_TYPE_NUMBER;
                    result->value.number = first->value.number / second->value.number;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_MULTIPLICATION:
                    result->type = AST_NODE_TYPE_NUMBER;
                    result->value.number = first->value.number * second->value.number;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_SUBTRACTION:
                    result->type = AST_NODE_TYPE_NUMBER;
                    result->value.number = first->value.number - second->value.number;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_POWER:
                    result->type = AST_NODE_TYPE_NUMBER;
                    result->value.number = pow(first->value.number, second->value.number);
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_EQUAL:
                    result->type = AST_NODE_TYPE_BOOLEAN;
                    result->value.boolean = first->value.number == second->value.number;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_NOT_EQUAL:
                    result->type = AST_NODE_TYPE_BOOLEAN;
                    result->value.boolean = first->value.number != second->value.number;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_GREATER:
                    result->type = AST_NODE_TYPE_BOOLEAN;
                    result->value.boolean = first->value.number > second->value.number;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_GREATER_OR_EQUAL:
                    result->type = AST_NODE_TYPE_BOOLEAN;
                    result->value.boolean = first->value.number >= second->value.number;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_LESS:
                    result->type = AST_NODE_TYPE_BOOLEAN;
                    result->value.boolean = first->value.number < second->value.number;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_LESS_OR_EQUAL:
                    result->type = AST_NODE_TYPE_BOOLEAN;
                    result->value.boolean = first->value.number <= second->value.number;
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
            switch(expression->type) {
                case AST_BINOMIAL_EXPRESSION_TYPE_OR:
                    result->type = AST_NODE_TYPE_BOOLEAN;
                    result->value.boolean = first->value.number || second->value.number;
                    break;
                case AST_BINOMIAL_EXPRESSION_TYPE_AND:
                    result->type = AST_NODE_TYPE_BOOLEAN;
                    result->value.boolean = first->value.number && second->value.number;
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

    free(first);
    free(second);

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

static void runVariableAssignment(VariableAssignmentValue* assignment, RuntimeContext* ctx) {
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

    declaration->value = nextDeclaration->value;

    free(nextDeclaration);
}

static void runIfStatement(List* conditions, RuntimeContext* ctx) {
    List* statements = NULL;
    ListNode* current = conditions;
    while(1) {
        ASTNode* condition = current->value;
        
        if(condition->kind == AST_KIND_IF_CONDITION) {
            IfConditionValue* conditionValue = condition->value;
            Declaration* conditionResult = runNode(conditionValue->condition, ctx);
            if(conditionResult->type != AST_NODE_TYPE_BOOLEAN) {
                fprintf(
                    stderr, 
                    "[ERROR][RUNTIME][733a43c282ad] Result of if-case condition expression must be boolean but got '%s'\n", 
                    AST.NodeType[conditionResult->type]
                );
                exit(1);
            }
            if(conditionResult->value.boolean) {
                statements = conditionValue->statements;
                break;
            }
        }
        if(condition->kind == AST_KIND_ELSE_STATEMENT) {
            statements = condition->value;
            break;
        }

        if(!(current = current->next)) break;
    }
    if(statements == NULL) return;

    Scopes.addTable(ctx->scope, HashTable.create());

    current = statements;
    while(1) {
        ASTNode* statement = current->value;
        runNode(statement, ctx);
        if(
            statement->kind == AST_KIND_RETURN_STATEMENT 
            || !(current = current->next)
        ) break;
    }
    RuntimeUtils.freeDeclarationsTable(Scopes.shiftTable(ctx->scope));
}

RunnersModule Runners = {
    runVariableDeclaration,
    runFunctionCall,
    runUnaryExpression,
    runBinomialExpression,
    runFunctionDefinition,
    runVariableAssignment,
    runIfStatement,
};
