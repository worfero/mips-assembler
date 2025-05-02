#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define R_TYPE     0    // R-TYPE instruction
#define I_TYPE     1    // I-TYPE instruction
#define J_TYPE     2    // J-TYPE instruction

typedef struct {
    char mnemonic[5]; // mnemonic
    int instType;
    unsigned numCode; // opcode number
} DataStructure;

static const DataStructure opcodes[] =
{
    {"addi", I_TYPE, 8},
    {"lw", I_TYPE, 35}
};

// saves the values of all registers available
static const DataStructure registers[] =
{
    {"$0", 0, 0},     //the constant value 0
    {"$at", 0, 1},    //assembler temporary
    {"$v0", 0, 2},    //procedure return values
    {"$v1", 0, 3},
    {"$a0", 0, 4},    //procedure arguments
    {"$a1", 0, 5},
    {"$a2", 0, 6},
    {"$a3", 0, 7},
    {"$t0", 0, 8},    //temporary variables
    {"$t1", 0, 9},
    {"$t2", 0, 10},
    {"$t3", 0, 11},
    {"$t4", 0, 12},
    {"$t5", 0, 13},
    {"$t6", 0, 14},
    {"$t7", 0, 15},
    {"$s0", 0, 16},    //saved variables
    {"$s1", 0, 17},
    {"$s2", 0, 18},
    {"$s3", 0, 19},
    {"$s4", 0, 20},
    {"$s5", 0, 21},
    {"$s6", 0, 22},
    {"$s7", 0, 23},
    {"$t8", 0, 24},    //temporary variables
    {"$t9", 0, 25},
    {"$k0", 0, 26},    //operating system (OS) temporaries
    {"$k1", 0, 27},
    {"$gp", 0, 28},    //global pointer
    {"$sp", 0, 29},    //stack pointer
    {"$fp", 0, 30},    //frame pointer
    {"$ra", 0, 31}     //procedure return address
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

int main() {
    unsigned op = 0;
    unsigned rs = 0;
    unsigned rt = 0;
    unsigned imm = 0;
    int argCounter = 0;
    char opMsg[30] = {"\0"};
    char rsMsg[30] = {"\0"};
    char rtMsg[30] = {"\0"};
    char immMsg[30] = {"\0"};

    char msg[120];

    printf("Please insert an instruction: ");
    fgets(msg, sizeof(msg), stdin);
    for(int i = 0; i < strlen(msg); i++){
        if(msg[i] == ' '){
            if(opMsg[0] == '\0'){
                strncpy(opMsg, msg, i);
                argCounter += strlen(opMsg) + 1;
            }
            else if(rtMsg[0] == '\0'){
                strncpy(rtMsg, msg + argCounter, i - argCounter);
                argCounter += strlen(rtMsg) + 1;
            }
            else if(rsMsg[0] == '\0'){
                strncpy(rsMsg, msg + argCounter, i - argCounter);
                argCounter += strlen(rsMsg) + 1;
                if(immMsg[0] == '\0'){
                    strncpy(immMsg, msg + argCounter, i+3 - argCounter);
                    argCounter = 0;
                }
            }
        }
    }

    for(int i = 0; i <= sizeof(opcodes)/sizeof(opcodes[0]); i++){
        if(!strcmp(opMsg, opcodes[i].mnemonic)){
            op = opcodes[i].numCode;
        }
    } 
    
    for(int i = 0; i <= sizeof(registers)/sizeof(registers[0]); i++){
        if(!strcmp(rtMsg, registers[i].mnemonic)){
            rt = registers[i].numCode;
        }
        if(!strcmp(rsMsg, registers[i].mnemonic)){
            rs = registers[i].numCode;
        }
    }
    imm = strtol(immMsg, (char **)NULL, 10);
    printf("0x%04x", generateInstruction(op, rs, rt, imm));
}