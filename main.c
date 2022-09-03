#include "main.h"

#include<stdio.h>
#include<stdlib.h>

#include "linkedList/linkedList.h"
#include "utils/utils.h"
#include "token/token.h"
#include "ast/ast.h"
#include "runtime/runtime.h"
#include "loggers/loggers.h"

char splitters[] = {
    CHAR_SPACE,
    CHAR_EQUAL,
    CHAR_SEMICOLON,
    CHAR_NL,
    CHAR_OPEN_BRACKET,
    CHAR_CLOSE_BRACKET,
    CHAR_COMMA,
    CHAR_DASH,
    CHAR_PLUS,
    CHAR_SLASH,
    CHAR_STAR,
    CHAR_CARET,
    CHAR_OPEN_CURLY_BRACKET,
    CHAR_CLOSE_CURLY_BRACKET,
};

int main(int argc, char **argv) {
    if(argc == 1) return 0;

    const char* filePath = argv[1];

    ParseArgvResult options;
    Utils.parseArgv(argc, argv, &options);

    FILE *fp = fopen(filePath, "r");

    if(fp == NULL) {
        printf("Failed to open file %s\n", filePath);
        exit(1);
    }
    
    List *tokens = LinkedList.createList();

    char ch;
    int spanLength = 0;
    char splitter[2];
    while((ch=fgetc(fp)) != EOF) {
        if(Utils.charArrayIncludes(splitters, sizeof(splitters), ch)) {
            if(spanLength > 0) {
                char* textChunk = malloc(sizeof(char) * spanLength + 1);
                fseek(fp, -spanLength-1, SEEK_CUR);

                fread(textChunk, 1, spanLength, fp);
                textChunk[spanLength] = '\0';

                fseek(fp, 1, SEEK_CUR);

                LinkedList.pushItem(tokens, TokenModule.tokenFromString(textChunk));
            }
            if(ch != CHAR_NL & ch != CHAR_SPACE) {
                splitter[0] = ch;
                splitter[1] = '\0';

                LinkedList.pushItem(tokens, TokenModule.tokenFromString(splitter));
            }
            spanLength = 0;
        } else {
           spanLength++;
        }
    }

    fclose(fp);

    if(options.tokens) {
        printf("Tokens: \n");
        Loggers.logTokens(tokens);
        printf("\n\n");
    }

    // check for empty file
    if(tokens->value == NULL) {
        return 0;
    }

    List* ast = AST.createASTFromTokens(tokens);

    if(options.ast) {
        printf("AST: \n");
        Loggers.logAST(ast);
        printf("\n\n");
    }

    if(!options.dry) {
        if(options.ast) {
            printf("Runtime:\n");
        }
        Runtime.run(ast);
    }

    return 0;
}
