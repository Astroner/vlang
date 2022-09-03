#if !defined(LOGGERS_H)
#define LOGGERS_H

#include "../linkedList/linkedList.h"

typedef struct {
    void (*logAST)(List* ast);
    void (*logTokens)(List* tokens);
} LoggersModule;

LoggersModule Loggers;

#endif // LOGGERS_H
