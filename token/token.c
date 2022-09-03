#include "token.h"
#include <stdlib.h>
#include <stdio.h>
#include<string.h>

#include "../main.h"

static char* TokenType[] = {
    "TOKEN_IDENTIFIER",
    "TOKEN_EQUAL",
    "TOKEN_SEMICOLON",
    "TOKEN_OPEN_BRACKET",
    "TOKEN_CLOSE_BRACKET",
    "TOKEN_NUMBER_KEYWORD",
    "TOKEN_NUMBER_LITERAL",
    "TOKEN_COMMA",
    "TOKEN_STAR",
    "TOKEN_DASH",
    "TOKEN_SLASH",
    "TOKEN_PLUS",
    "TOKEN_CARET",
    "TOKEN_OPEN_CURLY_BRACKET",
    "TOKEN_CLOSE_CURLY_BRACKET",
    "TOKEN_RETURN_KEYWORD",
    "TOKEN_BOOLEAN_KEYWORD",
    "TOKEN_BOOLEAN_LITERAL",
    "TOKEN_IF_KEYWORD",
    "TOKEN_ELSE_KEYWORD",
    "TOKEN_PIPE",
    "TOKEN_AMPERSAND",
    "TOKEN_DOUBLE_PIPE",
    "TOKEN_DOUBLE_AMPERSAND",
    "TOKEN_EXCLAMATION",
    "TOKEN_GREATER",
    "TOKEN_LESS",
    "TOKEN_NOT_EQUAL",
    "TOKEN_GREATER_OR_EQUAL",
    "TOKEN_LESS_OR_EQUAL",
    "TOKEN_DOUBLE_EQUAL",
};

static Token* createToken(TOKEN_TYPE type, void* value) {
    Token* token = malloc(
        value ? sizeof(Token) : 1
    );

    token->type = type;
    token->value = value;

    return token;
}

Token* parseToken(char* str, Token* prevToken) {

    Token *token = NULL;
    int length = strlen(str);
    if(length == 1) {
        char ch = str[0];

        if(ch >= CHAR_NUMBER_0 && ch <= CHAR_NUMBER_9) {
            int* intNum = malloc(sizeof(int));
            *intNum = ch - CHAR_NUMBER_0;

            return createToken(TOKEN_NUMBER_LITERAL, intNum);
        }

        switch (ch) {
            case CHAR_EQUAL:
                if(prevToken != NULL) {
                    if(prevToken->type == TOKEN_GREATER){
                        prevToken->type = TOKEN_GREATER_OR_EQUAL;
                    } 
                    else if(prevToken->type == TOKEN_LESS) {
                        prevToken->type = TOKEN_LESS_OR_EQUAL;
                    } 
                    else if(prevToken->type == TOKEN_EXCLAMATION) {
                        prevToken->type = TOKEN_NOT_EQUAL;
                    }
                    else if(prevToken->type == TOKEN_EQUAL) {
                        prevToken->type = TOKEN_DOUBLE_EQUAL;
                    } 
                    else {
                        token = createToken(TOKEN_EQUAL, 0);
                    }
                } else {
                    token = createToken(TOKEN_EQUAL, 0);
                }
                break;
            case CHAR_SEMICOLON:
                token = createToken(TOKEN_SEMICOLON, 0);
                break;
            case CHAR_OPEN_BRACKET:
                token = createToken(TOKEN_OPEN_BRACKET, 0);
                break;
            case CHAR_CLOSE_BRACKET:
                token = createToken(TOKEN_CLOSE_BRACKET, 0);
                break;
            case CHAR_COMMA:
                token = createToken(TOKEN_COMMA, 0);
                break;
            case CHAR_STAR:
                token = createToken(TOKEN_STAR, 0);
                break;
            case CHAR_DASH:
                token = createToken(TOKEN_DASH, 0);
                break;
            case CHAR_PLUS:
                token = createToken(TOKEN_PLUS, 0);
                break;
            case CHAR_SLASH:
                token = createToken(TOKEN_SLASH, 0);
                break;
            case CHAR_CARET:
                token = createToken(TOKEN_CARET, 0);
                break;
            case CHAR_OPEN_CURLY_BRACKET:
                token = createToken(TOKEN_OPEN_CURLY_BRACKET, 0);
                break;
            case CHAR_CLOSE_CURLY_BRACKET:
                token = createToken(TOKEN_CLOSE_CURLY_BRACKET, 0);
                break;
            case CHAR_PIPE:
                if(prevToken && prevToken->type == TOKEN_PIPE) {
                    prevToken->type = TOKEN_DOUBLE_PIPE;
                } else {
                    token = createToken(TOKEN_PIPE, 0);
                }
                break;
            case CHAR_AMPERSAND:
                if(prevToken && prevToken->type == TOKEN_AMPERSAND) {
                    prevToken->type = TOKEN_DOUBLE_AMPERSAND;
                } else {
                    token = createToken(TOKEN_AMPERSAND, 0);
                }
                break;
            case CHAR_GREATER:
                token = createToken(TOKEN_GREATER, 0);
                break;
            case CHAR_LESS:
                token = createToken(TOKEN_LESS, 0);
                break;
            case CHAR_EXCLAMATION:
                token = createToken(TOKEN_EXCLAMATION, 0);
                break;
            default:
                token = createToken(TOKEN_IDENTIFIER, str);
                break;
        }
    } else {
        int tryInt = atoi(str);
        if(strcmp(str, NUMBER_KEYWORD) == 0) {
            token = createToken(TOKEN_NUMBER_KEYWORD, NULL);
        } 
        else if(strcmp(str, RETURN_KEYWORD) == 0) {
            token = createToken(TOKEN_RETURN_KEYWORD, NULL);
        } 
        else if(strcmp(str, BOOLEAN_KEYWORD) == 0) {
            token = createToken(TOKEN_BOOLEAN_KEYWORD, NULL);
        } 
        else if(strcmp(str, BOOLEAN_TRUE) == 0) {
            int* valueTrue = malloc(sizeof(int));
            *valueTrue = 1;
            token = createToken(TOKEN_BOOLEAN_LITERAL, valueTrue);
        } 
        else if(strcmp(str, BOOLEAN_FALSE) == 0) {
            int* valueFalse = malloc(sizeof(int));
            *valueFalse = 0;
            token = createToken(TOKEN_BOOLEAN_LITERAL, valueFalse);
        } 
        else if(strcmp(str, IF_KEYWORD) == 0) {
            token = createToken(TOKEN_IF_KEYWORD, NULL);
        } 
        else if(strcmp(str, ELSE_KEYWORD) == 0) {
            token = createToken(TOKEN_ELSE_KEYWORD, NULL);
        } 
        else if(tryInt) {
            int* entity = malloc(sizeof(int));
            *entity = tryInt;
            token = createToken(TOKEN_NUMBER_LITERAL, entity);
        } 
        else {
            token = createToken(TOKEN_IDENTIFIER, str);
        }
    }

    return token;
}

TokenModuleType TokenModule = {
    TokenType,
    parseToken
};