#include "parsers.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../creators/creators.h"
#include "../../main.h"
#include "../../token/token.h"

#include "parseExpression.h"
#include "parseStatements.h"

static void parseVariableDefinition(List* tokens, int listLimit, ParserResult* result) {
    ASTNode* name;
    AST_NODE_TYPE variableType = AST_NODE_TYPE_BLANK;
    ASTNode* value;

    Token* typeToken = tokens->value;

    if(typeToken->type == TOKEN_NUMBER_KEYWORD) {
        variableType = AST_NODE_TYPE_NUMBER;
    } else if(typeToken->type == TOKEN_BOOLEAN_KEYWORD) {
        variableType = AST_NODE_TYPE_BOOLEAN;
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
            value = parseExpression(expressionStart, expressionLength);
            result->lastNode = current;
            break;
        }
        expressionLength++;
        if(!(current = current->next) || (listLimit > 0 && expressionLength == listLimit)) {
            fprintf(stderr, "[ERROR][AST][28af67d041f6] Expected semicolon at the end of the expression\n");
            exit(1);
        }
    }

    result->node = Creators.createVariableDeclaration(name, variableType, value);
    result->length = expressionLength + 4;
}

static void parseFunctionCall(List* tokens, int listLimit, BOOL semicolonAtTheEnd,ParserResult* result) {
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
            LinkedList.pushItem(arguments, parseExpression(argStart, argumentTokenLength));
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
                if(argumentTokenLength > 0) {
                    argumentsLength++;
                    LinkedList.pushItem(arguments, parseExpression(argStart, argumentTokenLength));
                    countable = FALSE;
                }
                break;
            } else {
                bracketsDepth--;
            }
        } 
        
        if(countable) {
            argumentTokenLength++;
        }

        if(!(current = current->next) || (listLimit > 0 && length == listLimit)) {
            fprintf(stderr, "[ERROR][AST][1541db82e715] Unexpected end of arguments\n");
            exit(1);
        }
    }

    ListNode* semicolonNode = current->next;

    if(semicolonAtTheEnd && (semicolonNode == NULL || ((Token*)semicolonNode->value)->type != TOKEN_SEMICOLON)) {
        fprintf(stderr, "[ERROR][AST][bcbf3f4f793e] Expected semicolon at the end of the function call\n");
        exit(1);
    }

    if(semicolonAtTheEnd) {
        result->lastNode = semicolonNode;
    } else {
        result->lastNode = current;
    }
    result->length = length + 3;
    result->node = Creators.createFunctionCall(name, argumentsLength, arguments);
}

static ASTNode* parseFunctionArgument(List* tokens, unsigned int argumentLength) {
    ASTNode* name;
    AST_NODE_TYPE type = AST_NODE_TYPE_BLANK;

    Token* typeToken = tokens->value;

    if(typeToken->type == TOKEN_NUMBER_KEYWORD) {
        type = AST_NODE_TYPE_NUMBER;
    } else if(typeToken->type == TOKEN_BOOLEAN_KEYWORD) {
        type = AST_NODE_TYPE_BOOLEAN;
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

static void parseFunctionDefinition(List* tokens, int listLimit, ParserResult* result) {
    BOOL pure = FALSE;
    BOOL memoized = FALSE;
    ASTNode* name;
    List* arguments = LinkedList.createList();
    AST_NODE_TYPE returnType = AST_NODE_TYPE_BLANK; 

    ListNode* typeNode = tokens;

    if(((Token*)typeNode->value)->type == TOKEN_PURE_KEYWORD) {
        pure = TRUE;
        if(typeNode->next == NULL) {
            fprintf(stderr, "[ERROR][AST][858cc4f3c27a] Unexpected function definition after 'pure' keyword \n");
            exit(1);
        }
        typeNode = typeNode->next;
    }
    if(((Token*)typeNode->value)->type == TOKEN_MEMOIZED_KEYWORD) {
        if(pure == FALSE) {
            fprintf(stderr, "[ERROR][AST][858cc4f3c29d] You cannot memoize impure function\n");
            exit(1);
        }
        memoized = TRUE;
        if(typeNode->next == NULL) {
            fprintf(stderr, "[ERROR][AST][858cc8s3c27a] Unexpected function definition after 'memoized' keyword \n");
            exit(1);
        }
        typeNode = typeNode->next;
    }

    Token* typeToken = typeNode->value;

    if(typeToken->type == TOKEN_NUMBER_KEYWORD) {
        returnType = AST_NODE_TYPE_NUMBER;
    } else if(typeToken->type == TOKEN_BOOLEAN_KEYWORD) {
        returnType = AST_NODE_TYPE_BOOLEAN;
    } else if(typeToken->type == TOKEN_VOID_KEYWORD) {
        returnType = AST_NODE_TYPE_BLANK;
    } else {
        fprintf(stderr, "[ERROR][AST][858cc4f36b7a] Unexpected token as return type '%s'\n", t2s(typeToken));
        exit(1);
    }

    ListNode* nameNode = typeNode->next;
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
            if(argumentsTokenLength == 0) break;
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

        argumentsTokenLength++;
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

    BracketsRange bracketRange;
    Parsers.parseCurlyBracketsRange(bodyOpenNode, 0, &bracketRange);

    List* statements = parseStatements(bodyOpenNode->next, bracketRange.length - 2, FALSE);

    result->lastNode = bracketRange.closeBracket;
    result->length = 
        3 
        + argumentsTokenLength 
        + bracketRange.length 
        + pure ? 1 : 0
        + memoized ? 1 : 0;
    result->node = Creators.createFunctionDefinition(
        pure,
        memoized,
        name, 
        returnType, 
        argumentsCount,
        arguments,
        statements
    );
}

static void parseReturnStatement(List* tokens, int listLimit, ParserResult* result) {
    ASTNode* value;

    ListNode* current = tokens->next;
    if(current == NULL) {
        fprintf(stderr, "[ERROR][AST][f9758b8cf250] Expected expression after return statement\n");
        exit(1);
    }
    ListNode* expressionStart = current;
    int expressionLength = 0;
    while(1) {
        expressionLength++;
        Token* token = current->value;
        if(token->type == TOKEN_SEMICOLON) {
            value = parseExpression(expressionStart, expressionLength - 1);
            break;
        }

        if(!(current = current->next) || (listLimit > 0 && expressionLength == listLimit)) {
            fprintf(stderr, "[ERROR][AST][7200110bae25] Expected ';' at the end of the return statement\n");
            exit(1);
        }
    }

    result->node = Creators.createReturnStatement(value);
    result->lastNode = current;
    result->length = 1 + expressionLength;
}

static void parseBracketsRange(List* tokens, unsigned int listLimit, BracketsRange* result) {
    ListNode* openBracketNode = tokens;

    if(openBracketNode->next == NULL) {
        fprintf(stderr, "[ERROR][AST][6e1e02e08a18] Expected expression\n");
        exit(1);
    }

    ListNode* current = openBracketNode->next;
    unsigned int rangeLength = 0;
    unsigned int bracketsDepth = 0;
    while(1) {
        Token* token = current->value;
        rangeLength++;
        // printf("Token: '%s', length: %d\n", t2s(token), rangeLength);

        switch(token->type) {
            case TOKEN_OPEN_BRACKET: {
                bracketsDepth++;
                break;
            }
            case TOKEN_CLOSE_BRACKET: {
                if(bracketsDepth == 0) {
                    result->closeBracket = current;
                    result->length = rangeLength + 1;
                    return;
                } else {
                    bracketsDepth--;
                }
                break;
            }
        }
        

        if(!(current = current->next) || (listLimit > 0 && rangeLength == listLimit)) {
            fprintf(stderr, "[ERROR][AST][0036d7ae61c6] Expected closing bracket\n");
            exit(1);
        }
    }
}

static void parseCurlyBracketsRange(List* tokens, unsigned int listLimit, BracketsRange* result) {
    ListNode* openBracketNode = tokens;

    if(openBracketNode->next == NULL) {
        fprintf(stderr, "[ERROR][AST][589d73760b05] Expected expression\n");
        exit(1);
    }

    ListNode* current = openBracketNode->next;
    unsigned int rangeLength = 0;
    unsigned int bracketsDepth = 0;
    while(1) {
        Token* token = current->value;
        rangeLength++;
        // printf("Token: '%s', length: %d\n", t2s(token), rangeLength);

        switch(token->type) {
            case TOKEN_OPEN_CURLY_BRACKET: {
                bracketsDepth++;
                break;
            }
            case TOKEN_CLOSE_CURLY_BRACKET: {
                if(bracketsDepth == 0) {
                    result->closeBracket = current;
                    result->length = rangeLength + 1;
                    return;
                } else {
                    bracketsDepth--;
                }
                break;
            }
        }
        

        if(!(current = current->next) || (listLimit > 0 && rangeLength == listLimit)) {
            fprintf(stderr, "[ERROR][AST][6e54788c34fa] Expected closing curly bracket\n");
            exit(1);
        }
    }
}

static void parseVariableAssignment(List* tokens, int listLimit, ParserResult* result) {
    ASTNode* name;
    ASTNode* value;

    Token* nameToken = tokens->value;
    if(nameToken->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "[ERROR][AST][96aa13f0528a] Unexpected token as identifier '%s'\n", t2s(nameToken));
        exit(1);
    }

    name = Creators.createIdentifier(nameToken->value);

    ListNode* equalNode = tokens->next;

    ListNode* current = equalNode->next;
    ListNode* expressionStart = current;
    unsigned int expressionLength = 0;
    while(1) {
        Token* token = current->value;
        if(token->type == TOKEN_SEMICOLON) {
            value = parseExpression(expressionStart, expressionLength);
            result->lastNode = current;
            break;
        }
        expressionLength++;
        if(!(current = current->next) || (listLimit > 0 && expressionLength == listLimit - 2)) {
            fprintf(stderr, "[ERROR][AST][28af67d048f6] Expected semicolon at the end of the expression\n");
            exit(1);
        }
    }
    result->node = Creators.createVariableAssignment(name, value);
    result->length = expressionLength + 3;
}

static void parseIfCondition(List* tokens, int listLimit, ParserResult* result) {
    unsigned int length = 0;

    ListNode* openNode = tokens->next;
    
    if(openNode == NULL || ((Token*)openNode->value)->type != TOKEN_OPEN_BRACKET) {
        fprintf(stderr, "[ERROR][AST][b7e40d022af3] Expected open bracket\n");
        exit(1);
    }

    BracketsRange bracketsRange;
    parseBracketsRange(openNode, listLimit, &bracketsRange);

    ListNode* expressionStart = openNode->next;

    if(expressionStart == NULL) {
        fprintf(stderr, "[ERROR][AST][3857aa02ec4c] Expected if-case condition\n");
        exit(1);
    }

    ASTNode* condition = parseExpression(expressionStart, bracketsRange.length - 2);

    length += bracketsRange.length;

    ListNode* curlyOpen = bracketsRange.closeBracket->next;

    if(curlyOpen == NULL || ((Token*)curlyOpen->value)->type != TOKEN_OPEN_CURLY_BRACKET) {
        fprintf(stderr, "[ERROR][AST][e3df03ea8978] Expected open curly bracket for if-condition\n");
        exit(1);
    }

    parseCurlyBracketsRange(curlyOpen, listLimit - bracketsRange.length - 1, &bracketsRange);
    
    result->lastNode = bracketsRange.closeBracket;
    result->length = length + bracketsRange.length + 1;
    if(bracketsRange.length == 2) {
        result->node = Creators.createIfCondition(condition, LinkedList.createList());
    } else {
        result->node = Creators.createIfCondition(
            condition, 
            parseStatements(curlyOpen->next, bracketsRange.length - 2, FALSE)
        );
    }
}

static void parseElseStatement(List* tokens, int listLimit, ParserResult* result) {
    unsigned int length = 0;

    ListNode* curlyOpen = tokens->next;

    if(curlyOpen == NULL || ((Token*)curlyOpen->value)->type != TOKEN_OPEN_CURLY_BRACKET) {
        fprintf(stderr, "[ERROR][AST][e3df03ea8978] Expected open curly bracket for else-statement\n");
        exit(1);
    }

    BracketsRange bracketsRange;

    parseCurlyBracketsRange(curlyOpen, listLimit - 1, &bracketsRange);
    
    result->lastNode = bracketsRange.closeBracket;
    result->length = length + bracketsRange.length + 1;
    if(bracketsRange.length == 2) {
        result->node = Creators.createElseStatement(LinkedList.createList());
    } else {
        result->node = Creators.createElseStatement(
            parseStatements(curlyOpen->next, bracketsRange.length - 2, FALSE)
        );
    }
}

static void parseIfStatement(List* tokens, int listLimit, ParserResult* result) {
    List* conditions = LinkedList.createList();

    ParserResult parserResult;
    
    unsigned int length = 0;
    ListNode* current = tokens;
    while(1) {
        Token* token = current->value;
        length++;

        if(token->type == TOKEN_IF_KEYWORD) {
            parseIfCondition(current, listLimit - length + 1, &parserResult);
            current = parserResult.lastNode;
            length += parserResult.length - 1;
            LinkedList.pushItem(conditions, parserResult.node);

            if(current->next == NULL || ((Token*)current->next->value)->type != TOKEN_ELSE_KEYWORD) {
                break;
            }
        } else if(token->type == TOKEN_ELSE_KEYWORD) {
            if(
                current->next == NULL
                || (
                    ((Token*)current->next->value)->type != TOKEN_OPEN_CURLY_BRACKET
                    && ((Token*)current->next->value)->type != TOKEN_IF_KEYWORD
                )
            ) {
                fprintf(stderr, "[ERROR][AST][5e7fbd6744ef] Unexpected token 'else'\n");
                exit(1);
            } else if(((Token*)current->next->value)->type == TOKEN_OPEN_CURLY_BRACKET) {
                parseElseStatement(current, listLimit - length + 1, &parserResult);
                current = parserResult.lastNode;
                length += parserResult.length - 1;
                LinkedList.pushItem(conditions, parserResult.node);
                break;
            }
        } else {
            fprintf(stderr, "[ERROR][AST][b494de1fbb8e] Unexpected token '%s'\n", t2s(token));
            exit(1);
        }

        if(!(current = current->next) || length == listLimit) {
            fprintf(stderr, "[ERROR][AST][9a389a05f584] Expected end of if-statement\n");
            exit(1);
        }
    }

    result->node = Creators.createIfStatement(conditions);
    result->length = length;
    result->lastNode = current;
}

ParsersType Parsers = {
    parseVariableDefinition,
    parseFunctionCall,
    parseFunctionDefinition,
    parseReturnStatement,
    parseBracketsRange,
    parseVariableAssignment,
    parseIfStatement,
    parseCurlyBracketsRange
};
