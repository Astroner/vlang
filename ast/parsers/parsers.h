#if !defined(PARSERS_H)
#define PARSERS_H

#include "../../linkedList/linkedList.h"
#include "../ast.h"
#include "../../main.h"

typedef struct {
    ASTNode* node;
    ListNode* lastNode;
    unsigned int length;
} ParserResult;

typedef struct {
    unsigned int length;
    ListNode* closeBracket;
} BracketsRange;

typedef struct {
    void (*parseVariableDefinition)(List* tokens, int listLimit, ParserResult* result);
    void (*parseFunctionCall)(List* tokens, int listLimit, BOOL semicolonAtTheEnd, ParserResult* result);
    void (*parseFunctionDefinition)(List* tokens, int listLimit, ParserResult* result);
    void (*parseReturnStatement)(List* tokens, int listLimit, ParserResult* result);
    void (*parseBracketsRange)(List* tokens, unsigned int listLimit, BracketsRange* result);
    void (*parseVariableAssignment)(List* tokens, int listLimit, ParserResult* result);
    void (*parseIfStatement)(List* tokens, int listLimit, ParserResult* result);
} ParsersType;

ParsersType Parsers;

#endif // PARSERS_H
