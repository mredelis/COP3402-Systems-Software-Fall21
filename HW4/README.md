# University of Central Florida
## COP3402: Systems Software, Fall 2021
### Author: Edelis Molina Rios

**Homework #4 - PL/0 Compiler**  


Implement a PL/0 compiler as in HW3, but this time using my own VM (HW1) and Scanner(HW2)

- To compile: gcc driver.c lex.c parser.c vm.c (all files are in the same directory) OR run the Makefile with command "make". Makefile runs the command "gcc driver.c lex.c parser.c vm.c"  
  Please note that compiler.h is not included in the submission as the file was provided   
  
- To execute: ./a.out filename.txt directives where the files in the command line are in the current directory.  
Example command: ./a.out input.txt –l –a –v -s. Print all types of output to the console.  

List of Directives:  

-l: print the list and table of lexemes/tokens (HW2 output)  

-s: print symbol table  

-a: print assembly code (parser output)  

-v: print virtual machine execution trace (HW1 output)
