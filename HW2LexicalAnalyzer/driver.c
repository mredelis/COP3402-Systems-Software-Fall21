/*
	This is the driver for the UCF Fall 2021 Systems Software Project
	If you choose to alter this, you MUST make a note of that in your
	readme file, otherwise you will lose 5 points.
*/

#include <stdlib.h>
#include <stdio.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 500

// MASTER OF CEREMONIES
int main(int argc, char *argv[])
{
	FILE *ifp;
	char *input;
	char c;
	lexeme *list;
	instruction *code;
	int i;
	
	if (argc < 2)
	{
		printf("Error : please include the file name\n");
		return 0;
	}
	
	ifp = fopen(argv[1], "r");

  // Allocate space for a char array of 500 elements
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
	
  // Homework #2 - Lexical Analyzer
	list = lexanalyzer(input);
	if (list == NULL)
	{
		free(input);
		return 0;
	}
	
	// code = parse(list);
	// if (code == NULL)
	// {
	// 	free(input);
	// 	free(list);
	// 	return 0;
	// }
	
	// execute_program(code);
	
	free(input);
	free(list);
	// free(code);
	return 0;
}