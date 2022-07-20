#include "../lexemes/lexemes.h"
#include "../linkedList/linkedList.h"

#if !defined(AST_H)
#define AST_H

#define AST_KIND unsigned char
#define AST_KIND_BLANK 0

#define AST_KIND_VARIABLE_DECLARATION 1
#define AST_KIND_FUNCTION_CALL 2
#define AST_KIND_IDENTIFIER 3
#define AST_KIND_NUMBER_LITERAL 4
#define AST_KIND_BINOMIAL_EXPRESSION 5
#define AST_KIND_UNARY_EXPRESSION 6

#define AST_NODE_TYPE unsigned char
#define AST_NODE_TYPE_BLANK 0
#define AST_NODE_TYPE_NUMBER 1

#define AST_BINOMIAL_EXPRESSION_TYPE unsigned char
#define AST_BINOMIAL_EXPRESSION_TYPE_BLANK 0
#define AST_BINOMIAL_EXPRESSION_TYPE_SUM 1
#define AST_BINOMIAL_EXPRESSION_TYPE_SUBTRACTION 2
#define AST_BINOMIAL_EXPRESSION_TYPE_MULTIPLICATION 3
#define AST_BINOMIAL_EXPRESSION_TYPE_DIVISION 4

#define AST_UNARY_EXPRESSION_TYPE unsigned char
#define AST_UNARY_EXPRESSION_TYPE_BLANK 0
#define AST_UNARY_EXPRESSION_TYPE_NEGATIVE 1

typedef struct {
    AST_KIND kind;
    void* value;
} ASTNode;

typedef struct {
    ASTNode* name;
    AST_NODE_TYPE type;
    ASTNode* value;
} VariableDeclarationValue;

typedef struct {
    ASTNode* name;
    int argLength;
    List* arguments;
} FunctionCallValue;

typedef struct {
    AST_BINOMIAL_EXPRESSION_TYPE type;
    ASTNode* first;
    ASTNode* second;
} BinomialExpressionValue;

typedef struct {
    AST_UNARY_EXPRESSION_TYPE type;
    ASTNode* member;
} UnaryExpressionValue;

typedef struct {
    List* (*createStatements)(List* lexeme);
    char** NodeKind;
    char** NodeType;
    char** BinomialExpressionType;
    char** UnaryExpressionType;
} ASTModuleType;

ASTModuleType AST;

#endif // AST_H
