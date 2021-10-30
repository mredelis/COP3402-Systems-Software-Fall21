#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100
#define MAX_SYMBOL_TABLE_SIZE 500

typedef enum op_code {
  LIT = 1, OPR, LOD, STO, CAL, INC, JMP, JPC, SIO 
} op_code;

//if op_code=2 (OPR), the M value has opr type
typedef enum opr {
  RTN = 0, NEG = 1, ADD, SUB, MUL, DIV, ODD, MOD, EQL, NEQ, LSS, LEQ, GTR, GEQ
} opr;

//if op_code=9 (SIO), the M value has the sio type
typedef enum sio {
  WRT = 1, READ = 2, HALT
} sio;

int beginEndFlag;

/* Global Variables */
instruction *code;
int cIndex;       // code index
symbol *table;
int tIndex;       // symbol table index
int level;     // lexicographical level
lexeme token;
lexeme *tokensList;
int tokenIdx;
int symIdx;     //return value of multiple_declaration_check()

/* Utilities functions. Given*/
void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();
void getNextToken();
int multiple_declaration_check(lexeme token);
int find_symbol(lexeme token, int kind);

/* Parsing Functions */
void program();
void block();
void const_declaration();
int var_declaration();
void procedure_declaration();
void statement();
void mark();
void condition();
void expression();
void term();
void factor();


/* HW3 function to implement */
instruction *parse(lexeme *list, int printTable, int printCode)
{
	code = NULL;

  // ??????????????????????????????????????????????????????????????
  cIndex = 0;
  tIndex = 0;
  tokenIdx = 0;

  //asign the lexeme list (HW2 output) to a global var to avoid passing it as an argument to the functions
  tokensList = list; 

  //code & table are allocated dynamically bc in the code given they are freed if an error occurs
  //allocate space for an array of instruction structs & an array of symbol structs
  code = malloc(sizeof(instruction) * MAX_CODE_LENGTH);
  table = malloc(sizeof(table) * MAX_SYMBOL_TABLE_SIZE);


  program();   //begin parsing

	/* this line is EXTREMELY IMPORTANT, you MUST uncomment it
		when you test your code otherwise IT WILL SEGFAULT in 
		vm.o THIS LINE IS HOW THE VM KNOWS WHERE THE CODE ENDS
		WHEN COPYING IT TO THE PAS
	code[cIndex].opcode = -1;
	*/
  code[cIndex].opcode = -1;

  if(printTable == 1)
    printsymboltable();

  if(printCode == 1)
    printassemblycode();

	return code;
}


/* Start the program */
void program()
{
  emit(JMP, 0, 0);   //To reserve space for jumping to 1st inst of main. JMP 0 M

  addToSymbolTable(3, "main", 0, 0, 0, 0);  //add main to symbol table

  level = -1;

  //Get first token
  getNextToken();
  
  block();

  if(token.type != periodsym)
    printparseerror(1); //program must be closed by a period
  
  emit(SIO, 0, HALT);  

  for(int i = 0; i < cIndex; i++)   
  {
    if(code[i].opcode == CAL)
    {
      code[i].m = table[code[i].m].addr;
    }
  }

  //fix first JMP instruction
  code[0].m = table[0].addr;
}


/* BLOCK: const-declaration  var-declaration  procedure-declaration statement. */
void block()
{
  level++; //level 0 first time blocked() it's called
  int procedure_idx = tIndex - 1;

  // printf("First token of the program: %s %d %d\n", token.name, token.value, token.type);
  const_declaration();

  int x; // number of vars declared
  x = var_declaration();

  procedure_declaration();
  table[procedure_idx].addr = cIndex * 3;
  if(level == 0)
    emit(INC, 0, x); //INC 0 M (M = x). Allocate M mem words
  else
    emit(INC, 0, x+3);

  statement();

  mark();

  level--;
}


/* CONST-DECLARATION: ["const" ident "=" number {"," ident "=" number} ";"]. */
void const_declaration()
{
  char ident_name[12];

  if(token.type == constsym)  //const pi := 3.14, e := 2.73, g := 9.8;
  {
    do{
      getNextToken();

      if(token.type != identsym)
        printparseerror(2); // constant declaration error

      // check if the identsym being declared is present and unmark in sym table at the same lex level
      // returns -1 if nothing is found
      symIdx = multiple_declaration_check(token);
      if(symIdx != -1) 
        printparseerror(18); 

      strcpy(ident_name, token.name); // Save ident name 

      getNextToken(); // should be sym :=
      if(token.type != assignsym)
        printparseerror(2); 

      getNextToken(); // should be a number
      if(token.type != numbersym)
        printparseerror(2); 

      // addToSymbolTable(kind=1, name, value, level, address=0, mark=0)
      addToSymbolTable(1, ident_name, token.value, level, 0, 0);

      getNextToken(); // should be commasym for multiple declaration or semicolonsym 

    } while (token.type == commasym);

    if(token.type != semicolonsym)
    {
      if(token.type == identsym)
        printparseerror(13);  //multiple const-declaration must be separated by commas
      else
        printparseerror(14);  //const-declaration should close with ;             
    }

    // if token is semicolonsym, const-declaration was fine, get next token
    getNextToken();  
  }

} // end of const_declaration. Return to block()


/* VAR-DECLARATION: [ "var "ident {"," ident} “;"] */
int var_declaration()
{
  int numVars = 0;

  if(token.type == varsym)
  {
    do {
      numVars++;
      getNextToken();

      if(token.type != identsym)
        printparseerror(3); 

      symIdx = multiple_declaration_check(token);
      if(symIdx != -1)
        printparseerror(18); 

      if(level == 0)
        addToSymbolTable(2, token.name, 0, level, numVars-1, 0); //value = 0; address = numVars-1
      else
        addToSymbolTable(2, token.name, 0, level, numVars+2, 0); //value = 0; address = numVars+2

      getNextToken();

    } while(token.type == commasym);

    if(token.type != semicolonsym)
    {
      if(token.type == identsym)
        printparseerror(13);
      else
        printparseerror(14);
    }

    // if token is semicolonsym, var-declaration was fine, get next token
    getNextToken();
  }

  return numVars;
}


/* PROCEDURE-DECLARATION: { "procedure" ident ";" block ";" } */
void procedure_declaration()
{
  while(token.type == procsym)
  {
    getNextToken(); // expected name (identifier) of the procedure
    if(token.type != identsym)
      printparseerror(4);

    symIdx = multiple_declaration_check(token);
    if(symIdx != -1)
      printparseerror(18);

    //add name of the procedure to the symbol table
    addToSymbolTable(3, token.name, 0, level, 0, 0); //value = 0; address = 0

    getNextToken(); // expected ;

    if(token.type != semicolonsym)
      printparseerror(4);

    // if token is semicolonsym, procedure-declaration was fine, get next token
    getNextToken();

    /*  quirk for begin-end */
    block();

    // if token is not semicolonsym after last statement of begin-end
    // it can be present or absent, neither case should cause error
    if(token.type != semicolonsym) 
      printparseerror(14);

    // if token is semicolonsym, get next token
    // after flow of ctrl returns from block() and semicolon was present, get next token
    getNextToken();

    emit(OPR, 0, RTN); //emit (2, 0, 0)
  }
}


/* STATEMENT */
void statement()
{
  int jpcIdx;
  int jmpIdx;
  int loopIdx;

  //1. identsym
  if(token.type == identsym)
  {
    symIdx = find_symbol(token, 2);

    if(symIdx == -1) 
    {
      if(find_symbol(token, 1) != find_symbol(token, 3))
        printparseerror(6);
      else
        printparseerror(19);
    }
    
    getNextToken(); //:=

    if(token.type != assignsym)
      printparseerror(5);

    getNextToken();
  
    expression();

    emit(STO, (level - table[symIdx].level), table[symIdx].addr);

		return;
  }

  //2. beginsym
  else if(token.type == beginsym)
  {
    do{
      getNextToken();
      statement();

    } while(token.type == semicolonsym);

    if(token.type != endsym)
    {
      if(token.type==identsym || token.type==beginsym || token.type==ifsym || token.type==whilesym || token.type==readsym || token.type==writesym || token.type==callsym) 
      {
        printparseerror(15);
      }
      else
      {
        printparseerror(16);
      }
    }

    getNextToken();

    return;
  }

  //3. ifsym
  else if(token.type == ifsym)
  {
    getNextToken();

    condition();

    jpcIdx = cIndex;

    emit(JPC, 0, 0);

    if(token.type != thensym)
      printparseerror(8);

    getNextToken();

    statement();

    if(token.type == elsesym)
    {
      jmpIdx = cIndex;
      emit(JMP, 0, 0);
      code[jpcIdx].m = cIndex * 3;

      getNextToken();

      statement();
      code[jmpIdx].m = cIndex * 3;
    }
    else
    {
      code[jpcIdx].m = cIndex * 3;
    }

    return;
  }

  //4. whilesym
  else if(token.type == whilesym)
  {
    getNextToken();
    loopIdx = cIndex;

    condition();

    if (token.type != dosym)
      printparseerror(9);

    getNextToken();

    jpcIdx = cIndex;
    emit(JPC, 0, 0);

    statement();

    emit(JMP, 0, loopIdx * 3); //M = loopIdx * 3

    code[jpcIdx].m = cIndex * 3;

    return;
  }

  //5.readsym
  else if(token.type == readsym)
  {
		getNextToken();

		if(token.type != identsym) //missing identifier
			printparseerror(6);

		symIdx = find_symbol(token, 2);

		if(symIdx == -1) //identifier present is not a var
    {
			if(find_symbol(token, 1) != find_symbol(token, 3))
				printparseerror(6);
			else
				printparseerror(19);
    }
		
    getNextToken();
		
    emit(SIO, 0, READ);
    emit(STO, level - table[symIdx].level, table[symIdx].addr);

		return;
  }

  //6. writesym
  else if(token.type == writesym)
  {
    getNextToken();
    expression();
    emit(SIO, 0, WRT);
    return;
  }

  //7. callsym
	else if(token.type == callsym)
  {
		getNextToken();
		symIdx = find_symbol(token, 3);

		if(symIdx == -1) //procedure ident is not present
    {
			if(find_symbol(token, 1) != find_symbol(token, 2))
				printparseerror(7); //ident is present but it is not a procedure
			else
				printparseerror(19);
    }

    getNextToken();
    emit(CAL, level - table[symIdx].level, symIdx);

    //return;
  }

} // end of statement()


/* CONDITION */
void condition()
{
  if(token.type == oddsym)
  {
    getNextToken();
    expression();
    emit(OPR, 0, ODD);
  }
  //if odd was not found
  else 
  {
    expression();

    if(token.type == eqlsym)
    {
      getNextToken();
      expression();
      emit(OPR, 0, EQL);
    }
    else if(token.type == neqsym)
    {
      getNextToken();
      expression();
      emit(OPR, 0, NEQ);
    }
    else if(token.type == lsssym)
    {
      getNextToken();
      expression();
      emit(OPR, 0, LSS);
    }
    else if(token.type == leqsym)
    {
      getNextToken();
      expression();
      emit(OPR, 0, LEQ);
    }
    else if(token.type == gtrsym)
    {
      getNextToken();
      expression();
      emit(OPR, 0, GTR);
    }
    else if(token.type == geqsym)
    {
      getNextToken();
      expression();
      emit(OPR, 0, GEQ);
    }
    //flow of ctrol returned from expression() and curr sym is not a relational operator
    else {
      printparseerror(10); 
    }
  } //else
}


/* EXPRESSION: ["+"|"-"] term { ("+"|"-") term} */
void expression()
{
  if(token.type == subsym)
  {
    getNextToken();
    term();
    emit(OPR, 0, NEG);

    while(token.type == addsym || token.type == subsym)
    {
      if(token.type == addsym) {
        getNextToken();
        term();
        emit(OPR, 0, ADD);
      }
      //token.type==subsym
      else {
        getNextToken();
        term();
        emit(OPR, 0, SUB);
      }
    }
  }
  else {
    if(token.type == addsym)
      getNextToken();

    term();

    while(token.type == addsym || token.type == subsym)
    {
      if(token.type == addsym) {
        getNextToken();
        term();
        emit(OPR, 0, ADD);
      }
      //token.type==subsym
      else {
        getNextToken();
        term();
        emit(OPR, 0, SUB);
      }
    }
  }

  if(token.type==addsym||token.type== subsym||token.type==multsym||token.type==divsym||token.type==modsym||token.type==lparensym||token.type==identsym||token.type==numbersym||token.type==oddsym) {
		printparseerror(17);
  }

}


/* TERM: factor {("*" | "/" | "%") factor} */
void term()
{
  factor();
  
  while(token.type == multsym || token.type == divsym || token.type == modsym)
  {
    if(token.type == multsym)
    {
			getNextToken();
			factor();
			emit(OPR, 0, MUL);
    }
		else if(token.type == divsym)
    {
			getNextToken();
			factor();
			emit(OPR, 0, DIV);
    }
		else
    {
			getNextToken();
			factor();
			emit(OPR, 0, MOD);
    }
  }

}


/* FACTOR: ident | number | "(" expression ")“ | “call” ident [ “(” expression “)” ] */
void factor()
{
  int symIdx_var, symIdx_const;

  if(token.type == identsym)
  {
    symIdx_var = find_symbol(token, 2);   //var
    symIdx_const = find_symbol(token, 1); //const

    if(symIdx_var == -1 && symIdx_const == -1) 
    {
      if(find_symbol(token, 3) != -1) //ident found, but it is a procedure
        printparseerror(11);          
      else                            //undeclared ident
        printparseerror(19);
    }

    //if symIdx_var!=-1 OR symIdx_const!=-1. If ident is a var OR a const
    if(symIdx_var == -1) // const
      emit(LIT, 0, table[symIdx_const].val);  

    //variable
    else if(symIdx_const == -1 || (table[symIdx_var].level > table[symIdx_const].level))
      emit(LOD, (level - table[symIdx_var].level), table[symIdx_var].addr);

    else
      emit(LIT, 0, table[symIdx_const].val);

    getNextToken();
  }

  else if(token.type == numbersym)
  {
    emit(LIT, 0, token.value); //LIT 0, M
    getNextToken();
  }
 
  else if(token.type == lparensym)
  {
    getNextToken();
    expression(); 

    // if flow of ctrol returns from expression() without error
    if(token.type != rparensym)
      printparseerror(12);

    getNextToken();
  }
  //current symbol is neither an identifier, a number, nor a (
  else {
    printparseerror(11);
  }
}



/* Linear pass through the sym table looking for the sym given*/
int multiple_declaration_check(lexeme token)
{
  for(int i = tIndex - 1; i >= 0; i--)
  {
    if(strcmp(table[i].name, token.name) == 0) // find name in sym table
    {
      if(table[i].mark == 0) // if it find an unmark instance
      {
        if(table[i].level == level) {
          return i;
        }
      }
    }
  }

  // if it gets to end of the table
  return -1;  
}

int find_symbol(lexeme token, int kind)
{
  int max_lev = -1;

  for(int i = tIndex - 1; i >=0; i--)
  {
    //ident found in sym table and it is unmark
    if(strcmp(table[i].name, token.name) == 0 && table[i].kind == kind && table[i].mark == 0)
    {
      if(i > max_lev) {
        max_lev = i;
      }
    }
  }
  
  return max_lev;
}

/* Starts at end of table and works backward */
void mark()
{
  for(int i = tIndex - 1; i >= 0; i--)
  {
    if(table[i].mark == 1) // ignore mark entries
      continue;

    else if (table[i].mark == 0 && table[i].level == level) //unmarked entry
    {
      table[i].mark = 1;
    }

    else if(table[i].mark == 0 && table[i].level < level)
      return;
  }

}

// Fetches a token from tokensList and increments tokenIdx
void getNextToken() {
  token = tokensList[tokenIdx++];
}

// Function to add an instruction to the code array
void emit(int opname, int level, int mvalue)
{
	code[cIndex].opcode = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

// Function to add an identifier(external name) to the symbol table
void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].val = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}


void printparseerror(int err_code)
{
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
			break;
		case 3:
			printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
			break;
		case 4:
			printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
			break;
		case 5:
			printf("Parser Error: Variables must be assigned using :=\n");
			break;
		case 6:
			printf("Parser Error: Only variables may be assigned to or read\n");
			break;
		case 7:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 8:
			printf("Parser Error: if must be followed by then\n");
			break;
		case 9:
			printf("Parser Error: while must be followed by do\n");
			break;
		case 10:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 11:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
			break;
		case 12:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 13:
			printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
			break;
		case 14:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 15:
			printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
			break;
		case 16:
			printf("Parser Error: begin must be followed by end\n");
			break;
		case 17:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 18:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 19:
			printf("Parser Error: Undeclared identifier\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}

	free(code);
	free(table);

  // Added to error function to stop execution after printing error type
  exit(0);
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark); 
	
	free(table);
	table = NULL;
}


void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("ODD\t");
						break;
					case 7:
						printf("MOD\t");
						break;
					case 8:
						printf("EQL\t");
						break;
					case 9:
						printf("NEQ\t");
						break;
					case 10:
						printf("LSS\t");
						break;
					case 11:
						printf("LEQ\t");
						break;
					case 12:
						printf("GTR\t");
						break;
					case 13:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				switch (code[i].m)
				{
					case 1:
						printf("WRT\t");
						break;
					case 2:
						printf("RED\t");
						break;
					case 3:
						printf("HAL\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	if (table != NULL)
		free(table);
}