#include <stdio.h>
#include <string.h>

#define R_TYPE     0    // R-TYPE instruction
#define I_TYPE     1    // I-TYPE instruction
#define J_TYPE     2    // J-TYPE instruction

typedef struct {
    char mnemonic[5]; // opcode mnemonic
    int instType;
    int numCode;      // opcode number
} Opcode;

static const Opcode opcodes[] =
{
    {"addi", I_TYPE, 8},
    {"lw", I_TYPE, 35}
};

// defines a register structure
typedef struct {
    char mnemonic[4]; // register mnemonic
    unsigned numCode; // register number code
} Reg;

// saves the values of all registers available
static const Reg registers[] =
{
    {"$0", 0},     //the constant value 0
    {"$at", 1},    //assembler temporary
    {"$v0", 2},    //procedure return values
    {"$v1", 3},
    {"$a0", 4},    //procedure arguments
    {"$a1", 5},
    {"$a2", 6},
    {"$a3", 7},
    {"$t0", 8},    //temporary variables
    {"$t1", 9},
    {"$t2", 10},
    {"$t3", 11},
    {"$t4", 12},
    {"$t5", 13},
    {"$t6", 14},
    {"$t7", 15},
    {"$s0", 16},    //saved variables
    {"$s1", 17},
    {"$s2", 18},
    {"$s3", 19},
    {"$s4", 20},
    {"$s5", 21},
    {"$s6", 22},
    {"$s7", 23},
    {"$t8", 24},    //temporary variables
    {"$t9", 25},
    {"$k0", 26},    //operating system (OS) temporaries
    {"$k1", 27},
    {"$gp", 28},    //global pointer
    {"$sp", 29},    //stack pointer
    {"$fp", 30},    //frame pointer
    {"$ra", 31}     //procedure return address
};

// generates a addi instruction based on input values
unsigned generateInstruction(unsigned opField, unsigned rsField, unsigned rtField, unsigned immField){
    unsigned result;
    opField = opField << 26; // bit shift for the opcode in the instruction
    rsField = rsField << 21; // bit shift for the rs in the instruction
    rtField = rtField << 16; // bit shift for the rt in the instruction
    result = opField + rsField + rtField + immField;
    return result;
}

void main() {
    unsigned op;
    unsigned rs;
    unsigned rt;
    unsigned imm;
    char opMne[5];
    char rsMne[4];
    char rtMne[4];

    printf("Please input an instruction: ");
    scanf("%5s", opMne);
    printf("Please input a rs register: ");
    scanf("%4s", rsMne);
    printf("Please input a rt register: ");
    scanf("%4s", rtMne);
    printf("Please input an immediate: ");
    scanf("%u", &imm);

    for(int i = 0; i <= sizeof(opcodes); i++){
        if(!strcmp(opMne, opcodes[i].mnemonic)){
            op = opcodes[i].numCode;
        }
    } 

    for(int i = 0; i <= sizeof(registers); i++){
        if(!strcmp(rsMne, registers[i].mnemonic)){
            rs = registers[i].numCode;
        }
        if(!strcmp(rtMne, registers[i].mnemonic)){
            rt = registers[i].numCode;
        }
    }
    printf("0x%04x", generateInstruction(op, rs, rt, imm));
}