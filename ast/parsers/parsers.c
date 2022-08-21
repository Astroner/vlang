#include "parsers.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../creators/creators.h"
#include "../../main.h"
#include "../../token/token.h"

#include "parseTokenExpression.h"

static ASTNode* parseVariableDefinition(List* tokens, ListNode* lastNode) {
    ASTNode* name;
    AST_NODE_TYPE variableType = AST_NODE_TYPE_BLANK;
    ASTNode* value;

    Token* typeToken = tokens->value;

    if(typeToken->type == TOKEN_NUMBER_KEYWORD) {
        variableType = AST_NODE_TYPE_NUMBER;
    } else {
        fprintf(stderr, "[ERROR][AST][1a41c4f5d539] Unexpected token as type '%s'\n", t2s(typeToken));
        exit(1);
    }

    ListNode* nameNode = tokens->next;
    if(nameNode == NULL) {
        fprintf(stderr, "[ERROR][AST][9b161c105716] Expected variable name\n");
        exit(1);
    }

    Token* nameToken = nameNode->value;
    if(nameToken->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "[ERROR][AST][96aa13f0528a] Unexpected token as identifier '%s'\n", t2s(nameToken));
        exit(1);
    }

    name = Creators.createIdentifier(nameToken->value);

    ListNode* equalNode = nameNode->next;
    if(equalNode == NULL || ((Token*)equalNode->value)->type != TOKEN_EQUAL) {
        fprintf(stderr, "[ERROR][AST][38b30eefc7e5] Expected '=' token\n");
        exit(1);
    }

    ListNode* current = equalNode->next;
    ListNode* expressionStart = current;
    unsigned int expressionLength = 0;
    while(1) {
        Token* token = current->value;
        if(token->type == TOKEN_SEMICOLON) {
            value = parseTokenExpression(expressionStart, FALSE, FALSE, NULL, NULL, expressionLength);
            *lastNode = *current;
            break;
        }
        if(!(current = current->next)) {
            fprintf(stderr, "[ERROR][AST][28af67d048f6] Expected semicolon at the end of the expression\n");
            exit(1);
        }
        expressionLength++;
    }

    return Creators.createVariableDeclaration(name, variableType, value);
}

static ASTNode* parseFunctionCall(List* tokens, ListNode* lastItem) {
    ASTNode* name;
    int argumentsLength = 0;
    List* arguments = LinkedList.createList();

    Token* nameToken = tokens->value;

    name = Creators.createIdentifier(nameToken->value);

    ListNode* openToken = tokens->next;
    
    assert(openToken && ((Token*)(openToken->value))->type == TOKEN_OPEN_BRACKET);

    ListNode* current = openToken->next;
    ListNode* argStart = current;
    int argumentTokenLength = 0;
    int bracketsDepth = 0;
    int length = 0;
    while(1) {
        length++;
        BOOL countable = TRUE;
        Token* token = current->value;

        if(token->type == TOKEN_COMMA && bracketsDepth == 0) {
            LinkedList.pushItem(arguments, parseTokenExpression(argStart, FALSE, FALSE, NULL, NULL, argumentTokenLength));
            if(!current->next) {
                fprintf(stderr, "[ERROR][AST][6f1c62336fdb] Expected another argument after ',' token\n");
                exit(1);
            }
            argStart = current->next;
            argumentTokenLength = 0;
            countable = FALSE;
            argumentsLength++;
        } else if(token->type == TOKEN_OPEN_BRACKET) {
            bracketsDepth++;
        } else if(token->type == TOKEN_CLOSE_BRACKET) {
            if(bracketsDepth == 0) {
                argumentsLength++;
                LinkedList.pushItem(arguments, parseTokenExpression(argStart, FALSE, FALSE, NULL, NULL, argumentTokenLength));
                countable = FALSE;
                break;
            } else {
                bracketsDepth--;
            }
        } 
        
        if(countable) {
            argumentTokenLength++;
        }

        if(!(current = current->next)) {
            fprintf(stderr, "[ERROR][AST][1541db82e715] Unexpected end of arguments\n");
            exit(1);
        }
    }

    *lastItem = *current;

    return Creators.createFunctionCall(name, argumentsLength, arguments);
}

ParsersType Parsers = {
    parseVariableDefinition,
    parseFunctionCall
};
