#include "../main.h"

#if !defined(UTILS_H)
#define UTILS_H

typedef struct {
    BOOL ast;
    BOOL tokens;
    BOOL dry;
} ParseArgvResult;

typedef struct {
    BOOL (*charArrayIncludes)(char *array, int count, char item);
    void (*parseArgv)(int argc, char** argv, ParseArgvResult* options);
} UtilsType;

UtilsType Utils;

#endif // UTILS_H
