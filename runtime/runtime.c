#include "runtime.h"

#include <stdlib.h>

#include "../HashTable/HashTable.h"
#include "../ast/ast.h"
#include "native/native.h"
#include "runners/runNode.h"

static void run(List* statements) {
    RuntimeContext ctx = {
        Scopes.createScope(NULL),
        HashTable.create(),
        NULL,
        HashTable.create(),
    };

    ListNode* current = statements;
    while(1) {
        ASTNode* statement = current->value;
        
        runNode(statement, &ctx);

        if(!(current = current->next)) break;
    }
}

RuntimeType Runtime = { run };