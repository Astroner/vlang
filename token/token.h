#if !defined(TOKENS_H)
#define TOKENS_H

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

#define NUMBER_KEYWORD "number"
#define RETURN_KEYWORD "return"
#define BOOLEAN_KEYWORD "boolean"
#define BOOLEAN_TRUE "true"
#define BOOLEAN_FALSE "false"

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
