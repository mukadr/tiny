#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "scan.h"
#include "parse.h"
#include "symtab.h"
#include "analyze.h"
#include "cgen.h"

FILE *outputFile;
FILE *sourceFile;
FILE *listing;

int main(int argc, char *argv[])
{
	TreeNode *prog;

	if (argc < 3)
	{
		printf("Tinybasic compiler - by Mauro D\n"
			   "no input file\n"
			   "use: tiny source.tny output.tc\n");
		exit(0);
	}

	listing = stdout;

	if (!(sourceFile = fopen(argv[1], "r")))
	{
		printf("Error: source file\n");
		exit(1);
	}
	if (!(outputFile = fopen(argv[2], "w")))
	{
		printf("Error: output file\n");
		exit(1);
	}

	prog = parse();	
	fclose(sourceFile);
	/*displayTree(prog);*/

	buildSymTab(prog);
	/*st_display();*/
	if (prog->nodeKind == VarK)
		typeCheck(prog->sibling);
	else
		typeCheck(prog);

	if (!analyzeError)
	{
		if (prog->nodeKind == VarK)
			translate(prog->sibling);
		else
			translate(prog);
	}
	fclose(outputFile);

	destroyTree(prog);
	st_destroy();

	return 0;
}

