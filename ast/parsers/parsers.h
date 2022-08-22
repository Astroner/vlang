#if !defined(PARSERS_H)
#define PARSERS_H

#include "../../linkedList/linkedList.h"
#include "../ast.h"

typedef struct {
    ASTNode* node;
    ListNode* lastNode;
    unsigned int length;
} ParserResult;

typedef struct {
    void (*parseVariableDefinition)(List* tokens, ParserResult* result);
    void (*parseFunctionCall)(List* tokens, ParserResult* result);
    void (*parseFunctionDefinition)(List* tokens, ParserResult* result);
    void (*parseReturnStatement)(List* tokens, ParserResult* result);
} ParsersType;

ParsersType Parsers;

#endif // PARSERS_H
