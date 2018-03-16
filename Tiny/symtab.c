#include <stdlib.h>

#include "symtab.h"

#include "globals.h"
#include "util.h"

int currentAddr = 0;

typedef struct symtabnode
{
	char *n;
	ExpType e;
	int addr;
	struct symtabnode *next;
} SymTabNode;

SymTabNode head = { NULL, VoidK, 0, NULL };

static SymTabNode *st_makenode(const char *n, ExpType e);

void st_insert(const char *name, ExpType expType)
{
	SymTabNode *p;

	if (head.next == NULL)
		p = &head;
	else
		for (p = head.next; p->next != NULL; p = p->next)
			if (strcmp(p->n, name) == 0)
			{
				fprintf(listing, "Variable %s redeclared\n", name);
				return;
			}

	p->next = st_makenode(name, expType);
	p->next->addr = currentAddr;
	currentAddr += 1;
}

ExpType st_getType(const char *name)
{
	SymTabNode *p;

	if (head.next == NULL)
		return -1;

	for (p = head.next; p != NULL; p = p->next)
	{
		if (strcmp(p->n, name) == 0)
			return p->e;
	}

	return -1;
}

ExpType st_getAddr(const char *name)
{
	SymTabNode *p;

	if (head.next == NULL)
		return -1;

	for (p = head.next; p != NULL; p = p->next)
	{
		if (strcmp(p->n, name) == 0)
			return p->addr;
	}

	return -1;
}

void st_display()
{
	SymTabNode *p;

	printf("NAME----------TYPE----------ADDRESS\n");

	if (head.next)
		for (p = head.next; p != NULL; p = p->next)
		{
			printf("%-10s", p->n);
			if (p->e == IntegerK)
				printf("integer");
			else if(p->e == BooleanK)
				printf("boolean");
			else if (p->e == VoidK)
				printf("void[BUG]");
			else
				printf("BUG");
			printf("%10d\n", p->addr);
		}
}

void st_destroy()
{
	SymTabNode *p, *aux;

	p = head.next;

	while (p != NULL)
	{
		aux = p->next;
		free(p);
		p = aux;
	}
}

static SymTabNode *st_makenode(const char *n, ExpType e)
{
	SymTabNode *p = (SymTabNode*)malloc(sizeof(SymTabNode));
	p->n = copyStr(n);
	p->e = e;
	p->next = NULL;
	return p;
}
