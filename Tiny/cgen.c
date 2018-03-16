#include "globals.h"
#include "cgen.h"
#include "symtab.h"

void cg_emit(const char *s)
{
	fprintf(outputFile, "%s\n", s);
}

int labelCounter = 0;
char code[50];

void translate(TreeNode *t)
{
	sprintf(code, "res %d", currentAddr); /* Reservar espaço na pilha para vars */
	cg_emit(code);
	genCode(t);
	sprintf(code, "halt\n");
	cg_emit(code);
}

void genCode(TreeNode *t)
{
	int lab1, lab2;

	if (t)
	{
		if (t->nodeKind == StmtK)
		{
			switch (t->kind.stmt)
			{
				case AssignK:
					sprintf(code, "ldc %d", st_getAddr(t->attr.s));
					cg_emit(code);
					genCode(t->child[0]);
					if (t->child[0]->expType == BooleanK && t->child[0]->kind.exp == OpK)
					{
						lab1 = labelCounter++;
						lab2 = labelCounter++;
						sprintf(code, "ldc 1\n"
									  "go %d\n"
									  "lab %d\n"
									  "ldc 0\n"
									  "lab %d", lab2, lab1, lab2);
						cg_emit(code);
					}
					sprintf(code, "sti");
					cg_emit(code);
					break;
				case IfK:
					genCode(t->child[0]);
					lab1 = labelCounter++;
					if (t->child[0]->kind.exp == BConstK || 
						t->child[0]->kind.exp == IdK) /* if TRUE|FALSE then... */
					{
						sprintf(code, "ldc 1\n"
									  "ne %d", lab1);
						cg_emit(code);				
					}
					genCode(t->child[1]); /* true */
					if (t->child[2]) /* para pular parte else */
					{
						lab2 = labelCounter++;
						if (t->child[0]->kind.exp == IdK){}
						sprintf(code, "go %d", lab2);
						cg_emit(code);
					}
					sprintf(code, "lab %d", lab1);
					cg_emit(code);
					if (t->child[2]) /* para parte else */
					{
						genCode(t->child[2]);
						sprintf(code, "lab %d", lab2);
						cg_emit(code);					
					}
					break;
				case WhileK:
					lab2 = labelCounter++;
					sprintf(code, "lab %d", lab2);
					cg_emit(code);
					genCode(t->child[0]);
					lab1 = labelCounter++;
					if (t->child[0]->kind.exp == BConstK || 
						t->child[0]->kind.exp == IdK) /* if TRUE|FALSE then... */
					{
						sprintf(code, "ldc 1\n"
									  "ne %d", lab1);
						cg_emit(code);				
					}
					genCode(t->child[1]);
					sprintf(code, "go %d", lab2);
					cg_emit(code);
					sprintf(code, "lab %d", lab1);
					cg_emit(code);
					break;
				case RepeatK:
					lab1 = labelCounter++;
					sprintf(code, "lab %d", lab1);
					cg_emit(code);
					genCode(t->child[0]);
					genCode(t->child[1]);
					if (t->child[1]->kind.exp == BConstK || 
						t->child[1]->kind.exp == IdK) /* if TRUE|FALSE then... */
					{
						sprintf(code, "ldc 1\n"
									  "ne %d", lab1);
						cg_emit(code);				
					}
					break;
				case ReadK:
					sprintf(code, "ldc %d\n"
								  "call 0", st_getAddr(t->attr.s));
					cg_emit(code);
					break;
				case WriteK:
					genCode(t->child[0]);
					sprintf(code, "call 1");
					cg_emit(code);
					break;
			}
		}
		else if (t->nodeKind == ExpK)
		{
			switch (t->kind.exp)
			{
				case ConstK:
					sprintf(code, "ldc %d", t->attr.num);
					cg_emit(code);
					break;
				case BConstK:
					sprintf(code, "ldc %d", t->attr.num);
					cg_emit(code);
					break;
				case IdK:
					sprintf(code, "ldv %d", st_getAddr(t->attr.s));
					cg_emit(code);
					break;
				case OpK:
					genCode(t->child[0]);
					genCode(t->child[1]);

					if (t->attr.op == TOK_ADD || t->attr.op == TOK_SUB || 
     					t->attr.op == TOK_MUL || t->attr.op == TOK_DIV || 
						t->attr.op == TOK_MOD)
					{
						switch(t->attr.op)
						{
							case TOK_ADD: sprintf(code, "adi"); break;
							case TOK_SUB: sprintf(code, "sbi"); break;
							case TOK_MUL: sprintf(code, "mli"); break;
							case TOK_DIV: sprintf(code, "dvi"); break;
							case TOK_MOD: sprintf(code, "mdi"); break;
						}
						cg_emit(code);
					}
					else
					{
						switch(t->attr.op)
						{
							case TOK_EQ: sprintf(code, "ne %d", labelCounter); break;
							case TOK_NE: sprintf(code, "eq %d", labelCounter); break;
							case TOK_GT: sprintf(code, "le %d", labelCounter); break;
							case TOK_GE: sprintf(code, "lt %d", labelCounter); break;
							case TOK_LT: sprintf(code, "ge %d", labelCounter); break;
							case TOK_LE: sprintf(code, "gt %d", labelCounter); break;
						}
						cg_emit(code);
					}
					break;
			}
		}
		genCode(t->sibling);
	}
}
