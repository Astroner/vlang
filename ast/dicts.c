static char* NodeKind[] = {
    "AST_KIND_BLANK",
    "AST_KIND_VARIABLE_DECLARATION",
    "AST_KIND_FUNCTION_CALL",
    "AST_KIND_IDENTIFIER",
    "AST_KIND_NUMBER_LITERAL",
    "AST_KIND_BINOMIAL_EXPRESSION",
    "AST_KIND_UNARY_EXPRESSION",
};

static char* NodeType[] = {
    "AST_NODE_TYPE_BLANK",
    "AST_NODE_TYPE_NUMBER"
};

static char* BinomialExpressionType[] = {
    "AST_BINOMIAL_EXPRESSION_TYPE_BLANK",
    "AST_BINOMIAL_EXPRESSION_TYPE_SUM",
    "AST_BINOMIAL_EXPRESSION_TYPE_SUBTRACTION",
    "AST_BINOMIAL_EXPRESSION_TYPE_MULTIPLICATION",
    "AST_BINOMIAL_EXPRESSION_TYPE_DIVISION",
    "AST_BINOMIAL_EXPRESSION_TYPE_POWER",
};

static char* UnaryExpressionType[] = {
    "AST_UNARY_EXPRESSION_TYPE_BLANK",
    "AST_UNARY_EXPRESSION_TYPE_NEGATIVE",
};