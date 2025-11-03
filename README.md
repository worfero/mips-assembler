# mips-assembler
MIPS assembler under development for learning purposes.

It is capable of compiling any basic instruction from the MIPS instruction set (pseudo-instructions not included).

It also ignores empty or blank lines of instruction, so if you like to use line breaks in your assembly code, feel free.

## Running
For it to run properly, you must have an .asm file with valid assembly code in the src folder. The program will generate a "machine-code.bin" file in the build folder containing the binary instructions. Also, the terminal will return the equivalent code in hex.

I also decided to include an assembly.asm file with testbench instructions for a MIPS processor in the repo so you have a test file already.

It does not have error handling yet, so keep in mind your assembly code **must be valid**.