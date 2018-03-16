#include "analyze.h"
#include "symtab.h"

int analyzeError = FALSE;

/* Constrói tabelas de variáveis */
void buildSymTab(TreeNode *t)
{
	TreeNode *p;

	if (t->nodeKind != VarK) /* Sem nó Var, nenhuma variável */
		return;

	if (t->child[0] == NULL) /* Nó de Var vazio */
		return;

	t = t->child[0];

	while (t != NULL)
	{
		p = t->child[0];
		while (p)
		{
			st_insert(p->attr.s, p->expType);
			p = p->sibling;
		}
		t = t->sibling;
	}
}

void typeCheck(TreeNode *t)
{
	int i;
	if (t)
	{
		for (i = 0; i < MAXCHILDREN; i++)
			typeCheck(t->child[i]);
		
		if (t->nodeKind == StmtK)
		{
			switch (t->kind.stmt)
			{
				case AssignK:
					if ( (t->expType = st_getType(t->attr.s)) == -1)
					{
						fprintf(listing, "Variable %s not declared at line %d\n", t->attr.s, t->lineNum);
						analyzeError = TRUE;
						break;
					}
					if (t->child[0]->expType != t->expType)
					{
						fprintf(listing, "Incompatible types in assignment at line %d\n", t->lineNum);
						analyzeError = TRUE;
						break;
					}
					break;
				case IfK:
					if (t->child[0]->expType != BooleanK)
					{
						fprintf(listing, "Incompatible types in if expression at line %d\n", t->lineNum);
						analyzeError = TRUE;
						break;
					}				
					break;
				case WhileK:
					if (t->child[0]->expType != BooleanK)
					{
						fprintf(listing, "Incompatible types in while expression at line %d\n", t->lineNum);
						analyzeError = TRUE;
						break;
					}				
					break;
				case RepeatK:
					if (t->child[1]->expType != BooleanK)
					{
						fprintf(listing, "Incompatible types in repeat expression at line %d\n", t->lineNum);
						analyzeError = TRUE;
						break;
					}				
					break;
			}
		} 
		else if (t->nodeKind == ExpK)
		{
			switch (t->kind.exp)
			{
				case ConstK:
					t->expType = IntegerK;
					break;
				case BConstK:
					t->expType = BooleanK;
					break;
				case IdK:
					if ( (t->expType = st_getType(t->attr.s)) == -1)
					{
						fprintf(listing, "Variable %s not declared at line %d\n", t->attr.s, t->lineNum);
						analyzeError = TRUE;
					}
					break;
				case OpK:
					if (t->attr.op == TOK_EQ || t->attr.op == TOK_NE)
					{
						if (t->child[0]->expType != t->child[1]->expType)
						{
							fprintf(listing, "Incompatible types at line %d\n", t->lineNum);
							analyzeError = TRUE;
							break;
						}
						t->expType = t->child[0]->expType;
					}	
					else if (t->child[0]->expType != IntegerK || 
				             t->child[1]->expType != IntegerK)
					{
						fprintf(listing, "Incompatible types at line %d\n", t->lineNum);
						analyzeError = TRUE;
						break;					
					}
					switch(t->attr.op)
					{
						case TOK_ADD: case TOK_SUB: case TOK_MUL: case TOK_DIV: case TOK_MOD: 
							t->expType = IntegerK;
							break;
						default:
							t->expType = BooleanK;
					}
					break;
			}
		}
		typeCheck(t->sibling);
	}
}

