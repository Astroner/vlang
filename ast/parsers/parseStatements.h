#if !defined(PARSE_STATEMENTS_FUNCTIONS)
#define PARSE_STATEMENTS_FUNCTIONS

#include "../../linkedList/linkedList.h"
#include "../../main.h"

List* parseStatements(List* tokens, unsigned int contentLength, BOOL isInsideFunction);

#endif //PARSE_STATEMENTS_FUNCTIONS