#include "../include/parser.h"

// array of labels
static Label labels[MAX_LABELS];

// lookup table for opcodes
static const Instruction opcodes[] =
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

static const unsigned opcodeCount = sizeof(opcodes)/sizeof(opcodes[0]);

static const PseudoInstruction pseudoOps[] =
{
    {"move", 2, false},
    {"li", 2, false}
};

static const unsigned pseudoOpsCount = sizeof(pseudoOps)/sizeof(pseudoOps[0]);

// saves the values of all registers available
static const Register registers[] =
{
    {"$zero", "$0", 0},     //the constant value 0
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

static const unsigned registerCount = sizeof(registers)/sizeof(registers[0]);

void storeLabels(char **codeLines, unsigned *numberOfLines){
    // check for labels
    unsigned labelIdx = 0;
    for(unsigned i = 0; i < *numberOfLines; i++){
        // checks if instruction has a label
        char *ptr;
        ptr = strchr(codeLines[i], ':');
        if(ptr != NULL){
            char beforeLabel[BUF_SIZE_LINE];
            char *afterLabel;
            strcpy(beforeLabel, codeLines[i]);
            strtok_r(beforeLabel, ":", &afterLabel);
            // if it's not in the same line, left shift all elements and decrease array size
            if(checkEmptyString(afterLabel)){
                removeElement(&codeLines, *numberOfLines, i);
                
                labels[labelIdx].index = i;
                *numberOfLines = *numberOfLines - 1;
                i--;
            }
            // if it is, cut it away from the instruction line
            else{
                strcpy(codeLines[i], afterLabel);
                labels[labelIdx].index = i;
            }
            // saves label mnemonic
            strcpy(labels[labelIdx].mnemonic, beforeLabel);

            //printf("\nLabel: %s\nIndex: %d\n", labels[labelIdx].mnemonic, labels[labelIdx].index);

            labelIdx++;
        }
    }
}

byte getRegister(char *regMne){
    byte reg;
    for(unsigned i = 0; i < registerCount; i++){
        if(!strcmp(regMne, registers[i].mnemonic) || !strcmp(regMne, registers[i].alt_mne)){
            reg = registers[i].numCode;
            return reg;
        }
    }
    printf("Error: register '%s' not found\n", regMne);
    return INVALID_REGISTER;
}

void getDefaultParams(char *opmne, Instruction *inst){
    // searches for the opcode in the lookup table
    for(unsigned i = 0; i < opcodeCount; i++){
        if(!strcmp(opmne, opcodes[i].mnemonic)){
            // fills the instruction with opcode default parameters
            inst->op = opcodes[i].op;
            inst->type = opcodes[i].type;
            inst->rd = opcodes[i].rd;
            inst->rs = opcodes[i].rs;
            inst->rt = opcodes[i].rt;
            inst->imm = opcodes[i].imm;
            inst->sa = opcodes[i].sa;
            inst->funct = opcodes[i].funct;

            return;
        }
    }
    printf("Error: instruction '%s' not found\n", opmne);
    inst->type = INVALID_INSTRUCTION;
}

void rTypeParsing(char *arguments, Instruction *parsedInst){

    // string placeholders for instruction parsing
    char field2[50] = "";
    char field3[50] = "";
    char field4[50] = "";

    // in this range of opcodes, the instruction follows always the following pattern: "mnemonic rd, rt, shamt"
    if(parsedInst->funct <= 3){
        sscanf(arguments, "%[^,],%[^,],%hhu", field2, field3, &parsedInst->sa);
        parsedInst->rt = getRegister(field3);
        parsedInst->rd = getRegister(field2);
    }
    // in this range of opcodes, the instruction follows always the following pattern: "mnemonic rd, rt, rs"
    else if(parsedInst->funct <= 7){
        sscanf(arguments, "%[^,],%[^,],%s", field2, field3, field4);
        parsedInst->rd = getRegister(field2);
        parsedInst->rt = getRegister(field3);
        parsedInst->rs = getRegister(field4);
    }
    else if(parsedInst->funct == 12 || parsedInst->funct == 13){
        // code to be written
    }
    // in this range of opcodes, the instruction follows always the following patterns: "mnemonic rd, rs, rt"
    else if(parsedInst->funct >= 8){
        // for some instructions, rd is a fixed value
        if(parsedInst->rd == INPUT_FIELD){
            // for some instructions with variable rd, rs has a fixed value. If rs is variable, rt is also variable
            if(parsedInst->rs == INPUT_FIELD){
                sscanf(arguments, "%[^,],%[^,],%s", field2, field3, field4);
                parsedInst->rs = getRegister(field3);
                parsedInst->rt = getRegister(field4);
                parsedInst->rd = getRegister(field2);
            }
            // if rd is variable and rs is fixed, rt is also fixed
            else{
                sscanf(arguments, "%s", field2);
                parsedInst->rd = getRegister(field2);
            }
        }
        // if rd is fixed and rt is variable, rs is always variable
        else if(parsedInst->rt == INPUT_FIELD){
            sscanf(arguments, "%[^,],%s", field2, field3);
            parsedInst->rs = getRegister(field2);
            parsedInst->rt = getRegister(field3);
        }
        // if rd and rt are fixed, rs is the only parameter
        else if(parsedInst->rs == INPUT_FIELD){
            sscanf(arguments, "%s", field2);
            parsedInst->rs = getRegister(field2);
        }
    }
}

void iTypeParsing(char *arguments, Instruction *parsedInst, unsigned index){
    char field2[50];
    char field3[50];

    // in this range of opcodes, the instruction follows always the following pattern: "mnemonic rs, rt, label"
    if(parsedInst->op < 8){
        char label[50];
        //for some instructions, rt is a fixed value
        if(parsedInst->rs == INPUT_FIELD){
            if(parsedInst->rt == INPUT_FIELD){
                sscanf(arguments, "%[^,],%[^,],%s", field2, field3, label);
                parsedInst->rt = getRegister(field3);
                parsedInst->rs = getRegister(field2);
            }
            else{
                sscanf(arguments, "%[^,],%s", field2, label);
                parsedInst->rs = getRegister(field2);
            }
        }
        for(unsigned i = 0; i < MAX_LABELS; i++){
            if(!strcmp(labels[i].mnemonic, label)){
                parsedInst->imm = (short)(labels[i].index - index);
            }
        }
    }
    // in this range of opcodes, the instruction follows always the following pattern: "mnemonic rt, rs, immediate"
    else if(parsedInst->op < 32){
        // for the "lui" instruction, rs is always 0
        if(parsedInst->rt == INPUT_FIELD){
            if(parsedInst->rs == INPUT_FIELD){
                sscanf(arguments, "%[^,],%[^,],%hd", field2, field3, &parsedInst->imm);
                parsedInst->rt = getRegister(field2);
                parsedInst->rs = getRegister(field3);
            }
            else{
                sscanf(arguments, "%[^,],%hd", field2, &parsedInst->imm);
                parsedInst->rt = getRegister(field2);
            }
        }
    }
    // in this range of opcodes, the instruction follows always the following pattern: "mnemonic rt, immediate(rs)"
    else if(parsedInst->op >= 32){
        sscanf(arguments, "%[^,],%hd(%[^)])", field2, &parsedInst->imm, field3);
        parsedInst->rt = getRegister(field2);
        parsedInst->rs = getRegister(field3);
    }
}

void jTypeParsing(char *arguments, Instruction *parsedInst){
    char label[50];
    sscanf(arguments, "%s", label);
    for(int i = 0; i < MAX_LABELS; i++){
        if(!strcmp(labels[i].mnemonic, label)){
            parsedInst->imm = (int)labels[i].index;
        }
    }
}

void instructionParsing(char *msg, unsigned index, Instruction *cur_inst){
    // opcode mnemonic for opcode identification
    char opmne[10];
    // instruction arguments
    char bare_arguments[100] = "";
    char arguments[100] = "";
    // gets current instruction's opcode mnemonic and separate it from the rest of the instruction
    sscanf(msg, "%9s %99c", opmne, bare_arguments);

    // remove any whitespaces in the argument string
    removeSpaces(arguments, bare_arguments);

    for(unsigned j = 0; j < pseudoOpsCount; j++){
        if(!strcmp(opmne, pseudoOps[j].mnemonic)){
            char arg1[10];
            char arg2[10];
            char pseudoArguments[100] = "";

            switch(j){
                case MOVE:
                    sscanf(arguments, "%[^,],%[^,]", arg1, arg2);
                    strcat(pseudoArguments, arg1);
                    strcat(pseudoArguments, ",$0,");
                    strcat(pseudoArguments, arg2);
                    
                    strcpy(opmne, "add");
                    strcpy(arguments, pseudoArguments);
                    break;

                case LI:
                    sscanf(arguments, "%[^,],%[^,]", arg1, arg2);
                    strcat(pseudoArguments, arg1);
                    strcat(pseudoArguments, ",$0,");
                    strcat(pseudoArguments, arg2);

                    strcpy(opmne, "ori");
                    strcpy(arguments, pseudoArguments);
                    break;

                default:
                    break;
            }
        }
    }

    // gets default parameters for the opcode using the lookup table
    getDefaultParams(opmne, cur_inst);

    if(cur_inst->type == INVALID_INSTRUCTION){
        printf("Compilation error at: '%s'\n", msg);
        exit(EXIT_FAILURE);
    }

    switch(cur_inst->type){
        case R_TYPE:
            rTypeParsing(arguments, cur_inst);
            break;
        case I_TYPE:
            iTypeParsing(arguments, cur_inst, index);
            break;
        case J_TYPE:
            jTypeParsing(arguments, cur_inst);
            break;
    }

    if(cur_inst->rd == INVALID_REGISTER || cur_inst->rs == INVALID_REGISTER || cur_inst->rt == INVALID_REGISTER){
        printf("Compilation error at: '%s'\n", msg);
        exit(EXIT_FAILURE);
    }
}