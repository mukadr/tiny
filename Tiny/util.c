#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "scan.h"

static void fatal(const char *s)
{
	printf("FATAL: %s\n", s);
	exit(1);
}

char *copyStr(const char *s)
{
	char *res = (char *) malloc(strlen(s) + 1);
	
	if (!res)
		fatal("Erro ao alocar string. Operacão cancelada.");
	
	strcpy(res, s);
	return res;
}

TreeNode *newStmtNode(StmtKind kind)
{
	int i;
	TreeNode *t = (TreeNode *) malloc(sizeof(TreeNode));

	if (!t)
		fatal("Erro ao alocar nó. Operação cancelada.");

	for (i = 0; i < MAXCHILDREN; i++) 
		t->child[i] = NULL;
	t->nodeKind = StmtK;
	t->sibling = NULL;
	t->lineNum = lineNum;
	t->kind.stmt = kind;
	t->attr.s = NULL;
	
	return t;
}

TreeNode *newExpNode(ExpKind kind)
{
	int i;
	TreeNode *t = (TreeNode *) malloc(sizeof(TreeNode));

	if (!t)
		fatal("Erro ao alocar nó. Operação cancelada.");

	for (i = 0; i < MAXCHILDREN; i++) 
		t->child[i] = NULL;
	t->nodeKind = ExpK;
	t->sibling = NULL;
	t->lineNum = lineNum;
	t->kind.exp = kind;
	t->attr.s = NULL;
	
	return t;
}

TreeNode *newVarNode(ExpType type)
{
	int i;
	TreeNode *t = (TreeNode *) malloc(sizeof(TreeNode));

	if (!t)
		fatal("Erro ao alocar nó. Operação cancelada.");

	for (i = 0; i < MAXCHILDREN; i++) 
		t->child[i] = NULL;
	t->nodeKind = VarK;
	t->sibling = NULL;
	t->lineNum = lineNum;
	t->expType = type;
	t->attr.s = NULL;
	
	return t;
}


