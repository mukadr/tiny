/*******************************************************************
 *   			  PARSE.C -> ANALISADOR SINTÁTICO
 *
 *  by mauro dreissig - 01/2007
 *******************************************************************/

/*
	REGRA EBNF

	<program>		::= 	[<var_list>] <main_block>

	<var>			::=		VAR <var_list>
	<var_list>		::= 	<var_decl> { <var_decl> }
	<var_decl>		::=		<var_type> ID {"," ID}
	<var_type>		::=		INTEGER | BOOLEAN

	<main_block>	::= 	MAIN <block> END

	<block>			::= 	<stmt> { <stmt> }
	<stmt>			::= 	<if_stmt> | <while_stmt> | <repeat_stmt> |
							<assign_stmt> | <write_stmt> | <read_stmt>

	<if_stmt>		::=		IF <exp> THEN <block> [ ELSE <block> ] END
	<while_stmt>	::=		WHILE <exp> DO <block> WEND
	<repeat_stmt>	::=		REPEAT <block> UNTIL <exp>
	<assign_stmt>	::=		ID ":=" <exp>
	<write_stmt>	::=		WRITE <exp>
	<read_stmt>		::=		READ ID

	<exp>			::=		<exp2> { <cmp> <exp2> }
	<cmp>			::=   	"<" | ">" | "=" | "<>" | "<=" | ">="
	<exp2>			::= 	<exp3> { <add> <exp3> }
	<add>			::=		"+" | "-"
	<exp3>			::=		<exp4> { <mul> <exp4> }
	<mul>			::=		"*" | "/" | "%"
	<exp4>			::=		"("<exp>")" | NUMBER | ID | BOOL

BOOL = TRUE | FALSE
*/


/*
var
	integer x;

main
	x := 0;
	while x <= 100 do
		x := x + 1;
		write x;
	wend
end
*/


/*
 Declaração de variaveis:
var
 integer x, y;
 boolean z, w, h;

   var --- main_block
   |
   |
   decl_int ----- decl_bool
   |              |
   |              |
   x -- y         z -- w -- h


*/

#include "globals.h"
#include "scan.h"
#include "util.h"
#include "parse.h"
#include <stdlib.h>

static TokenType token;

static TreeNode *program();
static TreeNode *var();
static TreeNode *var_list();
static TreeNode *var_decl();

static TreeNode *main_block();
static TreeNode *block();
static TreeNode *stmt();

static TreeNode *if_stmt();
static TreeNode *assign_stmt();
static TreeNode *while_stmt();
static TreeNode *repeat_stmt();
static TreeNode *write_stmt();
static TreeNode *read_stmt();

static TreeNode *exp();
static TreeNode *exp2();
static TreeNode *exp3();
static TreeNode *exp4();

int identLvl = 0;

TreeNode *progTree;
TreeNode *parse()
{
	token = getToken();
	progTree = program();

	return progTree;
}

static void parseError(const char *s)
{
	fprintf(listing, "Syntax error: %s[%d]\n", s, lineNum);
	destroyTree(progTree);
	exit(1);
}

static void match(TokenType tok)
{
	if (tok == token)
		token = getToken();
	else
		parseError("Unexpected token");
}

static TreeNode *program()
{
	TreeNode *t;

	if (token == TOK_VAR)
	{
		t = var();
		t->sibling = main_block();
	}
	else
		t = main_block();

	return t;
}

static TreeNode *var()
{
	TreeNode *t = NULL;

	match(TOK_VAR);

	t = var_list();

	return t;
}

static TreeNode *var_list()
{
	TreeNode *t = newVarNode(VoidK); /* Tipo nao interessa */
	TreeNode *temp;

	t->child[0] = var_decl();

	if (t->child[0])
	{
		temp = t->child[0];

		while (token == TOK_INTEGER || token == TOK_BOOLEAN)
		{
			temp->sibling = var_decl();
			temp = temp->sibling;
		}

		temp->sibling = NULL;
	}

	return t;
}

static TreeNode *var_decl()
{
	TreeNode *t, *temp;

	switch (token)
	{
		case TOK_INTEGER:
			t = newVarNode(IntegerK);
			match(TOK_INTEGER);
			break;
		case TOK_BOOLEAN:
			t = newVarNode(BooleanK);
			match(TOK_BOOLEAN);
			break;
		default:
			parseError("Invalid variable kind");
			t = NULL;
			break;
	}

	if (t)
	{
		t->child[0] = newVarNode(t->expType);
		temp = t->child[0];
		temp->attr.s = copyStr(tokenStr);
		match(TOK_ID);

		while (token == TOK_COMMA)
		{
			match(TOK_COMMA);
			temp->sibling = newVarNode(t->expType);
			temp = temp->sibling;
			temp->attr.s = copyStr(tokenStr);
			match(TOK_ID);
		}

		temp->sibling = NULL;
	}

	return t;
}


static TreeNode *main_block()
{
	TreeNode *t = NULL;

	match(TOK_MAIN);

	t = block();

	match(TOK_END);

	return t;
}

static TreeNode *block()
{
	TreeNode *t = stmt();
	TreeNode *p = t;

	if (t)
	{
		while (token != TOK_END && token != TOK_ELSE &&
			   token != TOK_UNTIL && token != TOK_WEND &&
			   token != TOK_EOF && token != TOK_ERROR)
		{
			p->sibling = stmt();

			if (!p->sibling) break;

			p = p->sibling;
		}
	}

	return t;
}

static TreeNode *stmt()
{
	TreeNode *t = NULL;

	switch (token)
	{
		case TOK_ID: t = assign_stmt(); break;
		case TOK_IF: t = if_stmt(); break;
		case TOK_WHILE: t = while_stmt(); break;
		case TOK_REPEAT: t = repeat_stmt(); break;
		case TOK_WRITE: t = write_stmt(); break;
		case TOK_READ: t = read_stmt(); break;
	}

	return t;
}

static TreeNode *if_stmt()
{
	TreeNode *t = newStmtNode(IfK);

	match(TOK_IF);
	t->child[0] = exp();
	match(TOK_THEN);
	t->child[1] = block();

	if (token == TOK_ELSE)
	{
		match(TOK_ELSE);
		t->child[2] = block();
	}
	match(TOK_END);

	return t;
}

static TreeNode *assign_stmt()
{
	TreeNode *t = newStmtNode(AssignK);

	t->attr.s = copyStr(tokenStr);
	match(TOK_ID);
	match(TOK_ASSIGN);
	t->child[0] = exp();

	return t;
}

static TreeNode *while_stmt()
{
	TreeNode *t = newStmtNode(WhileK);

	match(TOK_WHILE);
	t->child[0] = exp();
	match(TOK_DO);
	t->child[1] = block();
	match(TOK_WEND);

	return t;
}

static TreeNode *repeat_stmt()
{
	TreeNode *t = newStmtNode(RepeatK);

	match(TOK_REPEAT);
	t->child[0] = block();
	match(TOK_UNTIL);
	t->child[1] = exp();

	return t;
}

static TreeNode *write_stmt()
{
	TreeNode *t = newStmtNode(WriteK);

	match(TOK_WRITE);
	t->child[0] = exp();

	return t;
}

static TreeNode *read_stmt()
{
	TreeNode *t = newStmtNode(ReadK);

	match(TOK_READ);
	t->attr.s = copyStr(tokenStr);
	match(TOK_ID);

	return t;
}

static TreeNode *exp()
{
	TreeNode *t = exp2();

	if (token == TOK_EQ || token == TOK_NE || token == TOK_GT ||
		   token == TOK_GE || token == TOK_LT || token == TOK_LE)
	{
		TreeNode *p = newExpNode(OpK);
		p->child[0] = t;
		p->attr.op = token;
		t = p;
		match(token);
		p->child[1] = exp2();
	}

	return t;
}

static TreeNode *exp2()
{
	TreeNode *t = exp3();

	while (token == TOK_ADD || token == TOK_SUB)
	{
		TreeNode *p = newExpNode(OpK);
		p->child[0] = t;
		p->attr.op = token;
		t = p;
		match(token);
		p->child[1] = exp3();
	}

	return t;
}

static TreeNode *exp3()
{
	TreeNode *t = exp4();

	while (token == TOK_MUL || token == TOK_DIV || token == TOK_MOD)
	{
		TreeNode *p = newExpNode(OpK);
		p->child[0] = t;
		p->attr.op = token;
		t = p;
		match(token);
		p->child[1] = exp4();
	}

	return t;
}

static TreeNode *exp4()
{
	TreeNode *t = NULL;

	switch (token)
	{
		case TOK_LPAREN:
			match(TOK_LPAREN);
			t = exp();
			match(TOK_RPAREN);
			break;
		case TOK_ID:
			t = newExpNode(IdK);
			t->attr.s = copyStr(tokenStr);
			match(TOK_ID);
			break;
		case TOK_NUM:
			t = newExpNode(ConstK);
			t->attr.num = atoi(tokenStr);
			match(TOK_NUM);
			break;
		case TOK_TRUE:
		case TOK_FALSE:
			t = newExpNode(BConstK);
			t->attr.num = (token == TOK_TRUE) ? TRUE : FALSE;
			match(token);
			break;
		default:
			parseError("Unexpected token");
			break;
	}

	return t;
}

static void printIdent()
{
	int i;
	for (i = 0; i < identLvl; i++)
		fprintf(listing, " ");
}

void displayTree(TreeNode *tree)
{
	TreeNode *t = tree;

	identLvl += 3;

	while (t != NULL)
	{
		int i;

		printIdent();
		switch (t->nodeKind)
		{
			case StmtK:
				switch (t->kind.stmt)
				{
					case IfK:
						fprintf(listing, "If\n");
						break;
					case WhileK:
						fprintf(listing, "While\n");
						break;
					case RepeatK:
						fprintf(listing, "Repeat\n");
						break;
					case AssignK:
						fprintf(listing, "Assign to: %s\n", t->attr.s);
						break;
					case ReadK:
						fprintf(listing, "Read to: %s\n", t->attr.s);
						break;
					case WriteK:
						fprintf(listing, "Write\n");
						break;
					default:
						fprintf(listing, "Bug (Unknown StmtK)\n");
						break;
				}
				break;
			case ExpK:
				switch (t->kind.exp)
				{
					case OpK:
						fprintf(listing, "Op: ");
						printToken(t->attr.op, NULL, TRUE);
						break;
					case IdK:
						fprintf(listing, "Id: %s\n", t->attr.s);
						break;
					case ConstK:
						fprintf(listing, "Const: %d\n", t->attr.num);
						break;
					case BConstK:
						if (t->attr.num == TRUE)
							fprintf(listing, "Boolean Const: TRUE\n");
						else
							fprintf(listing, "Boolean Const: FALSE\n");
						break;
					default:
						fprintf(listing, "Bug (Unknown ExpK)\n");
						break;
				}
				break;
			case VarK:
				switch (t->expType)
				{
					case VoidK:
						fprintf(listing, "Var\n");
						break;
					case IntegerK:
						if (t->attr.s != NULL)
							fprintf(listing, "Integer: %s\n", t->attr.s);
						else
							fprintf(listing, "Integer\n");
						break;
					case BooleanK:
						if (t->attr.s != NULL)
							fprintf(listing, "Boolean: %s\n", t->attr.s);
						else
							fprintf(listing, "Boolean\n");
						break;
				}
				break;
		}

		for (i = 0; i < MAXCHILDREN; i++)
			displayTree(t->child[i]);

		t = t -> sibling;
	}

	identLvl -= 3;
}

void destroyTree(TreeNode *t)
{
	if (t != NULL)
	{
		int i;
		for (i = 0; i < MAXCHILDREN; i++)
			destroyTree(t->child[i]);
		destroyTree(t->sibling);

		switch (t->nodeKind)
		{
			case StmtK:
				if (t->kind.stmt == ReadK || t->kind.stmt == AssignK)
					if (t->attr.s != NULL)
						free(t->attr.s);
				break;
			case ExpK:
				if (t->kind.exp == IdK)
					if (t->attr.s != NULL)
						free(t->attr.s);
				break;
			case VarK:
				if (t->attr.s != NULL)
						free(t->attr.s);
				break;
		}
		free(t);
	}
}

