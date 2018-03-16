/* TINY BASIC MACHINE :P

-Ultra simples e sem checagens...

Comando
ou
Comando<1 espaço>parâmetro

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *inputFile;

typedef enum { OP_LDC = 0, OP_LDV, OP_STI, OP_ADI, OP_SBI, OP_MLI, OP_DVI, OP_MDI,
			   OP_LAB, OP_GO, OP_EQ, OP_NE, OP_GT, OP_GE, OP_LT, OP_LE,
			   OP_RES, OP_CALL, OP_HALT } Opcode;

typedef enum { FUNC_READ = 0, FUNC_WRITE } Functions;

int useParam;
int opParam;
Opcode getNextOpcode()
{
	char buf[50], *bufPtr = buf;

	useParam = 0;

	if (fgets(buf, 49, inputFile))
	{
		char temp[10], temp2[10], *tempPtr = temp;
		while (isalpha(*bufPtr))
			*tempPtr++ = *bufPtr++;
		*tempPtr = 0;

		if (strcmp(temp, "sti") == 0)
			return OP_STI;
		if (strcmp(temp, "adi") == 0)
			return OP_ADI;
		if (strcmp(temp, "sbi") == 0)
			return OP_SBI;
		if (strcmp(temp, "mli") == 0)
			return OP_MLI;
		if (strcmp(temp, "dvi") == 0)
			return OP_DVI;
		if (strcmp(temp, "mdi") == 0)
			return OP_MDI;
		if (strcmp(temp, "halt") == 0)
			return OP_HALT;

		bufPtr++; /* skip white */
		tempPtr = temp2;
		while (isdigit(*bufPtr))
			*tempPtr++ = *bufPtr++;
		*tempPtr = 0;
		opParam = atoi(temp2);

		useParam = 1;

		if (strcmp(temp, "lab") == 0)
			return OP_LAB;
		if (strcmp(temp, "res") == 0)
			return OP_RES;
		if (strcmp(temp, "call") == 0)
			return OP_CALL;
		if (strcmp(temp, "ldc") == 0)
			return OP_LDC;
		if (strcmp(temp, "ldv") == 0)
			return OP_LDV;
		if (strcmp(temp, "go") == 0)
			return OP_GO;
		if (strcmp(temp, "gt") == 0)
			return OP_GT;
		if (strcmp(temp, "ge") == 0)
			return OP_GE;
		if (strcmp(temp, "lt") == 0)
			return OP_LT;
		if (strcmp(temp, "le") == 0)
			return OP_LE;
		if (strcmp(temp, "eq") == 0)
			return OP_EQ;
		if (strcmp(temp, "ne") == 0)
			return OP_NE;
	}
	return OP_HALT;
}

#define STACK_SIZE 32
int stack[STACK_SIZE];
int *sPtr;

#define PROGRAM_SIZE 1024
int program[PROGRAM_SIZE];
int *pPtr;

int varCount = 0;
int labCount = 0;

void loadProgram(const char *s)
{
	int *p, addr;
	Opcode op;

	inputFile = fopen(s, "r");
	if (!inputFile)
	{
		printf("Error: input file\n");
		exit(1);
	}

	p = program;
	addr = 0;
	while ( (op = getNextOpcode()) != OP_HALT)
	{
		if (useParam)
		{
			if (op == OP_RES)
				varCount = opParam;
			else if (op == OP_LAB)
			{
				stack[varCount+opParam] = addr;
				labCount++;
			}
			else
			{
				*p++ = op;
				*p++ = opParam;
				addr+=2;
			}
		}
		else
		{
			*p++ = op;
			addr++;
		}
	}
	*p = OP_HALT;


	fclose(inputFile);
}

void push(int x)
{
	*sPtr++ = x;
}

int pop()
{
	return *--sPtr;
}

void runProgram()
{
	int a, b;

	sPtr = &stack[varCount+labCount];
	pPtr = program;

	while (*pPtr != OP_HALT)
	{
		switch (*pPtr)
		{
			case OP_LDC:
				pPtr++; /* param */
				push(*pPtr);
				break;
			case OP_LDV:
				pPtr++; /* param */
				push(stack[*pPtr]);
				break;
			case OP_STI:
				a = pop(); /* val */
				b = pop(); /* addr */
				stack[b] = a;
				break;
			case OP_CALL:
				pPtr++; /* param */
				switch(*pPtr)
				{
					case FUNC_WRITE:
						a = pop();
						printf("%d\n", a);
						break;
					case FUNC_READ:
						a = pop();
						printf(": ");
						scanf("%d", &stack[a]);
						break;
				}
				break;
			case OP_ADI:
				a = pop();
				b = pop();
				push(b+a);
				break;
			case OP_SBI:
				a = pop();
				b = pop();
				push(b-a);
				break;
			case OP_MLI:
				a = pop();
				b = pop();
				push(b*a);
				break;
			case OP_DVI:
				a = pop();
				b = pop();
				push(b/a);
				break;
			case OP_MDI:
				a = pop();
				b = pop();
				push(b%a);
				break;
			case OP_GO:
				pPtr++; /* param */
				pPtr = &program[stack[varCount+*pPtr]];
				pPtr--;
				break;
			case OP_EQ:
				a = pop();
				b = pop();
				pPtr++; /* param */
				if (b == a)
				{
					pPtr = &program[stack[varCount+*pPtr]];
					pPtr--;
				}
				break;
			case OP_NE:
				a = pop();
				b = pop();
				pPtr++; /* param */
				if (b != a)
				{
					pPtr = &program[stack[varCount+*pPtr]];
					pPtr--;
				}
				break;
			case OP_GT:
				a = pop();
				b = pop();
				pPtr++; /* param */
				if (b > a)
				{
					pPtr = &program[stack[varCount+*pPtr]];
					pPtr--;
				}
				break;
			case OP_GE:
				a = pop();
				b = pop();
				pPtr++; /* param */
				if (b >= a)
				{
					pPtr = &program[stack[varCount+*pPtr]];
					pPtr--;
				}
				break;
			case OP_LT:
				a = pop();
				b = pop();
				pPtr++; /* param */
				if (b < a)
				{
					pPtr = &program[stack[varCount+*pPtr]];
					pPtr--;
				}
				break;
			case OP_LE:
				a = pop();
				b = pop();
				pPtr++; /* param */
				if (b <= a)
				{
					pPtr = &program[stack[varCount+*pPtr]];
					pPtr--;
				}
				break;
		}
		pPtr++;
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Tinybasic machine - by Mauro D\n"
			   "no input file\n"
			   "use: tm source.tc\n");
		exit(0);
	}

	loadProgram(argv[1]);

	runProgram();

	return 0;
}
