#include "parseExpression.h"

#include<stdio.h>
#include<stdlib.h>

#include "../../token/token.h"
#include "../creators/creators.h"
#include "parsers.h"

#define operationPriority(operation) binomialOperationPriorities[operation]

typedef unsigned char Priority;

static Priority binomialOperationPriorities[] = {
    /* AST_BINOMIAL_EXPRESSION_TYPE_BLANK */ 0,
    /* AST_BINOMIAL_EXPRESSION_TYPE_OR */ 1,
    /* AST_BINOMIAL_EXPRESSION_TYPE_AND */ 1,
    /* AST_BINOMIAL_EXPRESSION_TYPE_EQUAL */ 2,
    /* AST_BINOMIAL_EXPRESSION_TYPE_GREATER */ 2,
    /* AST_BINOMIAL_EXPRESSION_TYPE_GREATER_OR_EQUAL */ 2,
    /* AST_BINOMIAL_EXPRESSION_TYPE_LESS */ 2,
    /* AST_BINOMIAL_EXPRESSION_TYPE_LESS_OR_EQUAL */ 2,
    /* AST_BINOMIAL_EXPRESSION_TYPE_NOT_EQUAL */ 2,
    /* AST_BINOMIAL_EXPRESSION_TYPE_SUM */ 3,
    /* AST_BINOMIAL_EXPRESSION_TYPE_SUBTRACTION */ 3,
    /* AST_BINOMIAL_EXPRESSION_TYPE_MULTIPLICATION */ 4,
    /* AST_BINOMIAL_EXPRESSION_TYPE_DIVISION */ 4,
    /* AST_BINOMIAL_EXPRESSION_TYPE_POWER */ 5,
};

static ASTNode* parseTokenExpression(
    List* tokens,
    unsigned int contentLength,
    ASTNode* __first
) {
    ASTNode* first = __first;
    ASTNode* second = NULL;

    AST_BINOMIAL_EXPRESSION_TYPE binomialExpressionType = AST_BINOMIAL_EXPRESSION_TYPE_BLANK;
    AST_UNARY_EXPRESSION_TYPE unaryExpressionType = AST_UNARY_EXPRESSION_TYPE_BLANK;

    BracketsRange bracketsRange;
    ParserResult parserResult;
    BOOL exitLoop = FALSE;
    ListNode* current = tokens;
    unsigned int length = 0;
    while(1) {
        Token* token = current->value;
        
        switch(token->type) {
            case TOKEN_IDENTIFIER:
            case TOKEN_BOOLEAN_LITERAL:
            case TOKEN_NUMBER_LITERAL: {
                ASTNode* subject;
                if(token->type == TOKEN_IDENTIFIER) {
                    if(current->next && ((Token*)current->next->value)->type == TOKEN_OPEN_BRACKET) {
                        Parsers.parseFunctionCall(
                            current,
                            0,
                            FALSE,
                            &parserResult
                        );
                        subject = parserResult.node;
                        current = parserResult.lastNode;
                        length += parserResult.length - 2;
                    } else {
                        subject = Creators.createIdentifier(token->value);
                    }
                } else if(token->type == TOKEN_NUMBER_LITERAL) {
                    subject = Creators.createNumberLiteral(token->value);
                } else {
                    subject = Creators.createBooleanLiteral(token->value);
                }
                if(unaryExpressionType != AST_UNARY_EXPRESSION_TYPE_BLANK) {
                    subject = Creators.createUnaryExpression(
                        unaryExpressionType,
                        subject
                    );
                    unaryExpressionType = AST_UNARY_EXPRESSION_TYPE_BLANK;
                }
                if(first == NULL) {
                    first = subject;
                } else if (binomialExpressionType != AST_BINOMIAL_EXPRESSION_TYPE_BLANK && second == NULL) {
                    second = subject;
                } else {
                    fprintf(stderr, "[ERROR][AST][d26dc49e3a0a] Unexpected number literal '%d'\n", *((int*)token->value));
                    exit(1);
                }
                break;
            }
            case TOKEN_EXCLAMATION:

            case TOKEN_DOUBLE_EQUAL:
            case TOKEN_NOT_EQUAL:

            case TOKEN_GREATER:
            case TOKEN_GREATER_OR_EQUAL:
            case TOKEN_LESS:
            case TOKEN_LESS_OR_EQUAL:

            case TOKEN_DOUBLE_PIPE:
            case TOKEN_DOUBLE_AMPERSAND:

            case TOKEN_STAR:
            case TOKEN_SLASH:
            case TOKEN_DASH:
            case TOKEN_CARET:
            case TOKEN_PLUS: {
                AST_UNARY_EXPRESSION_TYPE unaryOperation = AST_UNARY_EXPRESSION_TYPE_BLANK;
                if(token->type == TOKEN_DASH) unaryOperation = AST_UNARY_EXPRESSION_TYPE_NEGATIVE;
                if(token->type == TOKEN_EXCLAMATION) unaryOperation = AST_UNARY_EXPRESSION_TYPE_NOR;

                AST_BINOMIAL_EXPRESSION_TYPE binomialOperation = AST_BINOMIAL_EXPRESSION_TYPE_BLANK;
                if(token->type == TOKEN_PLUS) binomialOperation = AST_BINOMIAL_EXPRESSION_TYPE_SUM;
                if(token->type == TOKEN_DASH) binomialOperation = AST_BINOMIAL_EXPRESSION_TYPE_SUBTRACTION;
                if(token->type == TOKEN_STAR) binomialOperation = AST_BINOMIAL_EXPRESSION_TYPE_MULTIPLICATION;
                if(token->type == TOKEN_SLASH) binomialOperation = AST_BINOMIAL_EXPRESSION_TYPE_DIVISION;
                if(token->type == TOKEN_CARET) binomialOperation = AST_BINOMIAL_EXPRESSION_TYPE_POWER;

                if(token->type == TOKEN_DOUBLE_EQUAL) binomialOperation = AST_BINOMIAL_EXPRESSION_TYPE_EQUAL;
                if(token->type == TOKEN_NOT_EQUAL) binomialOperation = AST_BINOMIAL_EXPRESSION_TYPE_NOT_EQUAL;

                if(token->type == TOKEN_GREATER) binomialOperation = AST_BINOMIAL_EXPRESSION_TYPE_GREATER;
                if(token->type == TOKEN_GREATER_OR_EQUAL) binomialOperation = AST_BINOMIAL_EXPRESSION_TYPE_GREATER_OR_EQUAL;
                if(token->type == TOKEN_LESS) binomialOperation = AST_BINOMIAL_EXPRESSION_TYPE_LESS;
                if(token->type == TOKEN_LESS_OR_EQUAL) binomialOperation = AST_BINOMIAL_EXPRESSION_TYPE_LESS_OR_EQUAL;

                if(token->type == TOKEN_DOUBLE_PIPE) binomialOperation = AST_BINOMIAL_EXPRESSION_TYPE_OR;
                if(token->type == TOKEN_DOUBLE_AMPERSAND) binomialOperation = AST_BINOMIAL_EXPRESSION_TYPE_AND;


                // handling unary expressions
                if(
                    unaryExpressionType == AST_UNARY_EXPRESSION_TYPE_BLANK
                    && unaryOperation != AST_UNARY_EXPRESSION_TYPE_BLANK
                    && (
                        first == NULL 
                        || (
                            binomialExpressionType != AST_BINOMIAL_EXPRESSION_TYPE_BLANK
                            && second == NULL
                        )
                    )
                ) {
                    unaryExpressionType = unaryOperation;
                } else if(first != NULL && binomialExpressionType == AST_BINOMIAL_EXPRESSION_TYPE_BLANK){
                    binomialExpressionType = binomialOperation;
                } else if (second != NULL) {
                    if(
                        operationPriority(binomialOperation) > operationPriority(binomialExpressionType) 
                        || (
                            binomialOperation == AST_BINOMIAL_EXPRESSION_TYPE_POWER 
                            && binomialExpressionType == AST_BINOMIAL_EXPRESSION_TYPE_POWER
                        )
                    ) {
                        second = parseTokenExpression(
                            current,
                            contentLength - length,
                            second
                        );
                        exitLoop = TRUE;
                    } else {
                        first = Creators.createBinomialExpression(
                            binomialExpressionType,
                            first,
                            second
                        );
                        binomialExpressionType = binomialOperation;
                        second = NULL;
                    }
                } else {
                    fprintf(stderr, "[ERROR][AST][feffd65dc6b4] Unexpected token '%s'\n", t2s(token));
                    exit(1);
                }
                break;
            }
            case TOKEN_OPEN_BRACKET: {
                if(
                    (second != NULL && first != NULL)
                    || (first != NULL && binomialExpressionType == AST_BINOMIAL_EXPRESSION_TYPE_BLANK)
                ) {
                    fprintf(stderr, "[ERROR][AST][727ef76ae065] Unexpected open bracket\n");
                    exit(1);
                }
                Parsers.parseBracketsRange(
                    current,
                    contentLength - length,
                    &bracketsRange
                );

                if(first == NULL) {
                    first = parseTokenExpression(current->next, bracketsRange.length - 2, NULL);
                } else {
                    second = parseTokenExpression(current->next, bracketsRange.length - 2, NULL);
                }
                
                current = bracketsRange.closeBracket;
                length += bracketsRange.length - 1;
                break;
            }
            default: {
                fprintf(stderr, "[ERROR][AST][31a81bfc0636] Unexpected token '%s'\n", t2s(token));
                exit(1);
            }
        }

        length++;
        if(!(current = current->next) || (contentLength > 0 && contentLength == length) || exitLoop) {
            break;
        }
    }

    if(first == NULL && second == NULL) {
        fprintf(stderr, "[ERROR][AST][a4858677962c] Expected input in the expression\n");
        exit(1);
    }
    if(second) 
        return Creators.createBinomialExpression(binomialExpressionType, first, second);

    if(first != NULL && unaryExpressionType != AST_UNARY_EXPRESSION_TYPE_BLANK)
        return Creators.createUnaryExpression(unaryExpressionType, first);

    if(first != NULL)
        return first;

    fprintf(stderr, "[ERROR][AST][5c01d6bc0697] Expected end of the expression\n");
    exit(1);
}

ASTNode* parseExpression(
    List* tokens,
    unsigned int contentLength
) {
    return parseTokenExpression(tokens, contentLength, NULL);
}