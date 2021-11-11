/*
	This is the driver for the UCF Fall 2021 Systems Software Project
	If you choose to alter this, you MUST make a note of that in your
	readme file, otherwise you will lose 5 points.
*/

#include <stdlib.h>
#include <stdio.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 3000

// MASTER OF CEREMONIES
// argc(ARGument Count) is the # of command line arguments including the name of the execute_program
// argv(ARGument Vector) is array of character pointers listing all the arguments.
// argv[0] is the name of the program. After that till argv[argc-1] every elem is command-line arg
int main(int argc, char *argv[])
{
	FILE *ifp;
	char *input;
	char c;
	lexeme *list;
	instruction *code;
	int i;
	int tokens = 0, symbols = 0, codes = 0, outputs = 0;
	
	if (argc < 2)
	{
		printf("Error : please include the file name\n");
		return 0;
	}
	
	ifp = fopen(argv[1], "r");

  // Allocate space for a char array of MAX_CODE_LENGTH elements
	input = malloc(MAX_CODE_LENGTH * sizeof(char));
	i = 0;
	
  // Read in input file to a char array
	c = fgetc(ifp);
	while (1)
	{
		input[i++] = c;
		c = fgetc(ifp);
		if (c == EOF)
			break;
	}
	input[i] = '\0';
	
	for (i = 2; i < argc; i++)
	{
		if (argv[i][1] == 'l')      //print the list and table of lex/tokens (HW2 output) 
			tokens = 1;
		else if (argv[i][1] == 's') //print symbol table
			symbols = 1;
		else if (argv[i][1] == 'a') //print the generated assembly code (parser/codegen output)
			codes = 1;
		else if (argv[i][1] == 'v') //print vm execution trace (HW1 output)
			outputs = 1;
		else
		{
			printf("Error : unrecognized directive\n");
			free(input);
			return 0;
		}
	}


	list = lexanalyzer(input, tokens);
	if (list == NULL)
	{
		free(input);
		return 0;
	}

 
	code = parse(list, symbols, codes);
	if (code == NULL)
	{
	 	free(input);
	 	free(list);
	 	return 0;
	}
	
	execute_program(code, outputs);
	
	free(input);
	free(list);
	free(code);
	return 0;
}