#if !defined(PARSERS_H)
#define PARSERS_H

#include "../../linkedList/linkedList.h"
#include "../ast.h"

typedef struct ParsersType {
    ASTNode* (*parseVariableDefinition)(List* tokens, ListNode* lastNode);
    ASTNode* (*parseFunctionCall)(List* tokens, ListNode* lastItem);
    ASTNode* (*parseFunctionDefinition)(List* tokens, ListNode* lastItem);
} ParsersType;

ParsersType Parsers;

#endif // PARSERS_H
