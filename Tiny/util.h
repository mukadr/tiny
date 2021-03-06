#ifndef UTIL_HEADER
#define UTIL_HEADER

#include "globals.h"

char *copyStr(const char *s);

/* Cria nó de declaração */
TreeNode *newStmtNode(StmtKind kind);
/* Cria nó de expressão */
TreeNode *newExpNode(ExpKind kind);
/* Cria nó de variável */
TreeNode *newVarNode(ExpType type);

#endif
