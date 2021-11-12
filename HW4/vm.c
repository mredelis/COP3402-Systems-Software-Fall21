/*=============================================================================
| Assignment: HW01 – Virtual Machine
| Author: Edelis Molina
+=============================================================================*/

#include "compiler.h"

#define MAX_PAS_LENGTH 500

/* Global variables: Guaranteed to be initialized to zero */

// Registers
int IC;    // Instruction Counter. Initial Value is 0
int GP;    // Global Pointer. Points to DATA segment
int BP;    // Base Pointer
int SP;    // Stack Pointer
int PC;    // Program Counter
int DP;    // Global Data Pointer
int FREE;  // Heap Pointer
int IR[3]; // Instruction Register. IR[0] = opcode; IR[1] = L; IR[2] = M

int PAS[MAX_PAS_LENGTH]; // Process Address Space


void	execute_program(instruction* code, int outputs)
{
  int halt = 1;  // It's set to 0 at the end of the program

  // Helper variables
  int OP, L, M;  
  char opname[5];

  // Load the program instructions (code) into PAS
  int cIndex = 0;
  while(code[cIndex].opcode != -1)
  {
    PAS[IC++] = code[cIndex].opcode;
    PAS[IC++] = code[cIndex].l;
    PAS[IC++] = code[cIndex].m;
    cIndex++;
  }

  // Initialize all others CPU registers after the program was uploaded into PAS
  GP = IC;
  DP = IC - 1;
  FREE = IC + 40;
  BP = IC;
  PC = 0;
  SP = MAX_PAS_LENGTH;

  // Print column headers and initial values of each register to the Terminal
  if(outputs == 1)
  {
    printf("\t\t\t\t%s\t%s\t%s\t%s\t%s", "PC", "BP", "SP", "DP", "data\n");
    printf("Initial values:\t\t\t%d\t%d\t%d\t%d\n", PC, BP, SP, DP);
  }


  while(halt == 1)
  {
    // Save previous PC to print line number
    int oldPC = PC;

    // Fetch Cycle. Place instruction from TEXT segment into IR and increment PC by 3
    OP = IR[0] = PAS[PC];     // opcode
    L = IR[1] = PAS[PC + 1];  // L
    M = IR[2] = PAS[PC + 2];  // M
    PC += 3;

    // Execute Cycle   
    switch(OP) 
    {
      case 1:
        strcpy(opname, "LIT");
        if(BP == GP) {
          DP = DP + 1;
          PAS[DP] = IR[2];
        }
        else {
          SP = SP - 1;
          PAS[SP] = IR[2];
        }
        break; 

      case 2:
        switch(M)
        {
          case 0:
            strcpy(opname, "RTN");  // Return
            SP = BP + 1;
            BP = PAS[SP - 3];
            PC = PAS[SP - 4];
            break;

          case 1:
            strcpy(opname, "NEG");
            if(BP == GP)
              PAS[DP] = -1 * PAS[DP];
            else
              PAS[SP] = -1 * PAS[SP];
            break;

          case 2:
            strcpy(opname, "ADD");
            if(BP == GP) {
              DP -= 1;
              PAS[DP] = PAS[DP] + PAS[DP + 1];
            }
            else {
              SP += 1;
              PAS[SP] = PAS[SP] + PAS[SP - 1];
            }
            break;

          case 3:
            strcpy(opname, "SUB");
            if(BP == GP) {
              DP -= 1;
              PAS[DP] = PAS[DP] - PAS[DP + 1];
            }
            else {
              SP += 1;
              PAS[SP] = PAS[SP] - PAS[SP - 1];
            }
            break;

          case 4:
            strcpy(opname, "MUL");
            if(BP == GP) {
              DP -= 1;
              PAS[DP] = PAS[DP] * PAS[DP + 1];
            }
            else {
              SP += 1;
              PAS[SP] = PAS[SP] * PAS[SP - 1];
            }
            break;

          case 5:
            strcpy(opname, "DIV");
            if(BP == GP) {
              DP -= 1;
              PAS[DP] = PAS[DP] / PAS[DP + 1];
            }
            else {
              SP += 1;
              PAS[SP] = PAS[SP] / PAS[SP - 1];
            }
            break;

          case 6:
            strcpy(opname, "ODD");
            if(BP == GP)
              PAS[DP] = PAS[DP] % 2;
            else
              PAS[SP] = PAS[SP] % 2;
            break;

          case 7: 
            strcpy(opname, "MOD");
            if(BP == GP) {
              DP -= 1;
              PAS[DP] = PAS[DP] % PAS[DP + 1];
            }
            else {
              SP += 1;
              PAS[SP] = PAS[SP] % PAS[SP - 1];
            }
            break;  

          case 8:
            strcpy(opname, "EQL");
            if(BP == GP) {
              DP -= 1;
              PAS[DP] = (PAS[DP] == PAS[DP + 1]) ? 1 : 0;
            }
            else {
              SP += 1;
              PAS[SP] = (PAS[SP] == PAS[SP - 1]) ? 1 : 0;
            }
            break;

          case 9:
            strcpy(opname, "NEQ");
            if(BP == GP) {
              DP -= 1;
              PAS[DP] = (PAS[DP] != PAS[DP + 1]) ? 1 : 0;
            }
            else {
              SP += 1;
              PAS[SP] = (PAS[SP] != PAS[SP - 1]) ? 1 : 0;
            }
            break;

          case 10:
            strcpy(opname, "LSS");
            if(BP == GP) {
              DP -= 1;
              PAS[DP] = (PAS[DP] < PAS[DP + 1]) ? 1 : 0;
            }
            else {
              SP += 1;
              PAS[SP] = (PAS[SP] < PAS[SP - 1]) ? 1 : 0;
            }
            break;

          case 11:
            strcpy(opname, "LEQ");
            if(BP == GP) {
              DP -= 1;
              PAS[DP] = (PAS[DP] <= PAS[DP + 1]) ? 1 : 0;
            }
            else {
              SP += 1;
              PAS[SP] = (PAS[SP] <= PAS[SP - 1]) ? 1 : 0;
            }
            break;

          case 12:
            strcpy(opname, "GTR");
            if(BP == GP) {
              DP -= 1;
              PAS[DP] = (PAS[DP] > PAS[DP + 1]) ? 1 : 0;
            }
            else {
              SP += 1;
              PAS[SP] = (PAS[SP] > PAS[SP - 1]) ? 1 : 0;
            }
            break;

          case 13:
            strcpy(opname, "GEQ");
            if(BP == GP) {
              DP -= 1;
              PAS[DP] = (PAS[DP] >= PAS[DP + 1]) ? 1 : 0;
            }
            else {
              SP += 1;
              PAS[SP] = (PAS[SP] >= PAS[SP - 1]) ? 1 : 0;
            }
            break;

          default:
            printf("Value of M is out of the range 0 <= # <= 13");
        }
        break; // Case 2 of opcode

      case 3:
        strcpy(opname, "LOD");
        if (BP  == GP) {
          DP += 1;
          PAS[DP] = PAS[GP + M];
        }
        else {
          if(base(L) == GP) {
            SP -= 1;
            PAS[SP] = PAS[GP + M];
          }
          else {
            SP -= 1;
            PAS[SP] = PAS[base(L) - M];
          }
        }
        break;

      case 4:
        strcpy(opname, "STO");
        if(BP == GP) {
          PAS[GP + M] = PAS[DP];
          DP -= 1;
        }
        else {
          if(base(L) == GP) {
            PAS[GP + M] = PAS[SP];
            SP += 1;
          } 
          else {
            PAS[base(L) - M] = PAS[SP];
            SP += 1;
          }
        }
        break;

      case 5:
        strcpy(opname, "CAL");
        PAS[SP - 1] = 0;        // functional value (FV)
        PAS[SP - 2] = base(L); 	// static link (SL)
        PAS[SP - 3] = BP;		    // dynamic link (DL)
        PAS[SP - 4] = PC;	 		  // return address (RA)
        BP = SP - 2;
        PC = M;
        break;

      case 6:
        strcpy(opname, "INC");
        if(BP == GP)
          DP = DP + M;
        else
          SP = SP - M;
        break;
        
      case 7: 
        strcpy(opname, "JMP");
        PC = M;
        break;

      case 8:
        strcpy(opname, "JPC");
        if(BP == GP) {
          if(PAS[DP] == 0) {
            PC = M;
          }
          DP = DP -1;  
        }
        else {
          if(PAS[SP] == 0) {
            PC = M;
          }
          SP = SP + 1; 
        }
        break;

      case 9:
        strcpy(opname, "SYS");
        switch(M) 
        {
          case 1: // SYS 0, 1    
            if(BP == GP) {
              printf("Top of Stack Value: %d\n", PAS[DP]);
              DP = DP - 1; 
            }
            else {
              printf("Top of Stack Value: %d\n", PAS[SP]);
              SP = SP + 1; 
            }        
            break;

          case 2: // SYS 0, 2
            if(BP == GP) {
              DP = DP + 1; 
              printf("Please Enter an Integer: ");
              scanf("%d", &PAS[DP]);
            }
            else {
              SP = SP - 1;
              printf("Please Enter an Integer: ");
              scanf("%d", &PAS[SP]);
            } 
            break;

          case 3: // SYS 0, 3
            halt = 0; // End program
            break;

          default:
            printf("Value of M for instruction SYS is not valid. Allowed values are 1, 2, 3\n");
        }
        break; // For case 9

      default:
        printf("Value of OP is not valid\n");

    } //End of switch for OP

    if(outputs == 1)
      print_execution(oldPC/3, opname, IR, PC, BP, SP, DP, PAS, GP);

  } //End of while loop
 
}


// This function will be helpful to find a variable in a different AR some L levels down:
/* Find base L levels down */ 
int base(int L)
{
	int arb = BP;	     // arb = activation record base
	while ( L > 0)     // find base L levels down
	{
		arb = PAS[arb];
		L--;
	}
	return arb;
}


/* Function for printing */
void print_execution(int line, char *opname, int *IR, int PC, int BP, int SP, int DP, int *pas, int GP)
{
	int i;
	// print out instruction and registers
	printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t", line, opname, IR[1], IR[2], PC, BP, SP, DP);
	
	// print data section
	for (i = GP; i <= DP; i++)
    printf("%d ", pas[i]);
	printf("\n");
	
	// print stack
	printf("\tstack : ");
	for (i = MAX_PAS_LENGTH - 1; i >= SP; i--)
    printf("%d ", pas[i]);
	printf("\n");
}
