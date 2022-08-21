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

static ASTNode* createFunctionCall(ASTNode* name, int argumentsLength, List* arguments) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->kind = AST_KIND_FUNCTION_CALL;

    FunctionCallValue* declaration = malloc(sizeof(FunctionCallValue));
    declaration->name = name;
    declaration->argumentsLength = argumentsLength;
    declaration->arguments = arguments;

    node->value = declaration;

    return node;
}

CreatorsType Creators = {
    createVariableDeclaration,
    createIdentifier,
    createNumberLiteral,
    createBinomialExpression,
    createUnaryExpression,
    createFunctionCall
};