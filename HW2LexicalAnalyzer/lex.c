/* 
	This is the lex.c file for the UCF Fall 2021 Systems Software Project.
	For HW2, you must implement the function lexeme *lexanalyzer(char *input).
	You may add as many constants, global variables, and support functions
	as you desire.
	
	If you choose to alter the printing functions or delete list or lex_index, 
	you MUST make a note of that in you readme file, otherwise you will lose 5 points.
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"

#define MAX_NUMBER_TOKENS 500
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5
#define LENGTH 100     // for char buffer
#define MAX_MUNBER_RESERVED_WORDS 14

/* Global vars */
lexeme *list;
int lex_index;
char *reservedWords[] = {"const", "var", "procedure", "begin", "end", "while", "do","if",
                        "then", "else", "call", "write", "read", "odd"};

/* Function prototypes */
void printlexerror(int type);
void printtokens();
int isReservedWord(char *buff);

/* Function to be implemented in HW2 */
lexeme *lexanalyzer(char *input)
{
  // Allocate space for the 500 lexeme structs
  list = calloc(MAX_NUMBER_TOKENS, sizeof(lexeme));

  char buffer[LENGTH] = {0};  
  int i = 0; // index to traverse the input char array
  int j;     // index to fill in the char buffer array
  int cnt;
  lex_index = 0;

  while(input[i] != '\0') 
  {
    // Invisible Characters: tab, white spaces, newline
    if(iscntrl(input[i]) || isspace(input[i])) 
    {
      i++;
      continue;
    }

    // Special Sysmbols: == != < <= > >= % * / + - ( ) , . ; :=
    else if(input[i] == '+')
      list[lex_index].type = 17;
    else if(input[i] == '-')
      list[lex_index].type = 18;
    else if(input[i] == '*')
      list[lex_index].type = 19;
    else if(input[i] == '%')
      list[lex_index].type = 21;
    else if(input[i] == '(')
      list[lex_index].type = 29;
    else if(input[i] == ')')
      list[lex_index].type = 30;    
    else if(input[i] == ',')
      list[lex_index].type = 31;  
    else if(input[i] == '.')
      list[lex_index].type = 32; 
    else if(input[i] == ';')
      list[lex_index].type = 33; 

    // Handle special symbol with 2 chars. Check next char
    // Symbol ==
    else if(input[i] == '=')
    {
      if(input[i + 1] == '=')
      {
        list[lex_index].type = 22;
        i++;
      }
      else // Invalid symbol
      {
        printlexerror(1); 
        return NULL;  
      } 
    } 
    
    // Symbol !=
    else if(input[i] == '!')
    {
      if(input[i + 1] == '=') {
        list[lex_index].type = 23;
        i++;
      }
      else // Invalid symbol
      {
        printlexerror(1);   
        return NULL; 
      }
    } 

    // Symbol :=
    else if(input[i] == ':')
    {
      if(input[i + 1] == '=') {
        list[lex_index].type = 16;
        i++;
      }
      else // Invalid symbol
      {
        printlexerror(1);  
        return NULL;  
      }
    } 

    // Symbol <= OR <
    else if(input[i] == '<')
    {
      if(input[i + 1] == '=') {
        list[lex_index].type = 25;
        i++;
      }
      else 
        list[lex_index].type = 24;    
    } 
    
    // Symbol >= OR >
    else if(input[i] == '>')
    {
      if(input[i + 1] == '=') {
        list[lex_index].type = 27;
        i++;
      }
      else 
        list[lex_index].type = 26;    
    } 

    else if(input[i] == '/')
    {
      if(input[i + 1] == '/') // It's a comment
      {
        // Move i to the next newline to scape from the comment
        while(input[i] != '\n')
          i++;
        // Once here, i is the index of '\n' in the array

        // This is necessary to avoid lex_index++ at the end of the while loop
        continue;
      }
      else // It's a division symbol
        list[lex_index].type = 20;
    }

    // Numbers
    else if(isdigit(input[i]))
    {
      cnt = 1;
      j = 0;
      memset(buffer, 0, LENGTH); // clear buffer

      do{
        buffer[j++] = input[i++];
        cnt++;

        if(cnt > 6) // Invalid number Length
        {
          printlexerror(3);
          return NULL;
        }
      }
      while(isdigit(input[i])); // if starts with a digit, it could be a number, or an invalid identifier

      // if loop break for reaching nondigit char -> invalid identifier
      if(isalpha(input[i]))
      {
        printlexerror(2);
        return NULL;
      }

      // If code reaches here, it's a valid number
      buffer[j] = '\0';    

      list[lex_index].value = atoi(buffer);
      list[lex_index].type = 15;

      i--; // to come back to the last digit read
    }

    // Identifiers / Reserved words
    else if(isalpha(input[i]))
    {
      cnt = 1;
      j = 0;
      memset(buffer, 0, LENGTH); // clear buffer
 
      do
      {
        buffer[j++] = input[i++];
        cnt++;

        if(cnt > 12) // Invalid identifier Length
        {
          printlexerror(4);
          return NULL;
        }
      }
      while(isalnum(input[i])); // while is alphanumeric char. 

      // If code reaches here, it's a valid identifier
      buffer[j] = '\0';

      i--; // to come back to the last alphanumeric read

      if(isReservedWord(buffer))
      {
        if(!strcmp(buffer, "const"))
          list[lex_index].type = 1;
        if(!strcmp(buffer, "var"))
          list[lex_index].type = 2;
        if(!strcmp(buffer, "procedure"))
          list[lex_index].type = 3;
        if(!strcmp(buffer, "begin"))
          list[lex_index].type = 4;
        if(!strcmp(buffer, "end"))
          list[lex_index].type = 5;
        if(!strcmp(buffer, "while"))
          list[lex_index].type = 6;
        if(!strcmp(buffer, "do"))
          list[lex_index].type = 7;
        if(!strcmp(buffer, "if"))
          list[lex_index].type = 8;
        if(!strcmp(buffer, "then"))
          list[lex_index].type = 9;
        if(!strcmp(buffer, "else"))
          list[lex_index].type = 10;
        if(!strcmp(buffer, "call"))
          list[lex_index].type = 11;
        if(!strcmp(buffer, "write"))
          list[lex_index].type = 12;
        if(!strcmp(buffer, "read"))
          list[lex_index].type = 13;
        if(!strcmp(buffer, "odd"))
          list[lex_index].type = 28;
      }
      else // it's an identifier
      {
        list[lex_index].type = 14;
        strcpy(list[lex_index].name, buffer);
      }
    } // End of else if for identifiers and reserved words


    // If character read is not an invisible character, a special symbol, a digit or letter,
    // then it's an invalid symbol
    else {
      printlexerror(1); 
      return NULL;  
    }

    // Global increments
    i++;
    lex_index++;

  } // End of while loop traversing the input string

  // Per instructions, call function before returning from the lexanalizer function
  printtokens();

	return list;
} /* End of lexanalyzer function */


void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case oddsym:
				printf("%11s\t%d", "odd", oddsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "==", eqlsym);
				break;
			case neqsym:
				printf("%11s\t%d", "!=", neqsym);
				break;
			case lsssym:
				printf("%11s\t%d", "<", lsssym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case divsym:
				printf("%11s\t%d", "/", divsym);
				break;
			case addsym:
				printf("%11s\t%d", "+", addsym);
				break;
			case subsym:
				printf("%11s\t%d", "-", subsym);
				break;
			case lparensym:
				printf("%11s\t%d", "(", lparensym);
				break;
			case rparensym:
				printf("%11s\t%d", ")", rparensym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case assignsym:
				printf("%11s\t%d", ":=", assignsym);
				break;
			case beginsym:
				printf("%11s\t%d", "begin", beginsym);
				break;
			case endsym:
				printf("%11s\t%d", "end", endsym);
				break;
			case ifsym:
				printf("%11s\t%d", "if", ifsym);
				break;
			case thensym:
				printf("%11s\t%d", "then", thensym);
				break;
			case elsesym:
				printf("%11s\t%d", "else", elsesym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case constsym:
				printf("%11s\t%d", "const", constsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
		}
		printf("\n");
	} // End of for loop traversing the lexeme list

	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
  
	printf("\n");
	list[lex_index++].type = -1;
}


void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");
	
	free(list);
	return;
}


/* Helper function */
int isReservedWord(char *buff) {
  for(int i = 0; i < MAX_MUNBER_RESERVED_WORDS; i++)
  {
    if(strcmp(buff, reservedWords[i]) == 0)
      return 1;
  }
  return 0;
}