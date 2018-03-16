#ifndef ANALYZE_HEADER
#define ANALYZE_HEADER

#include "globals.h"

extern int analyzeError;
void buildSymTab(TreeNode *t);
void typeCheck(TreeNode *t);

#endif


