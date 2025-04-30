#include <stdio.h>

unsigned generateInstruction(unsigned opField, unsigned rsField, unsigned rtField, unsigned immField){
    unsigned result;

    opField = opField << 26;
    rsField = rsField << 21;
    rtField = rtField << 16;
    result = opField + rsField + rtField + immField;
    return result;
}

void main() {
    //addi
    unsigned op = 8;
    unsigned rs = 0;
    unsigned rt = 16;
    unsigned imm = 40;
    printf("0x%04x", generateInstruction(op, rs, rt, imm));
}