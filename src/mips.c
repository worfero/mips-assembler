#include "../include/mips.h"

// generates an I-TYPE instruction based on input values
unsigned machineCode(Instruction inst){
    unsigned result;
    if(inst.op != 0){
        unsigned op = (unsigned)inst.op << 26; // bit shift for the opcode in the instruction
        unsigned rs = (unsigned)inst.rs << 21; // bit shift for the rs in the instruction
        unsigned rt = (unsigned)inst.rt << 16; // bit shift for the rt in the instruction
        result = op + rs + rt + (unsigned short)inst.imm;
    }
    else if(inst.op == 2 || inst.op == 3){
        unsigned op = (unsigned)inst.op << 26; // bit shift for the opcode in the instruction
        result = op + (unsigned short)inst.imm;
    }
    else{
        unsigned op = (unsigned)inst.op << 26; // bit shift for the opcode in the instruction
        unsigned rs = (unsigned)inst.rs << 21; // bit shift for the rs in the instruction
        unsigned rt = (unsigned)inst.rt << 16; // bit shift for the rt in the instruction
        unsigned rd = (unsigned)inst.rd << 11; // bit shift for the rd in the instruction
        unsigned sa = (unsigned)inst.sa << 6; // bit shift for the sa in the instruction
        result = op + rs + rt + rd + sa + inst.funct;
    }
    return result;
}

void assemble(char *fileName, char *outputDir){
    // number of assembly code lines declared as a variable
    unsigned numberOfLines = 0;

    // reads the assembly code file and store each line of code in a string array, without comments, whitespaces and line breaks
    char **msg = readFile(&numberOfLines, fileName);
    while(msg == NULL){
        msg = readFile(&numberOfLines, fileName);
    }

    // memory allocation for binary instructions
    unsigned *binaryInstructions = (unsigned int*)malloc(numberOfLines*sizeof(unsigned));

    // instruction structure array for instruction parsing
    Instruction *instructions = (Instruction *)malloc(sizeof(Instruction) * numberOfLines);

    // start the parsing routine, from an array of text instructions separated by \n to a Instruction struct array
    parser(msg, instructions, &numberOfLines);

    // free assembly code lines array
    for(unsigned i = 0; i < numberOfLines; i++){
        free(msg[i]);
    }
    free(msg);

    for(unsigned i = 0; i < numberOfLines; i++){
        // generate 16-bit machine code instruction
        unsigned binInst = machineCode(instructions[i]);

        printf("0x%04x\n", binInst);

        // fill machine code array
        binaryInstructions[i] = binInst;
    }
    // free instructions array
    free(instructions);
    // write machine code to file
    writeFile(binaryInstructions, numberOfLines, outputDir);
    // free machine code array
    free(binaryInstructions);
}