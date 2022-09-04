#include "loggers.h"

#include <stdio.h>

#include "../token/token.h"
#include "../ast/ast.h"

void logPadding(int padding) {
    for(int i = 0; i < padding; i++) {
        printf("    ");
    }
}

void logASTNode(ASTNode* node, int padding) {
    logPadding(padding);
    printf("%s\n", AST.NodeKind[node->kind]);

    if(node->kind == AST_KIND_VARIABLE_DECLARATION) {
        VariableDeclarationValue* declaration = node->value;

        logPadding(padding + 1);
        printf("Identifier:\n");
        logASTNode(declaration->name, padding + 2);

        logPadding(padding + 1);
        printf("Type: %s\n", AST.NodeType[declaration->type]);

        logPadding(padding + 1);
        printf("Value:\n");
        logASTNode(declaration->value, padding + 2);
    }
    if(node->kind == AST_KIND_FUNCTION_CALL) {
        FunctionCallValue* declaration = node->value;

        logPadding(padding + 1);
        printf("Function: '%s'\n", declaration->name->value);

        logPadding(padding + 1);
        printf("Arguments Count: %d\n", declaration->argumentsCount);

        if(declaration->argumentsCount == 0) return;

        logPadding(padding + 1);
        printf("Arguments:\n");
        ListNode* current = declaration->arguments;
        while(1) {
            ASTNode* node = current->value;

            logASTNode(node, padding + 2);
            
            if(!(current = current->next)) break;
        }
    }
    if(node->kind == AST_KIND_IDENTIFIER) {
        logPadding(padding + 1);
        printf("Name: '%s'\n", node->value);
    }
    if(node->kind == AST_KIND_NUMBER_LITERAL || node->kind == AST_KIND_BOOLEAN_LITERAL) {
        logPadding(padding + 1);
        printf("Value: %d\n", *((int*)(node->value)));
    }
    if(node->kind == AST_KIND_BINOMIAL_EXPRESSION) {
        BinomialExpressionValue* expression = node->value;
        logPadding(padding + 1);
        printf("Type: %s\n", AST.BinomialExpressionType[expression->type]);
        logPadding(padding + 1);
        printf("First:\n");
        logASTNode(expression->first, padding + 2);
        logPadding(padding + 1);
        printf("Second:\n");
        logASTNode(expression->second, padding + 2);
    }
    if(node->kind == AST_KIND_UNARY_EXPRESSION) {
        UnaryExpressionValue* expression = node->value;
        logPadding(padding + 1);
        printf("Type: %s\n", AST.UnaryExpressionType[expression->type]);
        logPadding(padding + 1);
        printf("Member:\n");
        logASTNode(expression->member, padding + 2);
    }
    if(node->kind == AST_KIND_FUNCTION_DEFINITION) {
        FunctionDefinitionValue* declaration = node->value;

        logPadding(padding + 1);
        printf("Return Type: %s\n", AST.NodeType[declaration->returnType]);

        logPadding(padding + 1);
        printf("Arguments Count: %d\n", declaration->argumentsCount);

        ListNode* current = declaration->arguments;
        ASTNode* node;
        if(declaration->argumentsCount > 0) {
            logPadding(padding + 1);
            printf("Arguments:\n");
            while(1) {
                node = current->value;
                logASTNode(node, padding + 2);
                if(!(current = current->next)) break;
            }
        }

        logPadding(padding + 1);
        printf("Statements:\n");
        current = declaration->statements;
        while(1) {
            node = current->value;
            logASTNode(node, padding + 2);
            if(!(current = current->next)) break;
        }
    }
    if(node->kind == AST_KIND_FUNCTION_ARGUMENT) {
        FunctionArgumentValue* declaration = node->value;
        logPadding(padding + 1);
        printf("Type: %s\n", AST.NodeType[declaration->type]);
        logPadding(padding + 1);
        printf("Name:\n");
        logASTNode(declaration->name, padding + 2);
    }
    if(node->kind == AST_KIND_RETURN_STATEMENT) {
        ASTNode* expression = node->value;
        logPadding(padding + 1);
        printf("Expression:\n");
        logASTNode(expression, padding + 2);
    }
    if(node->kind == AST_KIND_VARIABLE_ASSIGNMENT) {
        VariableAssignmentValue* assignment = node->value;
        logPadding(padding + 1);
        printf("Name:\n");
        logASTNode(assignment->name, padding + 2);
        logPadding(padding + 1);
        printf("Value:\n");
        logASTNode(assignment->value, padding + 2);
    }
    if(node->kind == AST_KIND_IF_STATEMENT) {
        List* conditions = node->value;
        
        ListNode* current = conditions;
        while(1) {
            logASTNode(current->value, padding + 1);
            if(!(current = current->next)) return;
        }
    }
    if(node->kind == AST_KIND_IF_CONDITION) {
        IfConditionValue* condition = node->value;
        logPadding(padding + 1);
        printf("Condition: \n");
        logASTNode(condition->condition, padding + 2);

        logPadding(padding + 1);
        printf("Statements: \n");
        ListNode* current = condition->statements;
        while(1) {
            logASTNode(current->value, padding + 2);
            if(!(current = current->next)) return;
        }
    }
    if(node->kind == AST_KIND_ELSE_STATEMENT) {
        ListNode* current = node->value;
        while(1) {
            logASTNode(current->value, padding + 1);
            if(!(current = current->next)) return;
        }
    }
}

void ASTListLogger(void* item, int index) {
    ASTNode* node = item;
    logASTNode(node, 0);
}

void logAST(List* ast) {
    LinkedList.forEach(ast, ASTListLogger);
}

void logToken(void* item, int index) {
    Token* token = item;
    printf("%s", TokenModule.TokenType[token->type]);
    if(token->value) {
        if(token->type == TOKEN_NUMBER_LITERAL || token->type == TOKEN_BOOLEAN_LITERAL) {
            int *intNum = token->value;
            printf("; value = %d", *intNum);
        } else {
            printf("; value = '%s'", token->value);
        }
    }
    printf("\n");
}

void logTokens(List* tokens) {
    LinkedList.forEach(tokens, logToken);
}

LoggersModule Loggers = {
    logAST,
    logTokens,
};
