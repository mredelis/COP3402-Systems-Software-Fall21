# University of Central Florida
## COP3402: Systems Software, Fall 2021
### Author: Edelis Molina Rios

**Compiler Programming Project**

In COP3402, it was required to implement a virtual machine and developing a small compiler that generates code for the virtual machine. The project is divided into 4 steps.

1. HW1: Implement a virtual machine
2. HW2: Implement a scanner (lexical analizer)
3. HW3: Implement a parser/code generator for tiny language (PL/0) 
4. HW4: Implement some modifications to the VM.
- Modify the CAL and RTN instructions for an Activation Record with FV, SL, DL, RA. In HW1 an Activation Record had only SL, DL, RA
- Modify the grammar as shown below

| Old Symbol HW3 | New Symbol HW4 |
|----------------|----------------|
| begin          | do             |
| end            | od             |
| if             | when           |
| then           | do             |
| else           | elsedo         |
| == eqlsym      | =              |

Each project with the instructions has its own directory at the root of the repository 
