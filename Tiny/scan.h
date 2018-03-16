#ifndef SCANNER_HEADER
#define SCANNER_HEADER

#include "globals.h"

TokenType getToken();
void printToken(TokenType tok, const char *tokStr, int nl);

extern char tokenStr[];
extern unsigned long lineNum;

#endif
