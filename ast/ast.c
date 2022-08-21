#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

#include "../token/token.h"
#include "../main.h"
#include "creators/creators.h"
#include "parsers/parsers.h"

#include "dicts.c"

#define GUESS_TYPE unsigned int
#define GUESS_TYPE_BLANK 0
#define GUESS_TYPE_VARIABLE_DEFINITION 1
#define GUESS_TYPE_FUNCTION_DEFINITION 2
#define GUESS_TYPE_FUNCTION_CALL 4
#define guessIncludes(guess, toCheck) guess & (toCheck) == (toCheck)

/**
 * @brief 
 * 
 * @param tokens List<Token>*
 * @return List<ASTNodes>* 
 */
static List* createASTFromTokens(List* tokens) {
    List* nodes = LinkedList.createList();

    GUESS_TYPE guess = GUESS_TYPE_BLANK;

    ListNode* current = tokens;
    ListNode* nodeStart = current;

    while(1) {
        Token* token = current->value;
        switch(token->type) {
            case TOKEN_NUMBER_KEYWORD: {
                if(guess == GUESS_TYPE_BLANK) {
                    LinkedList.pushItem(
                        nodes, 
                        Parsers.parseVariableDefinition(
                            nodeStart,
                            current
                        )
                    );
                    nodeStart = current->next;
                } else {
                    fprintf(stderr, "[ERROR][AST][531840349f39] Unexpected usage of 'number' keyword\n");
                    exit(1);
                }
                break;
            }
            case TOKEN_IDENTIFIER: {
                if(guess == GUESS_TYPE_BLANK) {
                    LinkedList.pushItem(
                        nodes, 
                        Parsers.parseFunctionCall(
                            nodeStart,
                            current
                        )
                    );
                    nodeStart = current->next;
                } else {
                    fprintf(stderr, "[ERROR][AST][531840349f39] Unexpected identifier '%s'\n", token->value);
                    exit(1);
                }
                break;
            }
        }

        if(!(current = current->next)) break;
    }

    return nodes;
}

ASTModuleType AST = {
    createASTFromTokens,
    NodeKind, 
    NodeType, 
    BinomialExpressionType, 
    UnaryExpressionType 
};
