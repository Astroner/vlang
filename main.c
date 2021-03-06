#include<stdio.h>
#include<stdlib.h>

#include "linkedList/linkedList.h"
#include "utils/utils.h"
#include "token/token.h"
#include "main.h"
#include "lexemes/lexemes.h"
#include "ast/ast.h"
#include "runtime/runtime.h"

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
};

void logToken(void* item, int index) {
    Token* token = item;
    printf("%s", TokenModule.TokenType[token->type]);
    if(token->value) {
        if(token->type == TOKEN_NUMBER_LITERAL) {
            int *intNum = token->value;
            printf("; value = %d", *intNum);
        } else {
            printf("; value = '%s'", token->value);
        }
    }
    printf("\n");
}

void logLexeme(void* item, int index) {
    Lexeme* lexeme = item;
    printf("%s\n", Lexemes.LexemesTypes[lexeme->type]);

    ListNode* current = lexeme->tokens;

    while(1) {
        Token* token = current->value;
        printf("    %s\n", TokenModule.TokenType[token->type]);

        if(!current->next) break;
        current = current->next;
    }
}

void logPadding(int padding) {
    for(int i = 0; i < padding; i++) {
        printf("    ");
    }
}

void logASTNode(ASTNode* node, int padding) {
    logPadding(padding);
    printf("%s\n", AST.NodeKind[node->kind]);

    if(node->kind == AST_KIND_VARIABLE_DECLARATION) {
        VariableDeclarationValue* declaration = node->value;

        logPadding(padding + 1);
        printf("Identifier:\n");
        logASTNode(declaration->name, padding + 2);

        switch (declaration->type) {
            case AST_NODE_TYPE_NUMBER:
                logPadding(padding + 1);
                printf("Type: number\n");
                logPadding(padding + 1);
                printf("Value:\n");
                logASTNode(declaration->value, padding + 2);
                break;
            default:
                break;
        }
    }
    if(node->kind == AST_KIND_FUNCTION_CALL) {
        FunctionCallValue* declaration = node->value;
        logPadding(padding + 1);
        printf("Function: '%s'\n", declaration->name->value);
        logPadding(padding + 1);
        printf("Arguments Length: %d\n", declaration->argumentsLength);
        logPadding(padding + 1);
        printf("Arguments:\n");

        ListNode* current = declaration->arguments;
        while(1) {
            ASTNode* node = current->value;

            logASTNode(node, padding + 2);
            
            if(!(current = current->next)) break;
        }
    }
    if(node->kind == AST_KIND_IDENTIFIER) {
        logPadding(padding + 1);
        printf("Name: '%s'\n", node->value);
    }
    if(node->kind == AST_KIND_NUMBER_LITERAL) {
        logPadding(padding + 1);
        printf("Value: %d\n", *((int*)(node->value)));
    }
    if(node->kind == AST_KIND_BINOMIAL_EXPRESSION) {
        BinomialExpressionValue* expression = node->value;
        logPadding(padding + 1);
        printf("Type: %s\n", AST.BinomialExpressionType[expression->type]);
        logPadding(padding + 1);
        printf("First:\n");
        logASTNode(expression->first, padding + 2);
        logPadding(padding + 1);
        printf("Second:\n");
        logASTNode(expression->second, padding + 2);
    }
    if(node->kind == AST_KIND_UNARY_EXPRESSION) {
        UnaryExpressionValue* expression = node->value;
        logPadding(padding + 1);
        printf("Type: %s\n", AST.UnaryExpressionType[expression->type]);
        logPadding(padding + 1);
        printf("Member:\n");
        logASTNode(expression->member, padding + 2);
    }
}

void ASTListLogger(void* item, int index) {
    ASTNode* node = item;
    logASTNode(node, 0);
}

int main(int argc, char const **argv) {
    if(argc == 1) return 0;

    const char* filePath = argv[1];

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
            splitter[0] = ch;
            splitter[1] = '\0';

            LinkedList.pushItem(tokens, TokenModule.tokenFromString(splitter));

            spanLength = 0;
        } else {
           spanLength++;
        }
    }

    fclose(fp);

    // printf("Tokens: \n");
    // LinkedList.forEach(tokens, logToken);
    // printf("\n\n");

    List* lexemes = Lexemes.getLexemes(tokens);
    LinkedList.clear(tokens);

    // printf("Lexemes: \n");
    // LinkedList.forEach(lexemes, logLexeme);
    // printf("\n\n");

    List* ast = AST.createStatements(lexemes);

    // printf("AST: \n");
    // LinkedList.forEach(ast, ASTListLogger);

    Runtime.run(ast);

    return 0;
}
