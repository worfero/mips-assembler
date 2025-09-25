#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define R_TYPE          1        // R-TYPE instruction
#define I_TYPE          2        // I-TYPE instruction
#define J_TYPE          3        // J-TYPE instruction
#define N_A             0        // field not applicable
#define INPUT_FIELD     10       // input field

#define MAX_REG_NUM     31       // Maximum number of registers available
#define MAX_OPCODE_NUM  56       // Maximum number of registers available
#define MAX_FILE_NAME   200      // Maximum number of characters in a file name

#define BUF_SIZE_FILE   65536    // Maximum buffer for a file
#define BUF_SIZE_LINE   100      // Maximum buffer for a line

typedef struct {
    unsigned index;
    char mnemonic[10];
} labelFinder;

typedef struct {
    // declaring instruction variables
    unsigned type;
    unsigned op;
    unsigned rd;
    unsigned rs;
    unsigned rt;
    int imm;
    unsigned sa;
    unsigned funct;
    char field1[30];
} instLine;

// defining instruction code bit fields
typedef struct {
    char mnemonic[20]; // mnemonic
    unsigned instType; // instruction type
    unsigned numCode; // opcode number
    unsigned rdField; // rd register
    unsigned rsField; // rs register
    unsigned rtField; // rt register
    int immField; // immediate
    unsigned saField; // shamt
    unsigned functField; // function code
} Opcode;

// lookup table for opcodes
static const Opcode opcodes[] =
{
    {"sll"     , R_TYPE, 0 , INPUT_FIELD, 0          , INPUT_FIELD, N_A        , INPUT_FIELD, 0},
    {"srl"     , R_TYPE, 0 , INPUT_FIELD, 0          , INPUT_FIELD, N_A        , INPUT_FIELD, 2},
    {"sra"     , R_TYPE, 0 , INPUT_FIELD, 0          , INPUT_FIELD, N_A        , INPUT_FIELD, 3},
    {"sllv"    , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 4},
    {"srlv"    , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 6},
    {"srav"    , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 7},
    {"jr"      , R_TYPE, 0 , 0          , INPUT_FIELD, 0          , N_A        , 0          , 8},
    {"jalr"    , R_TYPE, 0 , 31         , INPUT_FIELD, 0          , N_A        , 0          , 9},
    {"syscall" , R_TYPE, 0 , 0          , 0          , 0          , N_A        , 0          , 12},
    {"break"   , R_TYPE, 0 , 0          , 0          , 0          , N_A        , 0          , 13},
    {"mfhi"    , R_TYPE, 0 , INPUT_FIELD, 0          , 0          , N_A        , 0          , 16},
    {"mthi"    , R_TYPE, 0 , 0          , INPUT_FIELD, 0          , N_A        , 0          , 17},
    {"mflo"    , R_TYPE, 0 , INPUT_FIELD, 0          , 0          , N_A        , 0          , 18},
    {"mtlo"    , R_TYPE, 0 , 0          , INPUT_FIELD, 0          , N_A        , 0          , 19},
    {"mult"    , R_TYPE, 0 , 0          , INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 24},
    {"multu"   , R_TYPE, 0 , 0          , INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 25},
    {"div"     , R_TYPE, 0 , 0          , INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 26},
    {"divu"    , R_TYPE, 0 , 0          , INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 27},
    {"add"     , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 32},
    {"addu"    , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 33},
    {"sub"     , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 34},
    {"subu"    , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 35},
    {"and"     , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 36},
    {"or"      , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 37},
    {"xor"     , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 38},
    {"nor"     , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 39},
    {"slt"     , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 42},
    {"sltu"    , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 43},
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
    char alt_mne[5]; // alternate mnemonic
    unsigned numCode; // opcode number
} Register;

// saves the values of all registers available
static const Register registers[] =
{
    {"$z", "$0", 0},     //the constant value 0
    {"$at", "$1" , 1},    //assembler temporary
    {"$v0", "$2" , 2},    //procedure return values
    {"$v1", "$3" , 3},
    {"$a0", "$4" , 4},    //procedure arguments
    {"$a1", "$5" , 5},
    {"$a2", "$6" , 6},
    {"$a3", "$7" , 7},
    {"$t0", "$8" , 8},    //temporary variables
    {"$t1", "$9" , 9},
    {"$t2", "$10" , 10},
    {"$t3", "$11" , 11},
    {"$t4", "$12" , 12},
    {"$t5", "$13" , 13},
    {"$t6", "$14" , 14},
    {"$t7", "$15" , 15},
    {"$s0", "$16" , 16},    //saved variables
    {"$s1", "$17" , 17},
    {"$s2", "$18" , 18},
    {"$s3", "$19" , 19},
    {"$s4", "$20" , 20},
    {"$s5", "$21" , 21},
    {"$s6", "$22" , 22},
    {"$s7", "$23" , 23},
    {"$t8", "$24" , 24},    //temporary variables
    {"$t9", "$25" , 25},
    {"$k0", "$26" , 26},    //operating system (OS) temporaries
    {"$k1", "$27" , 27},
    {"$gp", "$28" , 28},    //global pointer
    {"$sp", "$29" , 29},    //stack pointer
    {"$fp", "$30" , 30},    //frame pointer
    {"$ra", "$31" , 31}     //procedure return address
};

bool checkEmptyString(const char *str){
    // if string is null or empty, return true
    if(str == NULL || strlen(str) == 0){
        return true;
    }
    for(int i = 0; str[i] != '\0'; i++) {
        // if a '#' is found before any other character, the line is just a comment so return true
        if(str[i] == '#'){
            return true;
        }
        // if a character is found before the end of line, return false
        if(!isspace((unsigned char)str[i])){
            return false;
        }
    }
    return true;
}

unsigned countLines(FILE* file)
{
    char buf[BUF_SIZE_FILE];
    unsigned counter = 0;
    for(;;)
    {
        size_t res = fread(buf, 1, BUF_SIZE_FILE, file);
        if(ferror(file))
            return 0;

        for(size_t i = 0; i < res - 1; i++)
            if (buf[i] == '\n' && buf[i+1] != '\n' && buf[i+1] != ' '){
                counter++;
            }

        if(feof(file))
            break;
    }
    counter++;
    return counter;
}

char ** readFile(unsigned *numberOfLines, labelFinder *labels) {
    FILE *file;

    char fileName[MAX_FILE_NAME]; 
    printf("\nPlease input the name of the .asm file: ");
    fgets(fileName, sizeof(fileName), stdin);
    fileName[strcspn(fileName, "\n")] = 0;
    
    file = fopen(fileName, "r");
    if(file == NULL) {
        perror("Error opening file");
        return (char **)NULL;
    }

    *numberOfLines = countLines(file);
    
    rewind(file);

    char **lines = (char **)malloc(*numberOfLines * sizeof(char**));

    for(unsigned i = 0; i < *numberOfLines; i++){
        lines[i] = (char *)malloc((BUF_SIZE_LINE+1) * sizeof(char));
    }

    unsigned offset = 0;
    unsigned j = 0;
    for(unsigned i = 0; i < *numberOfLines; i++){
        fgets(lines[i - offset], BUF_SIZE_LINE, file);
        // if the line is empty or blank, ignore it
        if(checkEmptyString(lines[i - offset])){
            offset++;
        }
        else{
            // checks if instruction has a label
            char *ptr;
            ptr = strchr(lines[i - offset], ':');

            // removes newline and comments from the string
            lines[i - offset][strcspn(lines[i - offset], "\n")] = 0;
            lines[i - offset][strcspn(lines[i - offset], "#")] = '\0';

            // if the instruction has a label, stores it in the labels array and removes it from the line
            if(ptr != NULL){
                char *aux = strchr(lines[i - offset], ' ');
                labels[j].index = i;
                sscanf(lines[i - offset], "%[^:]:", labels[j].mnemonic);
                strcpy(lines[i - offset], aux + 1);
                j++;
            }
        }
    }
    // number of valid lines
    *numberOfLines = *numberOfLines - offset;
    fclose(file);

    return lines;
}

void writeFile(unsigned data[], unsigned numberOfLines){
    FILE *file;
    file = fopen("machine-code.bin", "wb");
    fwrite(data, sizeof(unsigned), numberOfLines, file);
}

unsigned getRegister(char *regMne){
    unsigned reg;
    for(unsigned i = 0; i <= (sizeof(registers))/sizeof(registers[0]); i++){
        if(!strcmp(regMne, registers[i].mnemonic) || !strcmp(regMne, registers[i].alt_mne)){
            reg = registers[i].numCode;
        }
    }
    return reg;
}

void getDefaultParams(unsigned *op, unsigned *type, unsigned *rd, unsigned *rs, unsigned *rt, int *imm, unsigned *sa, unsigned *funct, char *field1){
    // searches for the opcode in the lookup table
    for(unsigned i = 0; i <= sizeof(opcodes)/sizeof(opcodes[0]); i++){
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

void rTypeParsing(char *msg, unsigned *rd, unsigned *rs, unsigned *rt, unsigned *sa, unsigned funct){
    char field1[50];
    char field2[50];
    char field3[50];
    char field4[50];

    // in this range of opcodes, the instruction follows always the following pattern: "mnemonic rd, rt, shamt"
    if(funct <= 3){
        sscanf(msg, "%s %[^,], %[^,], %u", field1, field2, field3, sa);
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
            sscanf(msg, "%s %s", field1, field2);
            *rs = getRegister(field2);
        }
    }
}

void iTypeParsing(char *msg, unsigned op, unsigned *rt, unsigned *rs, int *imm, labelFinder *labels, unsigned index){
    char field1[50];
    char field2[50];
    char field3[50];

    // in this range of opcodes, the instruction follows always the following pattern: "mnemonic rs, rt, label" ps: label is an immediate
    //if(op < 8){
    //    // for some instructions, rt is a fixed value
    //    if(*rs == INPUT_FIELD){
    //        if(*rt == INPUT_FIELD){
    //            sscanf(msg, "%s %[^,], %[^,], %d", field1, field2, field3, imm);
    //            *rt = getRegister(field3);
    //            *rs = getRegister(field2);
    //        }
    //        else{
    //            sscanf(msg, "%s %[^,], %d", field1, field2, imm);
    //            *rs = getRegister(field2);
    //        }
    //    }
    //}
    // in this range of opcodes, the instruction follows always the following pattern: "mnemonic rs, rt, label"
    if(op < 8){
        char label[10];
        //for some instructions, rt is a fixed value
        if(*rs == INPUT_FIELD){
            if(*rt == INPUT_FIELD){
                sscanf(msg, "%s %[^,], %[^,], %s", field1, field2, field3, label);
                *rt = getRegister(field3);
                *rs = getRegister(field2);
            }
            else{
                sscanf(msg, "%s %[^,], %s", field1, field2, label);
                *rs = getRegister(field2);
            }
        }
        for(unsigned i = 0; i < 10; i++){
            if(!strcmp(labels[i].mnemonic, label)){
                *imm = (int)(labels[i].index - index);
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

void jTypeParsing(char *msg, int *imm, labelFinder *labels){
    char field1[50];
    char label[50];
    sscanf(msg, "%s %s", field1, label);
    for(int i = 0; i < 10; i++){
        if(!strcmp(labels[i].mnemonic, label)){
            *imm = (int)labels[i].index;
        }
    }
}

void instructionParsing(char *msg, instLine *cur_line, labelFinder *labels, unsigned index){
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
            iTypeParsing(msg, cur_line->op, &cur_line->rt, &cur_line->rs, &cur_line->imm, labels, index);
            break;
        case J_TYPE:
            jTypeParsing(msg, &cur_line->imm, labels);
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
        result = inst.op + inst.rs + inst.rt + (unsigned)inst.imm;
    }
    else if(inst.op == 2 || inst.op == 3){
        inst.op = inst.op << 26; // bit shift for the opcode in the instruction
        result = inst.op + (unsigned)inst.imm;
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
    unsigned numberOfLines = 0;
    labelFinder labels[10];

    // reads the assembly code file
    char **msg = readFile(&numberOfLines, labels);
    while(msg == NULL){
        msg = readFile(&numberOfLines, labels);
    }
    unsigned *data = (unsigned int*)malloc(numberOfLines*sizeof(unsigned));

    instLine *instLines = (instLine *)malloc(sizeof(msg) * sizeof(instLine) * numberOfLines);

    for(unsigned i = 0; i < numberOfLines; i++){
        instructionParsing(msg[i], &instLines[i], labels, i + 1);
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
    for(unsigned i = 0; i < numberOfLines; i++){
        //if(!isError){
        //    printf("0x%04x\n", generateInstruction(instLines[i]));
        //}
        printf("0x%04x\n", generateInstruction(instLines[i]));
        data[i] = generateInstruction(instLines[i]);
    }
    free(instLines);
    writeFile(data, numberOfLines);
}