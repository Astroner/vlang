#if !defined(AST_H)
#define AST_H

#include "../linkedList/linkedList.h"
#include "../main.h"

typedef int AST_KIND;
#define AST_KIND_BLANK 0

#define AST_KIND_VARIABLE_DECLARATION 1 // VariableDeclarationValue
#define AST_KIND_FUNCTION_CALL 2        // FunctionCallValue
#define AST_KIND_IDENTIFIER 3           // char*
#define AST_KIND_NUMBER_LITERAL 4       // int*
#define AST_KIND_BINOMIAL_EXPRESSION 5  // BinomialExpressionValue
#define AST_KIND_UNARY_EXPRESSION 6     // UnaryExpressionValue
#define AST_KIND_FUNCTION_DEFINITION 7  // FunctionDefinitionValue
#define AST_KIND_FUNCTION_ARGUMENT 8    // FunctionArgumentValue
#define AST_KIND_RETURN_STATEMENT 9     // ASTNode
#define AST_KIND_VARIABLE_ASSIGNMENT 10 // VariableAssignmentValue
#define AST_KIND_BOOLEAN_LITERAL 11     // int*(0, 1)
#define AST_KIND_IF_STATEMENT 12        // List<ASTNode>
#define AST_KIND_IF_CONDITION 13        // IfConditionValue
#define AST_KIND_ELSE_STATEMENT 14      // List<ASTNode>

typedef int AST_NODE_TYPE;
#define AST_NODE_TYPE_BLANK 0
#define AST_NODE_TYPE_NUMBER 1
#define AST_NODE_TYPE_BOOLEAN 2


typedef int AST_BINOMIAL_EXPRESSION_TYPE;
#define AST_BINOMIAL_EXPRESSION_TYPE_BLANK 0

#define AST_BINOMIAL_EXPRESSION_TYPE_OR 1
#define AST_BINOMIAL_EXPRESSION_TYPE_AND 2

#define AST_BINOMIAL_EXPRESSION_TYPE_EQUAL 3
#define AST_BINOMIAL_EXPRESSION_TYPE_GREATER 4
#define AST_BINOMIAL_EXPRESSION_TYPE_GREATER_OR_EQUAL 5
#define AST_BINOMIAL_EXPRESSION_TYPE_LESS 6
#define AST_BINOMIAL_EXPRESSION_TYPE_LESS_OR_EQUAL 7
#define AST_BINOMIAL_EXPRESSION_TYPE_NOT_EQUAL 8

#define AST_BINOMIAL_EXPRESSION_TYPE_SUM 9
#define AST_BINOMIAL_EXPRESSION_TYPE_SUBTRACTION 10
#define AST_BINOMIAL_EXPRESSION_TYPE_MULTIPLICATION 11
#define AST_BINOMIAL_EXPRESSION_TYPE_DIVISION 12
#define AST_BINOMIAL_EXPRESSION_TYPE_POWER 13



typedef int AST_UNARY_EXPRESSION_TYPE;
#define AST_UNARY_EXPRESSION_TYPE_BLANK 0
#define AST_UNARY_EXPRESSION_TYPE_NEGATIVE 1
#define AST_UNARY_EXPRESSION_TYPE_NOR 2

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
    int argumentsCount;
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
    AST_NODE_TYPE type;
    ASTNode* name;
} FunctionArgumentValue;

typedef struct {
    BOOL pure;
    BOOL memoized;
    ASTNode* name;
    AST_NODE_TYPE returnType;
    unsigned int argumentsCount;
    List* arguments;
    List* statements;
} FunctionDefinitionValue;

typedef struct {
    ASTNode* name;
    ASTNode* value;
} VariableAssignmentValue;

typedef struct {
    ASTNode* condition;
    List* statements;
} IfConditionValue;

typedef struct {
    List* (*createASTFromTokens)(List* token);
    char** NodeKind;
    char** NodeType;
    char** BinomialExpressionType;
    char** UnaryExpressionType;
} ASTModuleType;

ASTModuleType AST;

#endif // AST_H
