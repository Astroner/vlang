#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

#include "../main.h"

#include "parsers/parseStatements.h"

#include "dicts.c"
/**
 * @brief 
 * 
 * @param tokens List<Token>*
 * @return List<ASTNodes>* 
 */
static List* createASTFromTokens(List* tokens) {
    return parseStatements(tokens, 0, TRUE);
}

ASTModuleType AST = {
    createASTFromTokens,
    NodeKind, 
    NodeType, 
    BinomialExpressionType, 
    UnaryExpressionType 
};