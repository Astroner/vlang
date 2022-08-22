#include "parseStatements.h"

#include <stdio.h>
#include <stdlib.h>

#include "../../token/token.h"
#include "parsers.h"


#define GUESS_TYPE unsigned int
#define GUESS_TYPE_BLANK 0
#define GUESS_TYPE_VARIABLE_DEFINITION 1
#define GUESS_TYPE_FUNCTION_DEFINITION 2
#define GUESS_TYPE_FUNCTION_CALL 4
#define guessIncludes(guess, toCheck) ((guess & (toCheck)) == (toCheck))

/**
 * @brief   
 * 
 * @param tokens List<Token>
 * @param length content length (0 means no limit)
 * @return List* 
 */
List* parseStatements(List* tokens, unsigned int contentLength) {
    List* nodes = LinkedList.createList();

    GUESS_TYPE guess = GUESS_TYPE_BLANK;

    ListNode* current = tokens;
    ListNode* nodeStart = current;

    unsigned int length = 1;
    
    ParserResult parserResult;
    while(1) {
        Token* token = current->value;
        switch(token->type) {
            case TOKEN_NUMBER_KEYWORD: {
                if(guess == GUESS_TYPE_BLANK) {
                    guess = GUESS_TYPE_FUNCTION_DEFINITION | GUESS_TYPE_VARIABLE_DEFINITION;
                } else {
                    fprintf(stderr, "[ERROR][AST][3b19eecf0793] Unexpected usage of 'number' keyword\n");
                    exit(1);
                }
                break;
            }
            case TOKEN_IDENTIFIER: {
                if(guess == GUESS_TYPE_BLANK) {
                    Parsers.parseFunctionCall(
                        nodeStart,
                        &parserResult
                    );
                    LinkedList.pushItem(
                        nodes, 
                        parserResult.node
                    );
                    current = parserResult.lastNode;
                    nodeStart = current->next;
                    length += parserResult.length - 1;
                } else if(!guessIncludes(guess, GUESS_TYPE_FUNCTION_DEFINITION | GUESS_TYPE_VARIABLE_DEFINITION)) {
                    fprintf(stderr, "[ERROR][AST][531840349f39] Unexpected identifier '%s'\n", token->value);
                    exit(1);
                }
                break;
            }
            case TOKEN_EQUAL: {
                if(guessIncludes(guess, GUESS_TYPE_VARIABLE_DEFINITION)) {
                    Parsers.parseVariableDefinition(
                        nodeStart,
                        &parserResult
                    );
                    LinkedList.pushItem(
                        nodes, 
                        parserResult.node
                    );
                    current = parserResult.lastNode;
                    nodeStart = current->next;
                    guess = GUESS_TYPE_BLANK;
                    length += parserResult.length - 3;
                } else {
                    fprintf(stderr, "[ERROR][AST][aafda74de839] Unexpected token '='\n");
                    exit(1);
                }
                break;
            }
            case TOKEN_OPEN_BRACKET: {
                if(guessIncludes(guess, GUESS_TYPE_FUNCTION_DEFINITION)) {
                    Parsers.parseFunctionDefinition(
                        nodeStart,
                        &parserResult
                    );
                    LinkedList.pushItem(
                        nodes, 
                        parserResult.node
                    );
                    current = parserResult.lastNode;
                    nodeStart = current->next;
                    guess = GUESS_TYPE_BLANK;
                    length += parserResult.length - 3;
                } else {
                    fprintf(stderr, "[ERROR][AST][c9528490b4c6] Unexpected token '('\n");
                    exit(1);
                }
                break;
            }
            case TOKEN_RETURN_KEYWORD: {
                if(guess == GUESS_TYPE_BLANK && contentLength != 0) {
                    Parsers.parseReturnStatement(nodeStart, &parserResult);
                    LinkedList.pushItem(nodes, parserResult.node);
                    current = parserResult.lastNode;
                    nodeStart = current->next;
                    length += parserResult.length - 1;
                } else {
                    fprintf(stderr, "[ERROR][AST][18d1564eb42a] Unexpected 'return' keyword\n");
                    exit(1);
                }
                break;
            }
            default: {
                fprintf(stderr, "[ERROR][AST][d8ad5bfe5439] Unexpected token '%s'\n", t2s(token));
                exit(1);
            }
        }
        length++;

        if(!(current = current->next) || (contentLength > 0 && length == contentLength)) break;
    }

    return nodes;
}