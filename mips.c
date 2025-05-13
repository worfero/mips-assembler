#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define R_TYPE          1        // R-TYPE instruction
#define I_TYPE          2        // I-TYPE instruction
#define J_TYPE          3        // J-TYPE instruction
#define N_A             0        // field not applicable
#define INPUT_FIELD     -1       // input field

#define MAX_REG_NUM     31       // Maximum number of registers available
#define MAX_OPCODE_NUM  56       // Maximum number of registers available

#define BUF_SIZE_FILE   65536    // Maximum buffer for a file
#define BUF_SIZE_LINE   20       // Maximum buffer for a line

typedef struct {
    // declaring instruction variables
    int type;
    int op;
    int rd;
    int rs;
    int rt;
    int imm;
    int sa;
    int funct;
    int label;
    char field1[30];
} instLine;

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

// lookup table for opcodes
static const Opcode opcodes[] =
{
    {"sll"     , R_TYPE, 0 , INPUT_FIELD, 0          , INPUT_FIELD, N_A        , INPUT_FIELD, 0  },
    {"srl"     , R_TYPE, 0 , INPUT_FIELD, 0          , INPUT_FIELD, N_A        , INPUT_FIELD, 2  },
    {"sra"     , R_TYPE, 0 , INPUT_FIELD, 0          , INPUT_FIELD, N_A        , INPUT_FIELD, 3  },
    {"sllv"    , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 4  },
    {"srlv"    , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 6  },
    {"srav"    , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 7  },
    {"jr"      , R_TYPE, 0 , 0          , INPUT_FIELD, 0          , N_A        , 0          , 8  },
    {"jalr"    , R_TYPE, 0 , 31         , INPUT_FIELD, 0          , N_A        , 0          , 9  },
    {"syscall" , R_TYPE, 0 , 0          , 0          , 0          , N_A        , 0          , 12 },
    {"break"   , R_TYPE, 0 , 0          , 0          , 0          , N_A        , 0          , 13 },
    {"mfhi"    , R_TYPE, 0 , INPUT_FIELD, 0          , 0          , N_A        , 0          , 16 },
    {"mthi"    , R_TYPE, 0 , 0          , INPUT_FIELD, 0          , N_A        , 0          , 17 },
    {"mflo"    , R_TYPE, 0 , INPUT_FIELD, 0          , 0          , N_A        , 0          , 18 },
    {"mtlo"    , R_TYPE, 0 , 0          , INPUT_FIELD, 0          , N_A        , 0          , 19 },
    {"mult"    , R_TYPE, 0 , 0          , INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 24 },
    {"multu"   , R_TYPE, 0 , 0          , INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 25 },
    {"div"     , R_TYPE, 0 , 0          , INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 26 },
    {"divu"    , R_TYPE, 0 , 0          , INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 27 },
    {"add"     , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 32 },
    {"addu"    , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 33 },
    {"sub"     , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 34 },
    {"subu"    , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 35 },
    {"and"     , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 36 },
    {"or"      , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 37 },
    {"xor"     , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 38 },
    {"nor"     , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 39 },
    {"slt"     , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 42 },
    {"sltu"    , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 43 },
    {"bgez"    , I_TYPE, 1 , N_A        , INPUT_FIELD, 1          , INPUT_FIELD, N_A        , N_A},
    {"bltz"    , I_TYPE, 1 , N_A        , INPUT_FIELD, 0          , INPUT_FIELD, N_A        , N_A},
    {"j"       , J_TYPE, 2 , N_A        , N_A        , N_A        , INPUT_FIELD, N_A        , N_A},
    {"jal"     , J_TYPE, 3 , N_A        , N_A        , N_A        , INPUT_FIELD, N_A        , N_A},
    {"bne"     , I_TYPE, 5 , N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A},
    {"blez"    , I_TYPE, 6 , N_A        , INPUT_FIELD, 0          , INPUT_FIELD, N_A        , N_A},
    {"bgtz"    , I_TYPE, 7 , N_A        , INPUT_FIELD, 0          , INPUT_FIELD, N_A        , N_A},
    {"beq"     , I_TYPE, 4 , N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A},
    {"addi"    , I_TYPE, 8 , N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A},
    {"addiu"   , I_TYPE, 9 , N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A},
    {"slti"    , I_TYPE, 10, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A},
    {"sltiu"   , I_TYPE, 11, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A},
    {"andi"    , I_TYPE, 12, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A},
    {"ori"     , I_TYPE, 13, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A},
    {"xori"    , I_TYPE, 14, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A},
    {"lui"     , I_TYPE, 15, N_A        , 0          , INPUT_FIELD, INPUT_FIELD, N_A        , N_A},
    {"lb"      , I_TYPE, 32, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A},
    {"lh"      , I_TYPE, 33, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A},
    {"lw"      , I_TYPE, 35, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A},
    {"lbu"     , I_TYPE, 36, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A},
    {"lhu"     , I_TYPE, 37, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A},
    {"sb"      , I_TYPE, 40, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A},
    {"sh"      , I_TYPE, 41, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A},
    {"sw"      , I_TYPE, 43, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A},
    {"lwcl"    , I_TYPE, 49, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A},
    {"swcl"    , I_TYPE, 56, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A}
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

int countLines(FILE* file)
{
    char buf[BUF_SIZE_FILE];
    int counter = 0;
    for(;;)
    {
        size_t res = fread(buf, 1, BUF_SIZE_FILE, file);
        if(ferror(file))
            return -1;

        int i;
        for(i = 0; i < res; i++)
            if (buf[i] == '\n')
                counter++;

        if(feof(file))
            break;
    }
    counter++;
    return counter;
}

char ** readFile(int *numberOfLines) {
    FILE *file;
    
    file = fopen("assembly.asm", "r");
    if(file == NULL) {
        perror("Error opening file");
        return '\0';
    }

    *numberOfLines = countLines(file);
    
    rewind(file);

    char **lines = malloc(*numberOfLines * sizeof(char**));

    for(int i = 0; i < *numberOfLines; i++){
        lines[i] = (char *)malloc((BUF_SIZE_LINE+1) * sizeof(char));
    }

    for(int i = 0; i < *numberOfLines; i++){
        fgets(lines[i], 20, file);
        // removes newline from the string
        lines[i][strcspn(lines[i], "\n")] = 0;
    }
    fclose(file);

    return lines;
}

int getRegister(char *regMne){
    int reg;
    for(int i = 0; i <= sizeof(registers)/sizeof(registers[0]); i++){
        if(!strcmp(regMne, registers[i].mnemonic)){
            reg = registers[i].numCode;
        }
    }
    return reg;
}

void getDefaultParams(int *op, int *type, int *rd, int *rs, int *rt, int *imm, int *sa, int *funct, char *field1){
    // searches for the opcode in the lookup table
    for(int i = 0; i <= sizeof(opcodes)/sizeof(opcodes[0]); i++){
        if(!strcmp(field1, opcodes[i].mnemonic)){
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

void rTypeParsing(char *msg, int *rd, int *rs, int *rt, int *sa, int funct){
    char field1[20];
    char field2[20];
    char field3[20];
    char field4[20];

    // in this range of opcodes, the instruction follows always the following pattern: "mnemonic rd, rt, shamt"
    if(funct <= 3){
        sscanf(msg, "%s %[^,], %[^,], %d", field1, field2, field3, sa);
        *rt = getRegister(field3);
        *rd = getRegister(field2);
    }
    // in this range of opcodes, the instruction follows always the following pattern: "mnemonic rd, rt, rs"
    else if(funct <= 7){
        sscanf(msg, "%s %[^,], %[^,], %s", field1, field2, field3, field4);
        *rd = getRegister(field2);
        *rt = getRegister(field3);
        *rs = getRegister(field4);
    }
    else if(funct == 12 || funct == 13){
        //code
    }
    // in this range of opcodes, the instruction follows always the following patterns: "mnemonic rd, rs, rt"
    else if(funct >= 8){
        // for some instructions, rd is a fixed value
        if(*rd == INPUT_FIELD){
            // for some instructions with variable rd, rs has a fixed value. If rs is variable, rt is also variable
            if(*rs == INPUT_FIELD){
                sscanf(msg, "%s %[^,], %[^,], %s", field1, field2, field3, field4);
                *rs = getRegister(field3);
                *rt = getRegister(field4);
                *rd = getRegister(field2);
            }
            // if rd is variable and rs is fixed, rt is also fixed
            else{
                sscanf(msg, "%s %s", field1, field2);
                *rd = getRegister(field2);
            }
        }
        // if rd is fixed and rt is variable, rs is always variable
        else if(*rt == INPUT_FIELD){
            sscanf(msg, "%s %[^,], %s", field1, field2, field3);
            *rs = getRegister(field2);
            *rt = getRegister(field3);
        }
        // if rd and rt are fixed, rs is the only parameter
        else if(*rs == INPUT_FIELD){
            sscanf(msg, "%s %s", field1, field2, field3);
            *rs = getRegister(field2);
        }
    }
}

void iTypeParsing(char *msg, int op, int *rt, int *rs, int *imm){
    char field1[20];
    char field2[20];
    char field3[20];

    // in this range of opcodes, the instruction follows always the following pattern: "mnemonic rs, rt, label" ps: label is an immediate
    if(op < 8){
        // for some instructions, rt is a fixed value
        if(*rs == INPUT_FIELD){
            if(*rt == INPUT_FIELD){
                sscanf(msg, "%s %[^,], %[^,], %d", field1, field2, field3, imm);
                *rt = getRegister(field3);
                *rs = getRegister(field2);
            }
            else{
                sscanf(msg, "%s %[^,], %d", field1, field2, imm);
                *rs = getRegister(field2);
            }
        }
    }
    // in this range of opcodes, the instruction follows always the following pattern: "mnemonic rt, rs, immediate"
    else if(op < 32){
        // for the "lui" instruction, rs is always 0
        if(*rt == INPUT_FIELD){
            if(*rs == INPUT_FIELD){
                sscanf(msg, "%s %[^,], %[^,], %d", field1, field2, field3, imm);
                *rt = getRegister(field2);
                *rs = getRegister(field3);
            }
            else{
                sscanf(msg, "%s %[^,], %d", field1, field2, imm);
                *rt = getRegister(field2);
            }
        }
    }
    // in this range of opcodes, the instruction follows always the following pattern: "mnemonic rt, immediate(rs)"
    else if(op >= 32){
        sscanf(msg, "%s %[^,], %d(%[^)])", field1, field2, imm, field3);
        *rt = getRegister(field2);
        *rs = getRegister(field3);
    }
}

void jTypeParsing(char *msg, int op, int *offset){
    char field1[20];
    sscanf(msg, "%s %d", field1, offset);
}

instructionParsing(int numberOfLines, char *msg, instLine *cur_line){
    // gets opcode mnemonics
    sscanf(msg, "%s ", cur_line->field1);

    // gets default parameters for the opcode using the lookup table
    getDefaultParams(&cur_line->op, &cur_line->type, &cur_line->rd, &cur_line->rs, 
                        &cur_line->rt, &cur_line->imm, &cur_line->sa, &cur_line->funct, cur_line->field1);
    switch(cur_line->type){
        case R_TYPE:
            rTypeParsing(msg, &cur_line->rd, &cur_line->rs, &cur_line->rt, &cur_line->sa, cur_line->funct);
            break;
        case I_TYPE:
            iTypeParsing(msg, cur_line->op, &cur_line->rt, &cur_line->rs, &cur_line->imm);
            break;
        case J_TYPE:
            jTypeParsing(msg, cur_line->op, &cur_line->imm);
            break;
    }
    // frees allocated memory to prevent leaks
    free(msg);
}

// generates an I-TYPE instruction based on input values
unsigned generateInstruction(instLine inst){
    unsigned result;
    if(inst.op != 0){
        inst.op = inst.op << 26; // bit shift for the opcode in the instruction
        inst.rs = inst.rs << 21; // bit shift for the rs in the instruction
        inst.rt = inst.rt << 16; // bit shift for the rt in the instruction
        result = inst.op + inst.rs + inst.rt + inst.imm;
    }
    else if(inst.op == 2 || inst.op == 3){
        inst.op = inst.op << 26; // bit shift for the opcode in the instruction
        result = inst.op + inst.imm;
    }
    else{
        inst.op = inst.op << 26; // bit shift for the opcode in the instruction
        inst.rs = inst.rs << 21; // bit shift for the rs in the instruction
        inst.rt = inst.rt << 16; // bit shift for the rt in the instruction
        inst.rd = inst.rd << 11; // bit shift for the rd in the instruction
        inst.sa = inst.sa << 6; // bit shift for the sa in the instruction
        result = inst.op + inst.rs + inst.rt + inst.rd + inst.sa + inst.funct;
    }
    return result;
}

int main() {
    bool isError = false;
    int numberOfLines = 0;

    // reads the assembly code file
    char **msg = readFile(&numberOfLines);

    instLine *instLines = (instLine *)malloc(sizeof(msg) * sizeof(instLine) * numberOfLines);

    for(int i = 0; i < numberOfLines; i++){
        instructionParsing(numberOfLines, msg[i], &instLines[i]);
    }

    free(msg);

    //if(op < 0 || op > MAX_OPCODE_NUM){
    //    printf("\nERROR: Instruction \"%.20s\" not found. Try again\n", field1);
    //    isError = true;
    //}
    //if(rt < 0 || rt > MAX_REG_NUM){
    //    printf("\nERROR: Register rt not found. Try again\n");
    //    isError = true;
    //}
    //if(rs < 0 || rs > MAX_REG_NUM){
    //    printf("\nERROR: Register rs not found. Try again\n");
    //    isError = true;
    //}
    //if((strcmp(immmsg[0], zero)) && imm == 0){
    //    printf("\nERROR: Immediate not valid. Try Again\n");
    //    isError = true;
    //}
    for(int i = 0; i < numberOfLines; i++){
        //if(!isError){
        //    printf("0x%04x\n", generateInstruction(instLines[i]));
        //}
        printf("0x%04x\n", generateInstruction(instLines[i]));
    }
    free(instLines);
}