#include "parsers.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../creators/creators.h"
#include "../../main.h"
#include "../../token/token.h"

#include "parseTokenExpression.h"
#include "parseStatements.h"

static void parseVariableDefinition(List* tokens, ParserResult* result) {
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
            result->lastNode = current;
            break;
        }
        if(!(current = current->next)) {
            fprintf(stderr, "[ERROR][AST][28af67d048f6] Expected semicolon at the end of the expression\n");
            exit(1);
        }
        expressionLength++;
    }

    result->node = Creators.createVariableDeclaration(name, variableType, value);
    result->length = expressionLength + 4;
}

static void parseFunctionCall(List* tokens, ParserResult* result) {
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

    ListNode* semicolonNode = current->next;

    if(semicolonNode == NULL || ((Token*)semicolonNode->value)->type != TOKEN_SEMICOLON) {
        fprintf(stderr, "[ERROR][AST][bcbf3f4f793e] Unexpected semicolon at the end of the function call\n");
        exit(1);
    }

    result->lastNode = semicolonNode;
    result->length = length + 3;
    result->node = Creators.createFunctionCall(name, argumentsLength, arguments);
}

static ASTNode* parseFunctionArgument(List* tokens, unsigned int argumentLength) {
    ASTNode* name;
    AST_NODE_TYPE type = AST_NODE_TYPE_BLANK;

    Token* typeToken = tokens->value;

    if(typeToken->type == TOKEN_NUMBER_KEYWORD) {
        type = AST_NODE_TYPE_NUMBER;
    } else {
        fprintf(stderr, "[ERROR][AST][2847cafd97b1] Unexpected token as type '%s'\n", t2s(typeToken));
        exit(1);
    }

    ListNode* nameNode = tokens->next;
    if(nameNode == NULL) {
        fprintf(stderr, "[ERROR][AST][f5096b36fff0] Expected argument function name\n");
        exit(1);
    }

    Token* nameToken = nameNode->value;
    if(nameToken->type == TOKEN_IDENTIFIER) {
        name = Creators.createIdentifier(nameToken->value);
    } else {
        fprintf(stderr, "[ERROR][AST][2d7dc9c3f1c9] Unexpected token as function argument name '%s'\n", t2s(nameToken));
        exit(1);
    }

    return Creators.createFunctionArgument(name, type);
}

static void parseFunctionDefinition(List* tokens, ParserResult* result) {
    ASTNode* name;
    List* arguments = LinkedList.createList();
    AST_NODE_TYPE returnType = AST_NODE_TYPE_BLANK; 

    Token* typeToken = tokens->value;

    if(typeToken->type == TOKEN_NUMBER_KEYWORD) {
        returnType = AST_NODE_TYPE_NUMBER;
    } else {
        fprintf(stderr, "[ERROR][AST][858cc4f36b7a] Unexpected token as return type '%s'\n", t2s(typeToken));
        exit(1);
    }

    ListNode* nameNode = tokens->next;
    if(nameNode == NULL) {
        fprintf(stderr, "[ERROR][AST][313a20c4b58b] Expected function name\n");
        exit(1);
    }
    Token* nameToken = nameNode->value;
    if(nameToken->type == TOKEN_IDENTIFIER) {
        name = Creators.createIdentifier(nameToken->value);
    } else {
        fprintf(stderr, "[ERROR][AST][9405dd6da2be] Unexpected token as function name '%s'\n", t2s(nameToken));
        exit(1);
    }

    if(nameNode->next == NULL || ((Token*)nameNode->next->value)->type != TOKEN_OPEN_BRACKET) {
        fprintf(stderr, "[ERROR][AST][4958a493df6d] Expected open bracket but got '%s'\n", t2s(((Token*)nameNode->next->value)));
        exit(1);
    }

    ListNode* current = nameNode->next->next;
    if(current == NULL) {
        fprintf(stderr, "[ERROR][AST][a47b9f703c02] Expected arguments\n");
        exit(1);
    }

    unsigned int argumentsCount = 0;
    unsigned int argumentsTokenLength = 0;
    unsigned int currentArgumentLength = 0;
    ListNode* currentArgumentStart = current;
    Token* token;
    while(1) {
        argumentsTokenLength++;
        token = current->value;
        if(token->type == TOKEN_COMMA) {
            LinkedList.pushItem(
                arguments,
                parseFunctionArgument(
                    currentArgumentStart,
                    currentArgumentLength
                )
            );
            currentArgumentLength = 0;
            currentArgumentStart = current->next;
            argumentsCount++;
        } else if(token->type == TOKEN_CLOSE_BRACKET) {
            LinkedList.pushItem(
                arguments,
                parseFunctionArgument(
                    currentArgumentStart,
                    currentArgumentLength
                )
            );
            argumentsCount++;
            break;
        } else {
            currentArgumentLength++;
        }        

        if(!(current = current->next)) {
            fprintf(stderr, "[ERROR][AST][d81a0630e127] Expected end of arguments ')'\n");
            exit(1);
        }
    }

    ListNode* bodyOpenNode = current->next;
    if(bodyOpenNode == NULL || ((Token*)bodyOpenNode->value)->type != TOKEN_OPEN_CURLY_BRACKET) {
        fprintf(stderr, "[ERROR][AST][770a2b72aa1e] Expected function body '{'\n");
        exit(1);
    }

    current = bodyOpenNode->next;
    if(current == NULL) {
        fprintf(stderr, "[ERROR][AST][fdc18f2884b8] Expected function body after '{'\n");
        exit(1);
    }
    unsigned int bodyLength = 0;
    while(1) {
        token = current->value;
        if(token->type == TOKEN_CLOSE_CURLY_BRACKET) {
            break;
        } else {
            bodyLength++;
        }
        if(!(current = current->next)) {
            fprintf(stderr, "[ERROR][AST][a6c5b4093efc] Expected function body end with '}'\n");
            exit(1);
        }
    }
    List* statements = parseStatements(bodyOpenNode->next, bodyLength);


    result->lastNode = current;
    result->length = 3 + argumentsTokenLength + 1 + bodyLength + 1;
    result->node = Creators.createFunctionDefinition(
        name, 
        returnType, 
        argumentsCount,
        arguments,
        statements
    );
}

static void parseReturnStatement(List* tokens, ParserResult* result) {
    ReturnStatementValue* value;

    ListNode* current = tokens->next;
    if(current == NULL) {
        fprintf(stderr, "[ERROR][AST][f9758b8cf250] Expected expression after return statement\n");
        exit(1);
    }
    ListNode* expressionStart = current;
    int expressionLength = 0;
    while(1) {
        Token* token = current->value;
        if(token->type == TOKEN_SEMICOLON) {
            value = parseTokenExpression(expressionStart, FALSE, FALSE, NULL, NULL, expressionLength);
            break;
        }

        expressionLength++;
        if(!(current = current->next)) {
            fprintf(stderr, "[ERROR][AST][7200110bae25] Expected ';' at the end of the return statement\n");
            exit(1);
        }
    }

    result->node = Creators.createReturnStatement(value);
    result->lastNode = current;
    result->length = 1 + expressionLength;
}

ParsersType Parsers = {
    parseVariableDefinition,
    parseFunctionCall,
    parseFunctionDefinition,
    parseReturnStatement,
};
