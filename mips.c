#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define R_TYPE          1        // R-TYPE instruction
#define I_TYPE          2        // I-TYPE instruction
#define J_TYPE          3        // J-TYPE instruction
#define N_A             0       // field not applicable
#define INPUT_FIELD     -1       // input field

#define MAX_REG_NUM 31      // Maximum number of registers available
#define MAX_OPCODE_NUM 56   // Maximum number of registers available

// defining instruction code bit fields
typedef struct {
    char mnemonic[7]; // mnemonic
    int instType; // instruction type
    int numCode; // opcode number
    int rdField; // rd register
    int rsField; // rs register
    int rtField; // rt register
    int immField; // immediate
    int saField; // shamt
    int functField; // function code
} Opcode;

static const Opcode opcodes[] =
{
    {"bgez", I_TYPE, 1, N_A, INPUT_FIELD, 1, INPUT_FIELD, N_A, N_A},
    {"beq", I_TYPE, 4, N_A, INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A, N_A},
    {"addi", I_TYPE, 8, N_A, INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A, N_A},
    {"lw", I_TYPE, 35, N_A, INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A, N_A},
    {"lui", I_TYPE, 15, N_A, N_A, INPUT_FIELD, INPUT_FIELD, N_A, N_A}
};

// defining register structure
typedef struct {
    char mnemonic[5]; // mnemonic
    unsigned numCode; // opcode number
} Register;

// saves the values of all registers available
static const Register registers[] =
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

char * readFile() {
    char *text = malloc(20);
    FILE *file;

    file = fopen("assembly.asm", "r");
    if (file == NULL) {
        perror("Error opening file");
        return '\0';
    }

    fgets(text, 20, file);
    fclose(file);

    return text;
}

void getDefaultParams(int *op, int *type, int *rd, int *rs, int *rt, int *imm, int *sa, int *funct, char *opMsg){
    // searches for the opcode in the lookup table
    for(int i = 0; i <= sizeof(opcodes)/sizeof(opcodes[0]); i++){
        if(!strcmp(opMsg, opcodes[i].mnemonic)){
            // gets the instruction code default parameters
            *op = opcodes[i].numCode;
            *type = opcodes[i].instType;
            *rd = opcodes[i].rdField;
            *rs = opcodes[i].rsField;
            *rt = opcodes[i].rtField;
            *imm = opcodes[i].immField;
            *sa = opcodes[i].saField;
            *funct = opcodes[i].functField;
        }
    }
}

void iTypeParsing(char *msg, int op, int rt, int rs, int imm, char *rtMsg, char *rsMsg, char *immMsg){
    // in this range of opcodes, the instruction follows always the following pattern: "mnemonic rs, rt, label" ps: label is an immediate
    if(op < 8){
        char discard[20];
        // for some instructions, rt is a fixed value
        if(rs == INPUT_FIELD && rsMsg[0] == '\0'){
            if(rt == INPUT_FIELD && rtMsg[0] == '\0'){
                sscanf(msg, "%s %[^,], %[^,], %s", discard, rsMsg, rtMsg, immMsg);
            }
            else{
                sscanf(msg, "%s %[^,], %s", discard, rsMsg, immMsg);
            }
        }
    }
    // in this range of opcodes, the instruction follows always the following pattern: "mnemonic rt, rs, immediate"
    else if(op < 32){
        char discard[20];
        // for the "lui" instruction, rs is always 0
        if(rt == INPUT_FIELD && rtMsg[0] == '\0'){
            if(rs == INPUT_FIELD && rsMsg[0] == '\0'){
                sscanf(msg, "%s %[^,], %[^,], %s", discard, rtMsg, rsMsg, immMsg);
            }
            else{
                sscanf(msg, "%s %[^,], %s", discard, rtMsg, immMsg);
            }
        }
    }
    // in this range of opcodes, the instruction follows always the following pattern: "mnemonic rt, immediate(rs)"
    else if(op >= 32){
        char discard[20];
        sscanf(msg, "%s %[^,], %[^(]( %[^)])", discard, rtMsg, immMsg, rsMsg);
    }
}

// generates an I-TYPE instruction based on input values
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

    // declaring instruction variables
    int type;
    int op;
    int rd;
    int rs;
    int rt;
    int imm = 0;
    int sa;
    int funct;
    int label;
    char opMsg[30] = {"\0"};
    char rsMsg[30] = {"\0"};
    char rtMsg[30] = {"\0"};
    char immMsg[30] = {"\0"};

    // reads the assembly code file
    char *msg = readFile();

    // gets opcode mnemonic
    sscanf(msg, "%s ", opMsg);

    getDefaultParams(&op, &type, &rd, &rs, &rt, &imm, &sa, &funct, opMsg);

    if(type == I_TYPE){
        iTypeParsing(msg, op, rt, rs, imm, rtMsg, rsMsg, immMsg);
    }

    free(msg);
    
    for(int i = 0; i <= sizeof(registers)/sizeof(registers[0]); i++){
        if(!strcmp(rtMsg, registers[i].mnemonic) && rt == INPUT_FIELD){
            rt = registers[i].numCode;
        }
        if(!strcmp(rsMsg, registers[i].mnemonic) && rs == INPUT_FIELD){
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