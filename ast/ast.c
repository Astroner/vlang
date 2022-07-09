#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../token/token.h"
#include "dicts.c"

static ASTNode* createIdentifier(char* name) {
    ASTNode* identifier = malloc(sizeof(ASTNode));

    identifier->kind = AST_KIND_IDENTIFIER;
    identifier->value = name;

    return identifier;
}

static ASTNode* createNumberLiteral(int* value) {
    ASTNode* identifier = malloc(sizeof(ASTNode));

    identifier->kind = AST_KIND_NUMBER_LITERAL;
    identifier->value = value;

    return identifier;
}

static ASTNode* parseVariableDefinitionLexeme(List* tokens){
    ASTNode* node = malloc(sizeof(ASTNode));
    node->kind = AST_KIND_VARIABLE_DECLARATION;

    ASTNode* identifier = malloc(sizeof(ASTNode));
    identifier->kind = AST_KIND_IDENTIFIER;

    VariableDeclarationValue* value = malloc(sizeof(VariableDeclarationValue));
    value->name = identifier;

    node->value = value;

    Token* typeToken = tokens->value;
    AST_NODE_TYPE type = AST_NODE_TYPE_BLANK;

    switch (typeToken->type) {
    case TOKEN_NUMBER_KEYWORD:
        type = AST_NODE_TYPE_NUMBER;
        break;
    
    default:
        fprintf(stderr, "Unexpected variable type '%s'", TokenModule.TokenType[typeToken->type]);
        exit(1);
        break;
    }
    value->type = type;

    ListNode* nameTokenNode = tokens->next;
    Token* nameToken;

    if(
        !nameTokenNode || 
        (nameToken = ((Token*)(nameTokenNode->value)))->type != TOKEN_IDENTIFIER
    ) {
        fprintf(stderr, "No identifier provided");
        exit(1);
    }

    identifier->value = nameToken->value;

    ListNode* equalTokenNode = nameTokenNode->next;

    if(
        !equalTokenNode ||
        (((Token*)(equalTokenNode->value)))->type != TOKEN_EQUAL
    ) {
        fprintf(
            stderr, 
            "Unexpected token '%s'", 
            TokenModule.TokenType[(((Token*)(equalTokenNode->value)))->type]
        );
        exit(1);
    }

    assert(equalTokenNode->next);

    Token* valueBeginningToken = equalTokenNode->next->value;

    if(valueBeginningToken->type == TOKEN_NUMBER_LITERAL) {
        value->value = valueBeginningToken->value;
    } else {
        fprintf(stderr, "Unexpected token '%s'\n", TokenModule.TokenType[valueBeginningToken->type]);
        exit(1);
    }

    return node;
}

static ASTNode* parseArgument(List* tokens, int limit) {
    ASTNode* node;

    ListNode* current = tokens;
    int counter = 0;

    if(limit == 1) {
        Token* token = tokens->value;
        switch (token->type) {
        case TOKEN_NUMBER_LITERAL:
            node = createNumberLiteral(token->value);
            break;
        case TOKEN_IDENTIFIER:
            node = createIdentifier(token->value);
            break;
        default:
            fprintf(stderr, "Unexpected token as argument: '%s'\n", TokenModule.TokenType[token->type]);
            exit(1);
            break;
        }
    } else {
        fprintf(stderr, "Unexpected behavior: more then 1 token as argument\n");
        exit(1);
    }
 
    return node;
}

static ASTNode* parseFunctionCallLexeme(List* tokens){
    ASTNode* node = malloc(sizeof(ASTNode));
    FunctionCallValue* value = malloc(sizeof(FunctionCallValue));
    
    node->kind = AST_KIND_FUNCTION_CALL;
    node->value = value;

    Token* nameToken = tokens->value;

    ASTNode* identifier = malloc(sizeof(ASTNode));
    identifier->kind = AST_KIND_IDENTIFIER;
    identifier->value = nameToken->value;
    value->name = identifier;

    ListNode* openToken = tokens->next;
    
    assert(openToken && ((Token*)(openToken->value))->type == TOKEN_OPEN_BRACKET);

    int argLength = 0;
    List* arguments = LinkedList.createList();

    ListNode* current = openToken->next;
    ListNode* argStart = current;
    int argLimit = 0;
    while(1) {
        Token* token = current->value;
        if(token->type == TOKEN_CLOSE_BRACKET) {
            LinkedList.pushItem(arguments, parseArgument(argStart, argLimit));
            argLength++;
            break;
        }

        if(token->type == TOKEN_COMMA) {
            LinkedList.pushItem(arguments, parseArgument(argStart, argLimit));
            if(!current->next) {
                fprintf(stderr, "Expected another argument after ',' token\n");
                exit(1);
            }
            argStart = current->next;
            argLimit = 0;
            argLength++;
        } else {
            argLimit++;
        }

        if(!(current = current->next)) {
            fprintf(stderr, "Unexpected end of arguments\n");
            exit(1);
        }
    }

    value->argLength = argLength;
    value->arguments = arguments;

    return node;
}

static List* createStatements(List* lexemes) {
    List* result = LinkedList.createList();

    ListNode* current = lexemes;

    while(1){
        Lexeme* lexeme = current->value;
        switch (lexeme->type) {
        case LEXEME_VARIABLE_DEFINITION:
            LinkedList.pushItem(result, parseVariableDefinitionLexeme(lexeme->tokens));
            break;
        case LEXEME_FUNCTION_CALL:
            LinkedList.pushItem(result, parseFunctionCallLexeme(lexeme->tokens));
            break;
        default:
            break;
        }

        if(!(current = current->next)) break;
    }

    return result;
}

ASTModuleType AST = { createStatements, NodeKind, NodeType };