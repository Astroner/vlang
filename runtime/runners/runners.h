#if !defined(RUNNERS_H)
#define RUNNERS_H

#include "../runtime.h"

typedef struct {
    void (*runVariableDeclaration)(VariableDeclarationValue* variable, RuntimeContext* ctx);
    Declaration* (*runFunctionCall)(FunctionCallValue* call, RuntimeContext* ctx);
    Declaration* (*runUnaryExpression)(UnaryExpressionValue* expression, RuntimeContext* ctx);
    Declaration* (*runBinomialExpression)(BinomialExpressionValue* expression, RuntimeContext* ctx);
    void (*runFunctionDefinition)(FunctionDefinitionValue* definition, RuntimeContext* ctx);
    void (*runVariableAssignment)(VariableAssignmentValue* assignment, RuntimeContext* ctx);
    void (*runIfStatement)(List* conditions, RuntimeContext* ctx);
    void (*runIfCondition)(IfConditionValue* condition, RuntimeContext* ctx);
    void (*runElseStatement)(List* statements, RuntimeContext* ctx);
} RunnersModule;

RunnersModule Runners;

#endif // RUNNERS_H
