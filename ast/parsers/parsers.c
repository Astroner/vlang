#include "parsers.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../creators/creators.h"
#include "../../main.h"
#include "../../token/token.h"

#define operationPriority(operation) binomialOperationPriorities[operation]

typedef unsigned char Priority;

static Priority binomialOperationPriorities[] = {
    /* AST_BINOMIAL_EXPRESSION_TYPE_BLANK */            0,
    /* AST_BINOMIAL_EXPRESSION_TYPE_SUM */              1,
    /* AST_BINOMIAL_EXPRESSION_TYPE_SUBTRACTION */      1,
    /* AST_BINOMIAL_EXPRESSION_TYPE_MULTIPLICATION */   2,
    /* AST_BINOMIAL_EXPRESSION_TYPE_DIVISION */         2,
    /* AST_BINOMIAL_EXPRESSION_TYPE_POWER */            3,
};

static ASTNode* parseTokenExpression(
    List* tokens,           // Token List
    BOOL breakOnBracket,    // TRUE if u want to stop parsing on first closing bracket
    BOOL withLength,        // Should function store count of parsed tokens into lengthPointer
    int* lengthPointer,     // Pointer for length. Primary used for skip variable
    ASTNode* __first,       // Internal recursion arg for operation prior handling when we already have first member.
    int limit               // Limit for list reading; -1 means no limit
) {
    ASTNode* node;

    ASTNode* first = NULL;
    if(__first != NULL) {
        first = __first;
    }
    ASTNode* second = NULL;
    AST_BINOMIAL_EXPRESSION_TYPE binomialExpressionType = AST_BINOMIAL_EXPRESSION_TYPE_BLANK;
    AST_UNARY_EXPRESSION_TYPE unaryExpressionType = AST_UNARY_EXPRESSION_TYPE_BLANK;

    int length = 0;
    ListNode* current = tokens;
    int skip = 0;
    BOOL breakFlag = FALSE;
    while(1) {
        length++;
        if(skip > 0) {
            skip--;
            if(breakFlag || !(current = current->next) || (limit > 0 && length == limit)) break;
            continue;
        }

        Token* token = current->value;
        switch(token->type) {
            // subjects handling
            case TOKEN_IDENTIFIER:
            case TOKEN_NUMBER_LITERAL: {
                ASTNode* subject;
                if(token->type == TOKEN_IDENTIFIER) subject = Creators.createIdentifier(token->value);
                if(token->type == TOKEN_NUMBER_LITERAL) subject = Creators.createNumberLiteral(token->value);
                
                // If we have unary operator then applying it
                if(unaryExpressionType != AST_UNARY_EXPRESSION_TYPE_BLANK) {
                    subject = Creators.createUnaryExpression(unaryExpressionType, subject);
                    unaryExpressionType = AST_UNARY_EXPRESSION_TYPE_BLANK;
                }

                if(first == NULL) first = subject;
                else if(binomialExpressionType == AST_BINOMIAL_EXPRESSION_TYPE_BLANK) {
                    fprintf(stderr, "[ERROR][AST][08d708f1691d] Unexpected token before operation '%s'\n", t2s(token));
                    exit(1);
                } else if(second == NULL){
                    second = subject;
                } else {
                    fprintf(stderr, "[ERROR][AST][e663a4a120d2] Unexpected token '%s'\n", t2s(token));
                    exit(1);
                }
                break;
            }
            case TOKEN_OPEN_BRACKET:
                if(current->next == NULL) {
                    fprintf(stderr, "[ERROR][AST][b7e1fc9cfa0b] Expected tokens after '('\n");
                    exit(1);
                }
                // Recursively parsing next tokens until ')' and storing length to skip parsed tokens;
                ASTNode* subject = parseTokenExpression(current->next, TRUE, TRUE, &skip, NULL, -1);

                if(first == NULL) first = subject;
                else if(binomialExpressionType == AST_BINOMIAL_EXPRESSION_TYPE_BLANK) {
                    fprintf(stderr, "[ERROR][AST][08d708f1691d] Unexpected token before operation '%s'\n", t2s(token));
                    exit(1);
                } else if(second == NULL) second = subject;
                break;
            case TOKEN_CLOSE_BRACKET:
                if(breakOnBracket) {
                    breakFlag = TRUE;
                } else {
                    fprintf(stderr, "[ERROR][AST][930eaf6d630c] Unexpected token ')'\n");
                    exit(1);
                }
                break;
            case TOKEN_PLUS:
            case TOKEN_DASH:
            case TOKEN_SLASH:
            case TOKEN_STAR:
            case TOKEN_CARET:
                if(
                    // If we dont have any subject
                    first == NULL || 
                    // Or we have first subject, but we dont have second and already know operation type
                    (
                        binomialExpressionType != AST_BINOMIAL_EXPRESSION_TYPE_BLANK && 
                        second == NULL
                    )
                ) {
                    // Applying unary operation
                    if(unaryExpressionType != AST_UNARY_EXPRESSION_TYPE_BLANK) {
                        fprintf(stderr, "[ERROR][AST][c6ae21efba71] Unexpected token '%s'\n", t2s(token));
                        exit(1);
                    }
                    if(token->type == TOKEN_DASH) unaryExpressionType = AST_UNARY_EXPRESSION_TYPE_NEGATIVE;
                    else {
                        fprintf(stderr, "[ERROR][AST][76a853bc2762] Unknown unary operator '%s'\n", t2s(token));
                        exit(1);
                    }
                } else {
                    AST_BINOMIAL_EXPRESSION_TYPE operation;
                    if(token->type == TOKEN_PLUS) operation = AST_BINOMIAL_EXPRESSION_TYPE_SUM;
                    if(token->type == TOKEN_DASH) operation = AST_BINOMIAL_EXPRESSION_TYPE_SUBTRACTION;
                    if(token->type == TOKEN_SLASH) operation = AST_BINOMIAL_EXPRESSION_TYPE_DIVISION;
                    if(token->type == TOKEN_STAR) operation = AST_BINOMIAL_EXPRESSION_TYPE_MULTIPLICATION;
                    if(token->type == TOKEN_CARET) operation = AST_BINOMIAL_EXPRESSION_TYPE_POWER;

                    // If we dont have current bi operation we are applying it
                    if(binomialExpressionType == AST_BINOMIAL_EXPRESSION_TYPE_BLANK) {
                        binomialExpressionType = operation;
                    } else {
                        // In opposite case we already have both operands and operation, so we must process bi operations prior (* > +);
                        if(
                            operationPriority(operation) > operationPriority(binomialExpressionType) ||
                            operation == AST_BINOMIAL_EXPRESSION_TYPE_POWER && binomialExpressionType == AST_BINOMIAL_EXPRESSION_TYPE_POWER
                        ) {
                            // If the incoming operation is prioritized or its power operation cascade, then we recursively parse next tokens, using current second operand as first.
                            // 40 + 20 * 4; first = NULL; second = NULL; operation = NONE
                            // >40< + 20 * 4; first = 40; second = NULL; operation = NONE
                            // 40 >+< 20 * 4; first = 40; second = NULL; operation = SUM
                            // 40 + >20< * 4; first = 40; second = 20; operation = SUM
                            // 40 + 20 >*< 4; first = 40; second = 20; operation = SUM; here we got prioritized incoming operation and run the recursion with preset first operand
                            // >*< 4; first = 20; second = NULL; operation = MULT
                            // * >4<; first = 20; second = 4; operation = MULT
                            // then we return the result back to the main operation and skip all parsed tokens
                            // 40 + >20< * 4; first = 40; second = RecResult; operation = SUM
                            // 40 + 20 * 4><; first = 40; second = RecResult; operation = SUM
                            second = parseTokenExpression(current, FALSE, TRUE, &skip, second, limit - length + 1);
                            // and we should set new limit for 'subgroup' that at the end of the expression 
                            // ex: log(10 + 30 * 20);
                            // P.S why do this when power operation cascade:
                            // 2 ^ 2 ^ 3 = 256;
                            // Incoming power operator is more prioritized, we must at first 2^3 and then 2^8
                        } else {
                            // If not then we should just store current operation in "first", clear second and set current operation to the incoming one;
                            first = Creators.createBinomialExpression(binomialExpressionType, first, second);
                            second = NULL;
                            binomialExpressionType = operation;
                        }
                    }
                }
                break;
            default:
                fprintf(stderr, "[ERROR][AST][3e70f617ee39] Unexpected token '%s'\n", t2s(token));
                exit(1);
                break;
        }

        if(breakFlag || !(current = current->next) || (limit > 0 && length == limit)) break;
    }

    if(withLength) *lengthPointer = length;

    if(first == NULL) {
        fprintf(stderr, "[ERROR][AST][bc022b195334] Empty expression\n");
        exit(1);
    }

    if(
        binomialExpressionType == AST_BINOMIAL_EXPRESSION_TYPE_BLANK && 
        unaryExpressionType == AST_UNARY_EXPRESSION_TYPE_BLANK
    ) node = first;
    else if(binomialExpressionType != AST_BINOMIAL_EXPRESSION_TYPE_BLANK){
        if(second == NULL) {
            fprintf(stderr, "[ERROR][AST][1a1eaa382a99] No second operand\n");
            exit(1);
        }
        node = Creators.createBinomialExpression(binomialExpressionType, first, second);
    } else {
        node = Creators.createUnaryExpression(unaryExpressionType, first);
    }

    return node;
}

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
