# Stack Machine Project — L2 MIDO 2022-2023 | Computer Architecture

This project implements a stack-based virtual machine as part of the Computer Architecture course at Université Paris Dauphine - PSL (L2 MIDO).

The goal is to simulate a fictional stack machine. The project consists of two parts:
1. An **assembler** that converts a source program written in assembly language to machine code.
2. A **simulator** that executes the program written in machine code, instruction by instruction.

## Project Members

- Wardakhan Kévin
- Abdelouhab Yacine
- 
## Project Objectives

- **Assembler**
  - Parses an assembly language source file.
  - Detects syntax errors and reports them with precise line numbers.
  - Converts the assembly code into machine code and writes it to `hexa.txt`.

- **Simulator**
  - Reads the machine code from `hexa.txt`.
  - Simulates the execution of the program with proper handling of registers and memory.
  - Implements a stack of size 5000 and operations on PC (Program Counter) and SP (Stack Pointer).
  - Interacts with the user for input/output operations as defined in the assembly instructions.

## Compilation

To compile the assembler and the simulator:

```bash
gcc -Wall -o assembler projet.c
gcc -Wall -o simulator projet2.c
```

## Execution
Step 1: Assemble the source program

```bash
./assembler source_program.txt
```

Step 2: Run the simulator
```bash
./simulator
```
The simulator reads hexa.txt and executes the instructions.

## Project Structure
```bash
.
├── projet.c                 # Assembler source code
├── projet2.c                # Simulator source code
├── hexa.txt                 # Generated machine code (output of assembler)
├── docs/
│   ├── user.pdf             # User documentation 
│   └── dev.pdf              # Developer documentation 
└── README.md                # Project description (this file)
```
Instruction Set
The machine supports the following instructions:

push x : Push value at address x onto the stack.

push# i : Push constant i onto the stack.

ipush : Push the value at the address stored at the top of the stack.

pop x : Pop the top of the stack and store it at address x.

ipop : Pop the value and store it at the address found at the new top of the stack.

dup : Duplicate the top value of the stack.

op i : Perform operation i (arithmetic, logical, comparison).

jmp adr : Jump to address adr.

jpz adr : Pop and, if zero, jump to address adr.

rnd x : Push a random number between 0 and x - 1.

read x : Read an integer from the user and store it at address x.

write x : Print the value at address x.

call adr : Call a subroutine at adr.

ret : Return from subroutine.

halt : Stop the simulation.



