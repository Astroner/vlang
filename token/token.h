#if !defined(TOKENS_H)
#define TOKENS_H

#include "../linkedList/linkedList.h"
#include "../main.h"

#define TOKEN_TYPE char
#define TOKEN_BLANK -1
#define TOKEN_IDENTIFIER 0
#define TOKEN_EQUAL 1
#define TOKEN_SEMICOLON 2
#define TOKEN_OPEN_BRACKET 3
#define TOKEN_CLOSE_BRACKET 4
#define TOKEN_NUMBER_KEYWORD 5
#define TOKEN_NUMBER_LITERAL 6
#define TOKEN_COMMA 7
#define TOKEN_STAR 8
#define TOKEN_DASH 9
#define TOKEN_SLASH 10
#define TOKEN_PLUS 11
#define TOKEN_CARET 12
#define TOKEN_OPEN_CURLY_BRACKET 13
#define TOKEN_CLOSE_CURLY_BRACKET 14
#define TOKEN_RETURN_KEYWORD 15
#define TOKEN_BOOLEAN_KEYWORD 16
#define TOKEN_BOOLEAN_LITERAL 17
#define TOKEN_IF_KEYWORD 18
#define TOKEN_ELSE_KEYWORD 19
#define TOKEN_PIPE 20
#define TOKEN_AMPERSAND 21
#define TOKEN_DOUBLE_PIPE 22
#define TOKEN_DOUBLE_AMPERSAND 23
#define TOKEN_EXCLAMATION 24
#define TOKEN_GREATER 25
#define TOKEN_LESS 26
#define TOKEN_NOT_EQUAL 27
#define TOKEN_GREATER_OR_EQUAL 28
#define TOKEN_LESS_OR_EQUAL 29
#define TOKEN_DOUBLE_EQUAL 30
#define TOKEN_PURE_KEYWORD 31
#define TOKEN_MEMOIZED_KEYWORD 32

#define NUMBER_KEYWORD "number"
#define RETURN_KEYWORD "return"
#define BOOLEAN_KEYWORD "boolean"
#define BOOLEAN_TRUE "true"
#define BOOLEAN_FALSE "false"
#define IF_KEYWORD "if"
#define ELSE_KEYWORD "else"
#define PURE_KEYWORD "pure"
#define MEMOIZED_KEYWORD "memoized"

// returns stringified token type from dict
#define t2s(token) TokenModule.TokenType[token->type]

typedef struct {
    TOKEN_TYPE type;
    void* value;
} Token;

typedef struct {
    char** TokenType;
    Token* (*parseToken)(char* str, Token* prevToken, BOOL saveStringInitVal);
} TokenModuleType;

TokenModuleType TokenModule;

#endif // TOKENS_H
