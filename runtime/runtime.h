#include "../linkedList/linkedList.h"

#if !defined(RUNTIME)
#define RUNTIME

#define RUNTIME_DATA_TYPE_BINDING_NUMBER int

typedef struct {
    void (*run)(List* statements);
} RuntimeType;

RuntimeType Runtime;

#endif // RUNTIME
