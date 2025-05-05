#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define NOT_INST   0        // Not an instruction
#define R_TYPE     1        // R-TYPE instruction
#define I_TYPE     2        // I-TYPE instruction
#define J_TYPE     3        // J-TYPE instruction

#define MAX_REG_NUM 31      // Maximum number of registers available
#define MAX_OPCODE_NUM 56   // Maximum number of registers available

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
    {"$0", NOT_INST, 0},     //the constant value 0
    {"$at", NOT_INST, 1},    //assembler temporary
    {"$v0", NOT_INST, 2},    //procedure return values
    {"$v1", NOT_INST, 3},
    {"$a0", NOT_INST, 4},    //procedure arguments
    {"$a1", NOT_INST, 5},
    {"$a2", NOT_INST, 6},
    {"$a3", NOT_INST, 7},
    {"$t0", NOT_INST, 8},    //temporary variables
    {"$t1", NOT_INST, 9},
    {"$t2", NOT_INST, 10},
    {"$t3", NOT_INST, 11},
    {"$t4", NOT_INST, 12},
    {"$t5", NOT_INST, 13},
    {"$t6", NOT_INST, 14},
    {"$t7", NOT_INST, 15},
    {"$s0", NOT_INST, 16},    //saved variables
    {"$s1", NOT_INST, 17},
    {"$s2", NOT_INST, 18},
    {"$s3", NOT_INST, 19},
    {"$s4", NOT_INST, 20},
    {"$s5", NOT_INST, 21},
    {"$s6", NOT_INST, 22},
    {"$s7", NOT_INST, 23},
    {"$t8", NOT_INST, 24},    //temporary variables
    {"$t9", NOT_INST, 25},
    {"$k0", NOT_INST, 26},    //operating system (OS) temporaries
    {"$k1", NOT_INST, 27},
    {"$gp", NOT_INST, 28},    //global pointer
    {"$sp", NOT_INST, 29},    //stack pointer
    {"$fp", NOT_INST, 30},    //frame pointer
    {"$ra", NOT_INST, 31}     //procedure return address
};

char * readFile() {
    char *text = malloc(20);
    FILE *file;

    file = fopen("assembly.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return '\0';
    }

    fgets(text, 20, file);
    fclose(file);

    return text;
}

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
    char zero[] = {'0'};
    bool isError = false;

    unsigned op = MAX_OPCODE_NUM + 1;
    unsigned rs = MAX_REG_NUM + 1;
    unsigned rt = MAX_REG_NUM + 1;
    unsigned imm = 0;
    int argCounter = 0;
    char opMsg[30] = {"\0"};
    char rsMsg[30] = {"\0"};
    char rtMsg[30] = {"\0"};
    char immMsg[30] = {"\0"};

    char *msg = readFile();

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
                    strncpy(immMsg, msg + argCounter, strlen(msg) - argCounter);
                    argCounter = 0;
                }
            }
        }
    }

    free(msg);

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

    if(op < 0 || op > MAX_OPCODE_NUM){
        printf("\nERROR: Instruction \"%.20s\" not found. Try again\n", opMsg);
        isError = true;
    }
    if(rt < 0 || rt > MAX_REG_NUM){
        printf("\nERROR: Register \"%.20s\" not found. Try again\n", rtMsg);
        isError = true;
    }
    if(rs < 0 || rs > MAX_REG_NUM){
        printf("\nERROR: Register \"%.20s\" not found. Try again\n", rsMsg);
        isError = true;
    }
    if((strcmp(immMsg, zero)) && imm == 0){
        printf("\nERROR: Immediate \"%.20s\" not valid. Try Again\n", immMsg);
        isError = true;
    }

    if(!isError){
        printf("0x%04x", generateInstruction(op, rs, rt, imm));
    }
}