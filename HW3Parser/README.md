# University of Central Florida
## COP3402: Systems Software, Fall 2021
### @Author: Edelis Molina Rios


Homework #3 - Parser-Code Generator


Implementation of a Recursive Descent Parser and Intermediate Code Generator for the programming language PL/0.


- To compile: gcc driver.c lex.o parser.c vm.o (the files driver.c lex.o parser.c and vm.o are in the current directory) OR run the Makefile with command "make". Makefile runs the command "gcc driver.c lex.o parser.c vm.o"  
  

- To execute: ./a.out filename.txt directives where the files in the command line are in the current directory. Example command: ./a.out input.txt –l –a –v
Print all types of output to the console.

                            
- To run shell script for testing: ./name.sh (in this case ./tester.sh)