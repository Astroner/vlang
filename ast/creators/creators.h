#if !defined(CREATORS_H)
#define CREATORS_H

#include "../ast.h"
#include "../../main.h"

typedef struct {
    ASTNode* (*createVariableDeclaration)(ASTNode* identifier, AST_NODE_TYPE type, ASTNode* value);
    ASTNode* (*createIdentifier)(char* name);
    ASTNode* (*createNumberLiteral)(int* value);
    ASTNode* (*createBinomialExpression)(AST_BINOMIAL_EXPRESSION_TYPE type, ASTNode* first, ASTNode* second);
    ASTNode* (*createUnaryExpression)(AST_UNARY_EXPRESSION_TYPE type, ASTNode* member);
    ASTNode* (*createFunctionCall)(ASTNode* name, int argumentsCount, List* arguments);
    ASTNode* (*createFunctionDefinition)(
        BOOL pure,
        BOOL memoized,
        ASTNode* name, 
        AST_NODE_TYPE returnType, 
        unsigned int argumentsCount, 
        List* arguments, 
        List* statements
    );
    ASTNode* (*createFunctionArgument)(ASTNode* name, AST_NODE_TYPE type);
    ASTNode* (*createReturnStatement)(ASTNode* value);
    ASTNode* (*createVariableAssignment)(ASTNode* name, ASTNode* value);
    ASTNode* (*createBooleanLiteral)(int* value);
    ASTNode* (*createIfStatement)(List* conditions);
    ASTNode* (*createIfCondition)(ASTNode* condition, List* statements);
    ASTNode* (*createElseStatement)(List* statements);
} CreatorsType;

CreatorsType Creators;

#endif // CREATORS_H
