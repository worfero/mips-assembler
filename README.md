# mips-assembler
MIPS assembler under development for learning purposes.

It is capable of compiling any basic instruction from the MIPS instruction set.

It also ignores empty or blank lines of instruction, so if you like to use line breaks in your assembly code, feel free.

## Running
For it to run properly, you must compile mips.c and have an "assembly.asm" file with valid instructions. The program will generate a "machine-code.bin" file containing the binary instructions. Also, the terminal will return the equivalent code in hex.

It does not have error handling yet, so keep in mind your assembly code **must be valid**.