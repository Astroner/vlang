#include "parseStatements.h"

#include <stdio.h>
#include <stdlib.h>

#include "../../token/token.h"
#include "parsers.h"


#define GUESS_TYPE unsigned int
#define GUESS_TYPE_BLANK 0b0
#define GUESS_TYPE_VARIABLE_DEFINITION 0b1
#define GUESS_TYPE_FUNCTION_DEFINITION 0b10
#define GUESS_TYPE_FUNCTION_CALL 0b100
#define GUESS_TYPE_VARIABLE_ASSIGNMENT 0b1000
#define guessIncludes(guess, toCheck) ((guess & (toCheck)) == (toCheck))

/**
 * @brief   
 * 
 * @param tokens List<Token>
 * @param length content length (0 means no limit)
 * @return List* 
 */
List* parseStatements(List* tokens, unsigned int contentLength, BOOL isInGlobalScope) {
    List* nodes = LinkedList.createList();

    GUESS_TYPE guess = GUESS_TYPE_BLANK;

    ListNode* current = tokens;
    ListNode* nodeStart = current;

    unsigned int length = 0;
    
    ParserResult parserResult;
    while(1) {
        length++;
        Token* token = current->value;
        // printf("Token: %s, Length: %d, LIM: %d\n", t2s(token), length, contentLength);
        switch(token->type) {
            case TOKEN_BOOLEAN_KEYWORD:
            case TOKEN_NUMBER_KEYWORD: {
                if(guess == GUESS_TYPE_BLANK) {
                    guess = GUESS_TYPE_FUNCTION_DEFINITION | GUESS_TYPE_VARIABLE_DEFINITION;
                } else {
                    fprintf(stderr, "[ERROR][AST][3b19eecf0793] Unexpected type keyword\n");
                    exit(1);
                }
                break;
            }
            case TOKEN_IDENTIFIER: {
                if(guess == GUESS_TYPE_BLANK) {
                    guess = GUESS_TYPE_FUNCTION_CALL | GUESS_TYPE_VARIABLE_ASSIGNMENT;
                } else if(!guessIncludes(guess, GUESS_TYPE_FUNCTION_DEFINITION) && !guessIncludes(guess, GUESS_TYPE_VARIABLE_DEFINITION)) {
                    fprintf(stderr, "[ERROR][AST][531840349f39] Unexpected identifier '%s'\n", token->value);
                    exit(1);
                }
                break;
            }
            case TOKEN_EQUAL: {
                if(guessIncludes(guess, GUESS_TYPE_VARIABLE_DEFINITION)) {
                    Parsers.parseVariableDefinition(
                        nodeStart,
                        contentLength - length + 2,
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
                } else if(guessIncludes(guess, GUESS_TYPE_VARIABLE_ASSIGNMENT)) {
                    Parsers.parseVariableAssignment(
                        nodeStart,
                        contentLength - length + 2,
                        &parserResult
                    );
                    LinkedList.pushItem(
                        nodes, 
                        parserResult.node
                    );
                    current = parserResult.lastNode;
                    nodeStart = current->next;
                    guess = GUESS_TYPE_BLANK;
                    length += parserResult.length - 2;
                } else {
                    fprintf(stderr, "[ERROR][AST][aafda74de839] Unexpected token '='\n");
                    exit(1);
                }
                break;
            }
            case TOKEN_OPEN_BRACKET: {
                if(guessIncludes(guess, GUESS_TYPE_FUNCTION_DEFINITION) && isInGlobalScope) {
                    Parsers.parseFunctionDefinition(
                        nodeStart,
                        contentLength - length + 2,
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
                } else if(guessIncludes(guess, GUESS_TYPE_FUNCTION_CALL)){
                    Parsers.parseFunctionCall(
                        nodeStart,
                        contentLength - length + 2,
                        TRUE,
                        &parserResult
                    );
                    LinkedList.pushItem(
                        nodes, 
                        parserResult.node
                    );
                    current = parserResult.lastNode;
                    nodeStart = current->next;
                    length += parserResult.length - 2;
                    guess = GUESS_TYPE_BLANK;
                } else {
                    fprintf(stderr, "[ERROR][AST][c9528490b4c6] Unexpected token '('\n");
                    exit(1);
                }
                break;
            }
            case TOKEN_RETURN_KEYWORD: {
                if(guess == GUESS_TYPE_BLANK && !isInGlobalScope) {
                    Parsers.parseReturnStatement(nodeStart, contentLength - length, &parserResult);
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
            case TOKEN_IF_KEYWORD: {
                if(guess == GUESS_TYPE_BLANK) {
                    Parsers.parseIfStatement(nodeStart, contentLength - length, &parserResult);
                    LinkedList.pushItem(nodes, parserResult.node);
                    current = parserResult.lastNode;
                    nodeStart = current->next;
                    length += parserResult.length - 1;
                } else {
                    fprintf(stderr, "[ERROR][AST][73655fc4e4a2] Unexpected 'if' keyword\n");
                    exit(1);
                }
                break;
            }
            case TOKEN_PURE_KEYWORD: {
                if(guess == GUESS_TYPE_BLANK && isInGlobalScope) {
                    Parsers.parseFunctionDefinition(
                        nodeStart,
                        contentLength - length,
                        &parserResult
                    );
                    LinkedList.pushItem(
                        nodes, 
                        parserResult.node
                    );
                    current = parserResult.lastNode;
                    nodeStart = current->next;
                    guess = GUESS_TYPE_BLANK;
                    length += parserResult.length - 1;
                } else {
                    fprintf(stderr, "[ERROR][AST][73675fc4e4a2] Unexpected 'pure' keyword\n");
                    exit(1);
                }
                break;
            }
            case TOKEN_VOID_KEYWORD: {
                if(guess == GUESS_TYPE_BLANK && isInGlobalScope) {
                    Parsers.parseFunctionDefinition(
                        nodeStart,
                        contentLength - length,
                        &parserResult
                    );
                    LinkedList.pushItem(
                        nodes, 
                        parserResult.node
                    );
                    current = parserResult.lastNode;
                    nodeStart = current->next;
                    length += parserResult.length - 1;
                } else {
                    fprintf(stderr, "[ERROR][AST][73675fc4e4a2] Unexpected 'pure' keyword\n");
                    exit(1);
                }
                break;
            }
            default: {
                fprintf(stderr, "[ERROR][AST][d8ad5bfe5439] Unexpected token '%s'\n", t2s(token));
                exit(1);
            }
        }

        if(!(current = current->next) || (contentLength > 0 && length == contentLength)) break;
    }

    return nodes;
}