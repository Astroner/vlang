#if !defined(PARSE_TOKEN_EXPRESSION)
#define PARSE_TOKEN_EXPRESSION

#include "../../linkedList/linkedList.h"
#include "../../main.h"
#include "../ast.h"

ASTNode* parseTokenExpression(
    List* tokens,           // Token List
    BOOL breakOnBracket,    // TRUE if u want to stop parsing on first closing bracket
    BOOL withLength,        // Should function store count of parsed tokens into lengthPointer
    int* lengthPointer,     // Pointer for length. Primary used for skip variable
    ASTNode* __first,       // Internal recursion arg for operation prior handling when we already have first member.
    int limit               // Limit for list reading; -1 means no limit
);

#endif // PARSE_TOKEN_EXPRESSION
