#include "utils.h"

#include <stdlib.h>
#include <string.h>

#include "../main.h"

static BOOL charArrayIncludes(char *array, int count, char item) {
    for(int i = 0; i < count; i++) {
        if(array[i] == item) return TRUE;
    }

    return FALSE;
}

#define CLI_FLAG_AST "--ast"
#define CLI_FLAG_TOKENS "--tokens"
#define CLI_FLAG_DRY "--dry"

static void parseArgv(int argc, char** argv, ParseArgvResult* options) {
    options->ast = FALSE;
    options->tokens = FALSE;
    options->dry = FALSE;

    for(int i = 0; i < argc; i++) {
        if(strcmp(CLI_FLAG_AST, argv[i]) == 0) {
            options->ast = TRUE;
        } else if(strcmp(CLI_FLAG_TOKENS, argv[i]) == 0){
            options->tokens = TRUE;
        } else if(strcmp(CLI_FLAG_DRY, argv[i]) == 0) {
            options->dry = TRUE;
        }
    }
}

UtilsType Utils = { charArrayIncludes, parseArgv };
