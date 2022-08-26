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
    void (*parseVariableDefinition)(List* tokens, int contentLength, ParserResult* result);
    void (*parseFunctionCall)(List* tokens, int contentLength, BOOL semicolonAtTheEnd, ParserResult* result);
    void (*parseFunctionDefinition)(List* tokens, int contentLength, ParserResult* result);
    void (*parseReturnStatement)(List* tokens, int contentLength, ParserResult* result);
} ParsersType;

ParsersType Parsers;

#endif // PARSERS_H
