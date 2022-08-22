#include "../ast.h"

#if !defined(CREATORS_H)
#define CREATORS_H

typedef struct {
    ASTNode* (*createVariableDeclaration)(ASTNode* identifier, AST_NODE_TYPE type, ASTNode* value);
    ASTNode* (*createIdentifier)(char* name);
    ASTNode* (*createNumberLiteral)(int* value);
    ASTNode* (*createBinomialExpression)(AST_BINOMIAL_EXPRESSION_TYPE type, ASTNode* first, ASTNode* second);
    ASTNode* (*createUnaryExpression)(AST_UNARY_EXPRESSION_TYPE type, ASTNode* member);
    ASTNode* (*createFunctionCall)(ASTNode* name, int argumentsLength, List* arguments);
    ASTNode* (*createFunctionDefinition)(ASTNode* name, AST_NODE_TYPE returnType, unsigned int argumentsLength, List* arguments, List* statements);
    ASTNode* (*createFunctionArgument)(ASTNode* name, AST_NODE_TYPE type);
} CreatorsType;

CreatorsType Creators;

#endif // CREATORS_H
