#include "../main.h"
#include "utils.h"
#include <stdlib.h>

static BOOL charArrayIncludes(char *array, int count, char item) {
    for(int i = 0; i < count; i++) {
        if(array[i] == item) return TRUE;
    }

    return FALSE;
}

UtilsType Utils = { &charArrayIncludes };
