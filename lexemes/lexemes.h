#include "../token/token.h"
#include "../linkedList/linkedList.h"

#if !defined(LEXEMES_H)
#define LEXEMES_H

#define LEXEME_TYPE char
#define LEXEME_BLANK 0
#define LEXEME_VARIABLE_DEFINITION 1
#define LEXEME_FUNCTION_CALL 2

typedef struct {
    LEXEME_TYPE type;
    List* tokens;
} Lexeme;

typedef struct {
    List* (*getLexemes)(List* tokens);
    char** LexemesTypes;
} LexemesType;

LexemesType Lexemes;

#endif // LEXEMES_H
