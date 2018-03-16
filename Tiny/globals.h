#ifndef GLOBALS_HEADER
#define GLOBALS_HEADER

#include <stdio.h>

/*------------------------------------------------------------------*/

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* Imprimir informacoes uteis na analise lexica */
#define DEBUG_SCAN FALSE

extern FILE *sourceFile;
extern FILE *listing;
extern FILE *outputFile;

/*------------------------------------------------------------------*/
/* Marcas */
/*------------------------------------------------------------------*/
typedef enum { TOK_ID=1, TOK_NUM, TOK_MAIN, TOK_END, TOK_WEND,
			   TOK_IF, TOK_THEN, TOK_ELSE, TOK_WHILE, TOK_REPEAT, 
               TOK_UNTIL, TOK_VAR, TOK_INTEGER, TOK_BOOLEAN, TOK_ASSIGN, 
               TOK_EQ, TOK_NE, TOK_GT, TOK_GE, TOK_LT, 
               TOK_LE, TOK_ADD, TOK_SUB, TOK_MUL, TOK_DIV, 
               TOK_MOD, TOK_COLON, TOK_SEMI, TOK_LPAREN, TOK_RPAREN,
			   TOK_COMMA, TOK_WRITE, TOK_READ, TOK_DO, TOK_FALSE,
			   TOK_TRUE, TOK_ERROR, TOK_EOF } TokenType;

/*------------------------------------------------------------------*/
/* Árvore sintática */
/*------------------------------------------------------------------*/
typedef enum { StmtK, ExpK, VarK } NodeKind;
typedef enum { IfK, WhileK, RepeatK, ReadK, WriteK, AssignK } StmtKind;
typedef enum { OpK, IdK, ConstK, BConstK } ExpKind;
typedef enum { IntegerK, BooleanK, VoidK } ExpType;

#define MAXCHILDREN 3

typedef struct treenode
{
	NodeKind nodeKind;
	struct treenode *child[MAXCHILDREN];
	struct treenode *sibling;
	unsigned long lineNum;
	union { StmtKind stmt; ExpKind exp; } kind;
	union { TokenType op; char *s; int num; } attr;
	ExpType expType;
} TreeNode;

/*------------------------------------------------------------------*/

#endif

