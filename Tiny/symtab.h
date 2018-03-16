#ifndef SYMBOL_TABLE_HEADER
#define SYMBOL_TABLE_HEADER

#include "globals.h"

extern int currentAddr;

void st_insert(const char *name, ExpType expType);
ExpType st_getType(const char *name);
ExpType st_getAddr(const char *name);
void st_display();
void st_destroy();

#endif

