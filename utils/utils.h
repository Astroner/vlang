#include "../main.h"

#if !defined(UTILS_H)
#define UTILS_H

typedef struct {
    BOOL (*charArrayIncludes)(char *array, int count, char item);
} UtilsType;

UtilsType Utils;

#endif // UTILS_H
