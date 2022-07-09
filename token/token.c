#include "token.h"
#include <stdlib.h>
#include <stdio.h>
#include<string.h>

#include "../main.h"

static char* TokenType[] = {
    "TOKEN_IDENTIFIER",
    "TOKEN_EQUAL",
    "TOKEN_SPACE",
    "TOKEN_SEMICOLON",
    "TOKEN_NL",
    "TOKEN_OPEN_BRACKET",
    "TOKEN_CLOSE_BRACKET",
    "TOKEN_NUMBER_KEYWORD",
    "TOKEN_NUMBER_LITERAL",
    "TOKEN_COMMA"
};

static Token* createToken(TOKEN_TYPE type, void* value) {
    Token* token = malloc(
        value ? sizeof(Token) : 1
    );

    token->type = type;
    token->value = value;

    return token;
}

Token* tokenFromString(char* str) {
    Token *token;
    int length = strlen(str);
    if(length == 1) {
        char ch = str[0];

        if(ch >= CHAR_NUMBER_0 && ch <= CHAR_NUMBER_9) {
            int* intNum = malloc(sizeof(int));
            *intNum = ch - CHAR_NUMBER_0;
            return createToken(TOKEN_NUMBER_LITERAL, intNum);
        }

        switch (ch) {
            case CHAR_SPACE:
                token = createToken(TOKEN_SPACE, 0);
                break;
            case CHAR_EQUAL:
                token = createToken(TOKEN_EQUAL, 0);
                break;
            case CHAR_NL:
                token = createToken(TOKEN_NL, 0);
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
            default:
                token = createToken(TOKEN_IDENTIFIER, str);
                break;
        }
    } else {
        int tryInt = atoi(str);

        if(strcmp(str, NUMBER_KEYWORD) == 0) {
            token = createToken(TOKEN_NUMBER_KEYWORD, 0);
        } else if(tryInt) {
            int* entity = malloc(sizeof(int));
            *entity = tryInt;
            token = createToken(TOKEN_NUMBER_LITERAL, entity);
        } else {
            token = createToken(TOKEN_IDENTIFIER, str);
        }
    }

    return token;
}

TokenModuleType TokenModule = {
    TokenType,
    tokenFromString
};