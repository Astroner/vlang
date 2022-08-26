#if !defined(PARSE_TOKEN_EXPRESSION)
#define PARSE_TOKEN_EXPRESSION

#include "../../linkedList/linkedList.h"
#include "../../main.h"
#include "../ast.h"

ASTNode* parseTokenExpression(
    List* tokens,
    unsigned int contentLength
);

#endif // PARSE_TOKEN_EXPRESSION
