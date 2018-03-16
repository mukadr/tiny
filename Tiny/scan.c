/*******************************************************************
/*   			  SCAN.C -> ANALISADOR LÉXICO
/*******************************************************************/


#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "scan.h"

/* Valores de retorno adicional de getToken */
char tokenStr[33];

char buffer[256];
int buflen = 0;
int pos = 0;

unsigned long lineNum = 0;

#define MAX_RESERVED 17
static struct
{
	char *s;
	TokenType t;
} reservedTable[MAX_RESERVED] = {
	{ "main", TOK_MAIN },
	{ "end", TOK_END },
	{ "if", TOK_IF },
	{ "then", TOK_THEN },
	{ "else", TOK_ELSE },
	{ "while", TOK_WHILE },
	{ "do", TOK_DO },
	{ "wend", TOK_WEND },
	{ "repeat", TOK_REPEAT },
	{ "until", TOK_UNTIL },
	{ "read", TOK_READ },
	{ "write", TOK_WRITE },
	{ "var", TOK_VAR },
	{ "integer", TOK_INTEGER },
	{ "boolean", TOK_BOOLEAN },
	{ "true", TOK_TRUE },
	{ "false", TOK_FALSE }
};

static TokenType checkReserved(const char *s)
{
	int i;
	for (i = 0; i < MAX_RESERVED; i++)
		if (strcmp(s, reservedTable[i].s) == 0)
			return reservedTable[i].t;
	return TOK_ID;
}

static int getReservedStr(TokenType tok)
{
	int i;
	for (i = 0; i < MAX_RESERVED; i++)
		if (tok == reservedTable[i].t)
			return i;
}

static char getNextChar()
{
	if (!(pos < buflen))
	{
		/* Obtem proxima linha do arquivo */
		if (fgets(buffer, 255, sourceFile))
		{
			buflen = strlen(buffer);
			pos = 0;
			lineNum++;
		}
		else
			return EOF;
	}
	return buffer[pos++];
}

static void ungetNextChar()
{
	pos--;
}

typedef enum { SKIP_WHITE, COMMENT, NUM, ID, DONE } State;
TokenType getToken()
{
	/* Estado atual */
	State state = SKIP_WHITE;
	/* Char atual */
	char currentChar;
	/* Indice para tokenStr */
	int tokenStrIdx = 0;
	/* Valor de retorno */
	TokenType result;
	/* Salvar em tokenStr ? */
	int save;

	do
	{
		currentChar = getNextChar();

		switch (state)
		{
			case SKIP_WHITE:
				save = FALSE;
				if (currentChar == '{')
				{
					state = COMMENT;
				}
				else if (isdigit(currentChar))
				{
					save = TRUE;
					state = NUM;
					result = TOK_NUM;
				}
				else if (isalpha(currentChar))
				{
					save = TRUE;
					state = ID;
					result = TOK_ID;
				}
				else if (currentChar != ' ' && currentChar != '\t' &&
                         currentChar != '\n' && currentChar != '\r')
				{
					state = DONE;
					switch (currentChar)
					{
						case EOF:
							result = TOK_EOF;
							break;
						case '+':
							result = TOK_ADD;
							break;
						case '-':
							result = TOK_SUB;
							break;
						case '*':
							result = TOK_MUL;
							break;
						case '/':
							result = TOK_DIV;
							break;
						case '%':
							result = TOK_MOD;
							break;
						case '=':
							result = TOK_EQ;
							break;
						case ';':
							result = TOK_SEMI;
							break;
						case '(':
							result = TOK_LPAREN;
							break;
						case ')':
							result = TOK_RPAREN;
							break;
						case ',':
							result = TOK_COMMA;
							break;

						case ':':
						{
							char temp = getNextChar();

							result = TOK_COLON;

							if (temp == '=')
								result = TOK_ASSIGN;
							else
								ungetNextChar();

							break;
						}

						case '<':
						{
							char temp = getNextChar();

							result = TOK_LT;

							if (temp == '=')
								result = TOK_LE;
							else if (temp == '>')
								result = TOK_NE;
							else
								ungetNextChar();

							break;
						}

						case '>':
						{
							char temp = getNextChar();

							result = TOK_GT;

							if (temp == '=')
								result = TOK_GE;
							else
								ungetNextChar();
							break;
						}

						default:
							result = TOK_ERROR;
							break;
					}
				}
				break;

			case COMMENT:
				if (currentChar == '}')
					state = SKIP_WHITE;
				break;

			case NUM:
				if (!isdigit(currentChar))
				{
					ungetNextChar();
					state = DONE;
					save = FALSE;
				}
				break;

			case ID:
				if (!isalpha(currentChar))
				{
					ungetNextChar();
					state = DONE;
					save = FALSE;
				}
				break;
		}
		if (save)
			tokenStr[tokenStrIdx++] = currentChar;
	}
	while (state != DONE);

	tokenStr[tokenStrIdx] = 0;

	if (result == TOK_ID) result = checkReserved(tokenStr);

	#if DEBUG_SCAN
	if (tokenStr[0])
		fprintf(listing, "%d, [%s]\n", result, tokenStr);
	else
		fprintf(listing, "%d\n", result);
	#endif

	return result;
}

void printToken(TokenType tok, const char *tokStr, int nl)
{
	switch (tok)
	{
		case TOK_MAIN:
		case TOK_END:
		case TOK_IF:
		case TOK_THEN:
		case TOK_ELSE:
		case TOK_WHILE:
		case TOK_REPEAT:
		case TOK_UNTIL:
		case TOK_VAR:
		case TOK_INTEGER:
		case TOK_BOOLEAN:
		case TOK_READ:
		case TOK_WRITE:
		case TOK_WEND:
		case TOK_DO:
		case TOK_TRUE:
		case TOK_FALSE:
			fprintf(listing, "Reserved: %s", reservedTable[getReservedStr(tok)]);
			break;
		case TOK_ID: fprintf(listing, "TOK_ID: %s", tokStr); break;
		case TOK_NUM: fprintf(listing, "TOK_NUM: %s", tokStr); break;
		case TOK_ERROR: fprintf(listing, "TOK_ERROR", tokStr); break;

		case TOK_ASSIGN: fprintf(listing, "':='"); break;
		case TOK_EQ: fprintf(listing, "'='"); break;
		case TOK_NE: fprintf(listing, "'<>'"); break;
		case TOK_GT: fprintf(listing, "'>'"); break;
		case TOK_GE: fprintf(listing, "'>='"); break;
		case TOK_LT: fprintf(listing, "'<'"); break;
		case TOK_LE: fprintf(listing, "'<='"); break;
		case TOK_ADD: fprintf(listing, "'+'"); break;
		case TOK_SUB: fprintf(listing, "'-'"); break;
		case TOK_MUL: fprintf(listing, "'*'"); break;
		case TOK_DIV: fprintf(listing, "'/'"); break;
		case TOK_MOD: fprintf(listing, "'%'"); break;
		case TOK_COLON: fprintf(listing, "':'"); break;
		case TOK_SEMI: fprintf(listing, "';'"); break;
		case TOK_LPAREN: fprintf(listing, "'('"); break;
		case TOK_RPAREN: fprintf(listing, "')'"); break;
		case TOK_COMMA: fprintf(listing, "','"); break;
	}

	if (nl)
		fprintf(listing, "\n");
}

