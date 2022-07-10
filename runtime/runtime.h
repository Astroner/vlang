#include "../linkedList/linkedList.h"

#if !defined(RUNTIME)
#define RUNTIME

typedef struct {
    void (*run)(List* statements);
} RuntimeType;

RuntimeType Runtime;

#endif // RUNTIME
