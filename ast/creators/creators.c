#include "creators.h"

#include <stdlib.h>

static ASTNode* createIdentifier(char* name) {
    ASTNode* identifier = malloc(sizeof(ASTNode));

    identifier->kind = AST_KIND_IDENTIFIER;
    identifier->value = name;

    return identifier;
}

static ASTNode* createNumberLiteral(int* value) {
    ASTNode* identifier = malloc(sizeof(ASTNode));

    identifier->kind = AST_KIND_NUMBER_LITERAL;
    identifier->value = value;

    return identifier;
}

static ASTNode* createBinomialExpression(
    AST_BINOMIAL_EXPRESSION_TYPE type,
    ASTNode* first,
    ASTNode* second
) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->kind = AST_KIND_BINOMIAL_EXPRESSION;

    BinomialExpressionValue* value = malloc(sizeof(BinomialExpressionValue));
    value->type = type;
    value->first = first;
    value->second = second;

    node->value = value;

    return node;
}

static ASTNode* createUnaryExpression(
    AST_UNARY_EXPRESSION_TYPE type,
    ASTNode* member
) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->kind = AST_KIND_UNARY_EXPRESSION;

    UnaryExpressionValue* value = malloc(sizeof(UnaryExpressionValue));
    value->type = type;
    value->member = member;

    node->value = value;

    return node;
}

static ASTNode* createVariableDeclaration(ASTNode* identifier, AST_NODE_TYPE type, ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->kind = AST_KIND_VARIABLE_DECLARATION;
    
    VariableDeclarationValue* declaration = malloc(sizeof(VariableDeclarationValue));
    
    declaration->name = identifier;
    declaration->type = type;
    declaration->value = value;

    node->value = declaration;

    return node;
}

static ASTNode* createFunctionCall(ASTNode* name, int argumentsCount, List* arguments) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->kind = AST_KIND_FUNCTION_CALL;

    FunctionCallValue* declaration = malloc(sizeof(FunctionCallValue));
    declaration->name = name;
    declaration->argumentsCount = argumentsCount;
    declaration->arguments = arguments;

    node->value = declaration;

    return node;
}

static ASTNode* createFunctionDefinition(ASTNode* name, AST_NODE_TYPE returnType, unsigned int argumentsCount, List* arguments, List* statements) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->kind = AST_KIND_FUNCTION_DEFINITION;

    FunctionDefinitionValue* declaration = malloc(sizeof(FunctionDefinitionValue));
    declaration->name = name;
    declaration->returnType = returnType;
    declaration->statements = statements;
    declaration->argumentsCount = argumentsCount;
    declaration->arguments = arguments;

    node->value = declaration;

    return node;
}

static ASTNode* createFunctionArgument(ASTNode* name, AST_NODE_TYPE type) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->kind = AST_KIND_FUNCTION_ARGUMENT;

    FunctionArgumentValue* declaration = malloc(sizeof(FunctionArgumentValue));

    declaration->name = name;
    declaration->type = type;

    node->value = declaration;

    return node;
}

static ASTNode* createReturnStatement(ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->kind = AST_KIND_RETURN_STATEMENT;
    node->value = value;

    return node;
}

ASTNode* createVariableAssignment(ASTNode* name, ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->kind = AST_KIND_VARIABLE_ASSIGNMENT;

    VariableAssignmentValue* assignment = malloc(sizeof(VariableAssignmentValue));
    assignment->name = name;
    assignment->value = value;

    node->value = assignment;

    return node;
}

CreatorsType Creators = {
    createVariableDeclaration,
    createIdentifier,
    createNumberLiteral,
    createBinomialExpression,
    createUnaryExpression,
    createFunctionCall,
    createFunctionDefinition,
    createFunctionArgument,
    createReturnStatement,
    createVariableAssignment
};
