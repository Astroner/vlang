#if !defined(RUNTIME_NATIVE_H)
#define RUNTIME_NATIVE_H

#include "../../ast/ast.h"
#include "../runtime.h"

typedef struct {
    void (*log)(FunctionCallValue* call, RuntimeContext* ctx);
    int (*randomNumber)(FunctionCallValue* call, RuntimeContext* ctx);
} NativeModule;

NativeModule Native;

#endif // RUNTIME_NATIVE_H
