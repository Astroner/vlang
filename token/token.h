#if !defined(TOKENS_H)
#define TOKENS_H

#define TOKEN_TYPE char
#define TOKEN_BLANK -1
#define TOKEN_IDENTIFIER 0
#define TOKEN_EQUAL 1
#define TOKEN_SPACE 2
#define TOKEN_SEMICOLON 3
#define TOKEN_NL 4
#define TOKEN_OPEN_BRACKET 5
#define TOKEN_CLOSE_BRACKET 6
#define TOKEN_NUMBER_KEYWORD 7
#define TOKEN_NUMBER_LITERAL 8
#define TOKEN_COMMA 9
#define TOKEN_STAR 10
#define TOKEN_DASH 11
#define TOKEN_SLASH 12
#define TOKEN_PLUS 13
#define TOKEN_CARET 14

#define NUMBER_KEYWORD "number"
// returns stringified token type from dict
#define t2s(token) TokenModule.TokenType[token->type]

typedef struct {
    TOKEN_TYPE type;
    void* value;
} Token;

typedef struct {
    char** TokenType;
    Token* (*tokenFromString)(char* str);
} TokenModuleType;

TokenModuleType TokenModule;

#endif // TOKENS_H
