#ifndef CODEGEN_HEADER
#define CODEGEN_HEADER

#include "globals.h"

void translate(TreeNode *t);
void emitCode(const char *s);
void genCode(TreeNode *t);

#endif

