#include "lexemes.h"
#include <stdio.h>
#include <stdlib.h>
#include "../main.h"
#include "../utils/utils.h"

static LEXEME_TYPE notValuableTokens[] = {
    TOKEN_NL,
    TOKEN_SPACE
};

static BOOL isValuableToken(TOKEN_TYPE type) {
    return !Utils.charArrayIncludes(notValuableTokens, sizeof(notValuableTokens), type);
}

static char* LexemesTypes[] = {
    "LEXEME_BLANK",
    "LEXEME_VARIABLE_DEFINITION",
    "LEXEME_FUNCTION_CALL",
    "LEXEME_FUNCTION_DEFINITION",
};

static List* getLexemes(List* tokens) {
    List* lexemes = LinkedList.createList();

    LEXEME_TYPE ctx = LEXEME_BLANK;
    List* lexemeTokens = LinkedList.createList();

    ListNode* current = tokens;
    while(1) {     
        Token* prevLexemeToken = lexemeTokens->value;
        Token* token = current->value;
        if(isValuableToken(token->type)) {
            if(ctx != LEXEME_BLANK) {
                if(token->type == TOKEN_NUMBER_KEYWORD && ctx != LEXEME_FUNCTION_DEFINITION) {
                    fprintf(stderr, "[ERROR][LEXER][52e735541a6c] Unexpected usage of 'number' keyword");
                    exit(1);
                } else if(
                    ctx == LEXEME_VARIABLE_DEFINITION && 
                    token->type == TOKEN_OPEN_BRACKET &&
                    prevLexemeToken != NULL &&
                    prevLexemeToken->type == TOKEN_IDENTIFIER
                ) {
                    ctx = LEXEME_FUNCTION_DEFINITION;
                    LinkedList.pushItem(lexemeTokens, token);
                } else if(token->type == TOKEN_SEMICOLON){
                    Lexeme* newLexeme = malloc(sizeof(Lexeme));
                    newLexeme->type = ctx;
                    newLexeme->tokens = lexemeTokens;

                    ctx = LEXEME_BLANK;
                    lexemeTokens = LinkedList.createList();

                    LinkedList.pushItem(lexemes, newLexeme);
                } else {
                    LinkedList.pushItem(lexemeTokens, token);
                }
            } else {
                if(token->type == TOKEN_NUMBER_KEYWORD) {
                    ctx = LEXEME_VARIABLE_DEFINITION;
                    LinkedList.pushItem(lexemeTokens, token);
                } else if(token->type == TOKEN_IDENTIFIER) {
                    ctx = LEXEME_FUNCTION_CALL;
                    LinkedList.pushItem(lexemeTokens, token);
                }
            }
        }

        if(!current->next) break;
        current = current->next;
    };

    return lexemes;
}

LexemesType Lexemes = { getLexemes, LexemesTypes };