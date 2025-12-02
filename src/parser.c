#include "../include/parser.h"

// array of labels
static Label labels[MAX_LABELS];
static StoredData storedData[MAX_VAR];
static unsigned varCount;
static unsigned labelCount;

// lookup table for opcodes
static const Instruction opcodes[] =
{
    {"sll"     , R_TYPE, 0 , INPUT_FIELD, 0          , INPUT_FIELD, N_A        , INPUT_FIELD, 0  , N_A},
    {"srl"     , R_TYPE, 0 , INPUT_FIELD, 0          , INPUT_FIELD, N_A        , INPUT_FIELD, 2  , N_A},
    {"sra"     , R_TYPE, 0 , INPUT_FIELD, 0          , INPUT_FIELD, N_A        , INPUT_FIELD, 3  , N_A},
    {"sllv"    , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 4  , N_A},
    {"srlv"    , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 6  , N_A},
    {"srav"    , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 7  , N_A},
    {"jr"      , R_TYPE, 0 , 0          , INPUT_FIELD, 0          , N_A        , 0          , 8  , N_A},
    {"jalr"    , R_TYPE, 0 , 31         , INPUT_FIELD, 0          , N_A        , 0          , 9  , N_A},
    {"syscall" , R_TYPE, 0 , 0          , 0          , 0          , N_A        , 0          , 12 , N_A},
    {"break"   , R_TYPE, 0 , 0          , 0          , 0          , N_A        , 0          , 13 , N_A},
    {"mfhi"    , R_TYPE, 0 , INPUT_FIELD, 0          , 0          , N_A        , 0          , 16 , N_A},
    {"mthi"    , R_TYPE, 0 , 0          , INPUT_FIELD, 0          , N_A        , 0          , 17 , N_A},
    {"mflo"    , R_TYPE, 0 , INPUT_FIELD, 0          , 0          , N_A        , 0          , 18 , N_A},
    {"mtlo"    , R_TYPE, 0 , 0          , INPUT_FIELD, 0          , N_A        , 0          , 19 , N_A},
    {"mult"    , R_TYPE, 0 , 0          , INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 24 , N_A},
    {"multu"   , R_TYPE, 0 , 0          , INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 25 , N_A},
    {"div"     , R_TYPE, 0 , 0          , INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 26 , N_A},
    {"divu"    , R_TYPE, 0 , 0          , INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 27 , N_A},
    {"add"     , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 32 , N_A},
    {"addu"    , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 33 , N_A},
    {"sub"     , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 34 , N_A},
    {"subu"    , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 35 , N_A},
    {"and"     , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 36 , N_A},
    {"or"      , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 37 , N_A},
    {"xor"     , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 38 , N_A},
    {"nor"     , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 39 , N_A},
    {"slt"     , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 42 , N_A},
    {"sltu"    , R_TYPE, 0 , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , 0          , 43 , N_A},
    {"bgez"    , I_TYPE, 1 , N_A        , INPUT_FIELD, 1          , INPUT_FIELD, N_A        , N_A, N_A},
    {"bltz"    , I_TYPE, 1 , N_A        , INPUT_FIELD, 0          , INPUT_FIELD, N_A        , N_A, N_A},
    {"j"       , J_TYPE, 2 , N_A        , N_A        , N_A        , INPUT_FIELD, N_A        , N_A, N_A},
    {"jal"     , J_TYPE, 3 , N_A        , N_A        , N_A        , INPUT_FIELD, N_A        , N_A, N_A},
    {"bne"     , I_TYPE, 5 , N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A},
    {"blez"    , I_TYPE, 6 , N_A        , INPUT_FIELD, 0          , INPUT_FIELD, N_A        , N_A, N_A},
    {"bgtz"    , I_TYPE, 7 , N_A        , INPUT_FIELD, 0          , INPUT_FIELD, N_A        , N_A, N_A},
    {"beq"     , I_TYPE, 4 , N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A},
    {"addi"    , I_TYPE, 8 , N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A},
    {"addiu"   , I_TYPE, 9 , N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A},
    {"slti"    , I_TYPE, 10, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A},
    {"sltiu"   , I_TYPE, 11, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A},
    {"andi"    , I_TYPE, 12, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A},
    {"ori"     , I_TYPE, 13, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A},
    {"xori"    , I_TYPE, 14, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A},
    {"lui"     , I_TYPE, 15, N_A        , 0          , INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A},
    {"lb"      , I_TYPE, 32, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A},
    {"lh"      , I_TYPE, 33, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A},
    {"lw"      , I_TYPE, 35, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A},
    {"lbu"     , I_TYPE, 36, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A},
    {"lhu"     , I_TYPE, 37, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A},
    {"sb"      , I_TYPE, 40, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A},
    {"sh"      , I_TYPE, 41, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A},
    {"sw"      , I_TYPE, 43, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A},
    {"lwcl"    , I_TYPE, 49, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A},
    {"swcl"    , I_TYPE, 56, N_A        , INPUT_FIELD, INPUT_FIELD, INPUT_FIELD, N_A        , N_A, N_A}
};

static const unsigned opcodeCount = sizeof(opcodes)/sizeof(opcodes[0]);

void printInstructions(Instruction *instructions, int count){
    for(int i = 0; i < count; i++){
        printf("Instruction\n----------------\n");
        printf("index: %d\n", instructions[i].index);
        printf("mnemonic: %s\n", instructions[i].mnemonic);
        switch(instructions[i].type){
            case R_TYPE:
                printf("type: %s\n", "R-TYPE");
                break;
            case I_TYPE:
                printf("type: %s\n", "I-TYPE");
                break;
            case J_TYPE:
                printf("type: %s\n", "J-TYPE");
                break;
            default:
                printf("type: Invalid");
                break;
        }
        printf("opcode: %d\n", instructions[i].op);
        if(instructions[i].rd != N_A){
            printf("rd: %d\n", instructions[i].rd);
        }
        if(instructions[i].rs != N_A){
            printf("rs: %d\n", instructions[i].rs);
        }
        if(instructions[i].rt != N_A){
            printf("rt: %d\n", instructions[i].rt);
        }
        if(instructions[i].imm != N_A){
            printf("immediate: %d\n", instructions[i].imm);
        }
        if(instructions[i].sa != N_A){
            printf("sa: %d\n", instructions[i].sa);
        }
        if(instructions[i].funct != N_A){
            printf("func: %d\n", instructions[i].funct);
        }
    }
}

static const char pseudoOps[8][10] =
{
    "move",
    "li",
    "lw",
    "la",
    "blt",
    "ble",
    "bgt",
    "bge"
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

Segment findSegment(char *type, char **lines, unsigned numberOfLines){
    Segment segment;
    char **linePtr;
    char **eof;

    eof = lines + numberOfLines;
    
    segment.start = findStart(type, lines, numberOfLines);
    if(segment.start == NULL){
        segment.end = NULL;
        segment.lineCount = 0;
        return segment;
    }
    linePtr = segment.start;

    while(linePtr < eof){
        char *line = *linePtr;
        if(line[0] == '.'){
            segment.end = linePtr;
            break;
        }
        linePtr++;
    }
    if(linePtr == eof){
        segment.end = eof;
    }

    segment.lineCount = segment.end - segment.start;

    return segment;
}

void printSegment(Segment segment){
    char **linePtr = segment.start;
    for(; linePtr < segment.end; linePtr++){
        char *line = *linePtr;
        printf("%s\n", line);
    }
}

char **findStart(char *type, char **lines, unsigned numberOfLines){
    char **segment = NULL;
    for(unsigned i = 0; i < numberOfLines; i++){
        char *ptr = strstr(lines[i], type);
        if(ptr != NULL){
            char *endOfLine = lines[i] + strlen(lines[i]);
            ptr += strlen(type);
            if(ptr < endOfLine){
                // if the first instruction doesn't start in the same line as the segment declaration, it starts in the next
                if(!checkEmptyString(ptr)){ 
                    trimLeadingWhitespaces(ptr);
                    segment = &ptr;
                    break;
                }
            }
            segment = &lines[i+1];
            break;
        }
    }

    if(segment == NULL){
        if(!strcmp(type, ".text")){
            segment = &lines[0];
            return segment;
        }
        return NULL;
    }

    return segment;
}

void parseData(Segment dataSegment){
    char *line;
    char **linePtr = dataSegment.start;
    int varIndex = 0;
    int memOffset = 0;
    for(; linePtr < dataSegment.end; linePtr++){
        line = *linePtr;
        trimLeadingWhitespaces(line);
        if(line[0] == '.'){
            break;
        }

        char *colon;
        colon = strchr(line, ':');
        if(colon == NULL){
            printf("Error: missing colon on data declaration\n");
            exit(EXIT_FAILURE);
        }

        char beforeLabel[BUF_SIZE_LINE];
        char type[BUF_SIZE_LINE];
        char val[BUF_SIZE_LINE];
        char *afterLabel;

        strcpy(beforeLabel, line);
        strtok_r(beforeLabel, ":", &afterLabel);
        trimLeadingWhitespaces(afterLabel);

        if(checkEmptyString(afterLabel)){
            printf("Error: missing data declaration\n");
            exit(EXIT_FAILURE);
        }

        strcpy(storedData[varIndex].name, beforeLabel);

        sscanf(afterLabel, "%99s %999s", type, val);

        if(!strcmp(".word", type)){
            storedData[varIndex].type = WORD;

            char *end;
            storedData[varIndex].value.wordVal = (int32_t)strtol(val, &end, 10);

            if(memOffset % 4 == 0){
                storedData[varIndex].addr = memOffset;
            }
            else{
                while(memOffset % 4 != 0) memOffset++;
                storedData[varIndex].addr = memOffset;
            }
            memOffset += 4;
        }
        else if(!strcmp(".float", type)){
            storedData[varIndex].type = FLOAT;

            char *end;
            storedData[varIndex].value.floatVal = strtof(val, &end);

            if(memOffset % 4 == 0){
                storedData[varIndex].addr = memOffset;
            }
            else{
                while(memOffset % 4 != 0) memOffset++;
                storedData[varIndex].addr = memOffset;
            }
            memOffset += 4;
        }
        else if(!strcmp(".half", type)){
            storedData[varIndex].type = HALF;

            char *end;
            storedData[varIndex].value.halfVal = (int16_t)strtol(val, &end, 10);

            if(memOffset % 2 == 0){
                storedData[varIndex].addr = memOffset;
            }
            else{
                memOffset++;
                storedData[varIndex].addr = memOffset;
            }
            memOffset += 2;
        }
        else if(!strcmp(".byte", type)){
            storedData[varIndex].type = BYTE;

            char *end;
            storedData[varIndex].value.halfVal = (int8_t)strtol(val, &end, 10);
            storedData[varIndex].addr = memOffset;
            memOffset += 1;
        }
        else{
            printf("Error: '%s' is not a data type\n", type);
        }
        varIndex++;
    }

    varCount = varIndex;
}

void parseLabels(Segment *codeSegment){
    // check for labels
    char **linePtr = codeSegment->start;
    unsigned labelIdx = 0;
    for(; linePtr < codeSegment->end; linePtr++){
        // checks if instruction has a label
        char *ptr;
        ptr = strchr(*linePtr, ':');
        if(ptr != NULL){
            char beforeLabel[BUF_SIZE_LINE];
            char *afterLabel;
            
            strcpy(beforeLabel, *linePtr);
            strtok_r(beforeLabel, ":", &afterLabel);
            trimLeadingWhitespaces(afterLabel);

            // if it's not in the same line, left shift all elements ahead and decrease ending pointer
            if(checkEmptyString(afterLabel)){
                int index = linePtr - codeSegment->start;
                removeElement(codeSegment->start, codeSegment->lineCount, index);
                codeSegment->end = codeSegment->end - 1;
                codeSegment->lineCount = codeSegment->lineCount - 1;
                linePtr--;
            }
            // if it is, change the pointer to after the label (i.e the instruction)
            else{
                ptr++;
                while(isspace(*ptr)){
                    ptr++;
                }
                char *newPtr = strdup(ptr);  // allocates new memory
                free(*linePtr);
                *linePtr = newPtr;

                unsigned index = linePtr - codeSegment->start;
                labels[labelIdx].index = index;
            }
            // saves label mnemonic
            strcpy(labels[labelIdx].mnemonic, beforeLabel);

            labelIdx++;
        }
    }
    labelCount = labelIdx;
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
            strcpy(inst->mnemonic, opcodes[i].mnemonic);
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

void iTypeParsing(char *arguments, Instruction *parsedInst){
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
                parsedInst->imm = (short)(labels[i].index - parsedInst->index);
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

void preProcess(char *line, char *cleanLine, bool *isSecondInstruction){
    // opcode mnemonic for opcode identification
    char opmne[10] = "";
    // instruction arguments
    char bare_arguments[100] = "";
    // instruction arguments
    char arguments[100] = "";
    // gets current instruction's opcode mnemonic and separate it from the rest of the instruction
    sscanf(line, "%9s %99c", opmne, bare_arguments);

    // remove any whitespaces in the argument string
    removeSpaces(arguments, bare_arguments);

    for(unsigned j = 0; j < pseudoOpsCount; j++){
        if(!strcmp(opmne, pseudoOps[j])){
            char arg1[10];
            char arg2[10];
            char arg3[10];
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

                case LW:
                    if(strchr(arguments, '(') == NULL){
                        if(*isSecondInstruction){
                            sscanf(arguments, "%[^,],%[^,]", arg1, arg2);
                            strcat(pseudoArguments, arg1);
                            strcat(pseudoArguments, ",");
                            char varOffset[10];
                            varOffset[0] = '\0';
                            for(int i = 0; i < varCount; i++){
                                if(!strcmp(arg2, storedData[i].name)){
                                    // get upper 16-bit of the variable address
                                    snprintf(varOffset, sizeof(varOffset), "%u", storedData[i].addr);
                                    if(varOffset[0] == '\0'){
                                        printf("Error: variable '%9s' is used but not defined\n", arg2);
                                        exit(EXIT_FAILURE);
                                    }
                        
                                    strcat(pseudoArguments, varOffset);
                                    strcat(pseudoArguments, "($at)");
                                }
                            }
                        }
                        else{
                            strcpy(opmne, "lui");
                            sscanf(arguments, "%[^,],%[^,]", arg1, arg2);

                            char dataMemAddr[10];
                            sprintf(dataMemAddr, "%u", (int16_t)(MIPS_DATA_ADDR >> 16));
                            
                            strcat(pseudoArguments, "$at,");
                            strcat(pseudoArguments, dataMemAddr);

                            *isSecondInstruction = true;
                        }
                        strcpy(arguments, pseudoArguments);
                    }
                    break;

                case LA:
                    if(*isSecondInstruction){
                        strcpy(opmne, "ori");
                        sscanf(arguments, "%[^,],%[^,]", arg1, arg2);
                        strcat(pseudoArguments, arg1);
                        strcat(pseudoArguments, ",$at,");
                        char varOffset[10];
                        varOffset[0] = '\0';
                        for(int i = 0; i < varCount; i++){
                            if(!strcmp(arg2, storedData[i].name)){
                                // get upper 16-bit of the variable address
                                snprintf(varOffset, sizeof(varOffset), "%u", storedData[i].addr);
                                if(varOffset[0] == '\0'){
                                    printf("Error: variable '%9s' is used but not defined\n", arg2);
                                    exit(EXIT_FAILURE);
                                }
                    
                                strcat(pseudoArguments, varOffset);
                            }
                        }
                    }
                    else{
                        strcpy(opmne, "lui");
                        sscanf(arguments, "%[^,],%[^,]", arg1, arg2);

                        char dataMemAddr[10];
                        sprintf(dataMemAddr, "%u", (int16_t)(MIPS_DATA_ADDR >> 16));
                        
                        strcat(pseudoArguments, "$at,");
                        strcat(pseudoArguments, dataMemAddr);

                        *isSecondInstruction = true;
                    }

                    strcpy(arguments, pseudoArguments);
                    break;
                
                
                case BLT:
                    if(*isSecondInstruction){
                        strcpy(opmne, "bne");
                        sscanf(arguments, "%[^,],%[^,],%[^,]", arg1, arg2, arg3);

                        strcat(pseudoArguments, "$at,$0,");
                        bool isValid = false;
                        for(int i = 0; i < labelCount; i++){
                            if(!strcmp(arg3, labels[i].mnemonic)){
                                strcat(pseudoArguments, labels[i].mnemonic);
                                isValid = true;
                                break;
                            }
                        }
                        if(!isValid){
                            printf("Error: label '%s' not found.\n", arg3);
                            exit(EXIT_FAILURE);
                        }
                    }
                    else{
                        strcpy(opmne, "slt");
                        sscanf(arguments, "%[^,],%[^,],%[^,]", arg1, arg2, arg3);

                        strcat(pseudoArguments, "$at,");
                        strcat(pseudoArguments, arg1);
                        strcat(pseudoArguments, arg2);

                        *isSecondInstruction = true;
                    }

                    strcpy(arguments, pseudoArguments);
                    break;
                
                case BLE:
                    if(*isSecondInstruction){
                        strcpy(opmne, "beq");
                        sscanf(arguments, "%[^,],%[^,],%[^,]", arg1, arg2, arg3);

                        strcat(pseudoArguments, "$at,$0,");
                        bool isValid = false;
                        for(int i = 0; i < labelCount; i++){
                            if(!strcmp(arg3, labels[i].mnemonic)){
                                strcat(pseudoArguments, labels[i].mnemonic);
                                isValid = true;
                                break;
                            }
                        }
                        if(!isValid){
                            printf("Error: label '%s' not found.\n", arg3);
                            exit(EXIT_FAILURE);
                        }
                    }
                    else{
                        strcpy(opmne, "slt");
                        sscanf(arguments, "%[^,],%[^,],%[^,]", arg1, arg2, arg3);

                        strcat(pseudoArguments, "$at,");
                        strcat(pseudoArguments, arg2);
                        strcat(pseudoArguments, arg1);

                        *isSecondInstruction = true;
                    }

                    strcpy(arguments, pseudoArguments);
                    break;

                case BGT:
                    if(*isSecondInstruction){
                        strcpy(opmne, "bne");
                        sscanf(arguments, "%[^,],%[^,],%[^,]", arg1, arg2, arg3);

                        strcat(pseudoArguments, "$at,$0,");
                        bool isValid = false;
                        for(int i = 0; i < labelCount; i++){
                            if(!strcmp(arg3, labels[i].mnemonic)){
                                strcat(pseudoArguments, labels[i].mnemonic);
                                isValid = true;
                                break;
                            }
                        }
                        if(!isValid){
                            printf("Error: label '%s' not found.\n", arg3);
                            exit(EXIT_FAILURE);
                        }
                    }
                    else{
                        strcpy(opmne, "slt");
                        sscanf(arguments, "%[^,],%[^,],%[^,]", arg1, arg2, arg3);

                        strcat(pseudoArguments, "$at,");
                        strcat(pseudoArguments, arg2);
                        strcat(pseudoArguments, arg1);

                        *isSecondInstruction = true;
                    }

                    strcpy(arguments, pseudoArguments);
                    break;
                
                case BGE:
                    if(*isSecondInstruction){
                        strcpy(opmne, "beq");
                        sscanf(arguments, "%[^,],%[^,],%[^,]", arg1, arg2, arg3);

                        strcat(pseudoArguments, "$at,$0,");
                        bool isValid = false;
                        for(int i = 0; i < labelCount; i++){
                            if(!strcmp(arg3, labels[i].mnemonic)){
                                strcat(pseudoArguments, labels[i].mnemonic);
                                isValid = true;
                                break;
                            }
                        }
                        if(!isValid){
                            printf("Error: label '%s' not found.\n", arg3);
                            exit(EXIT_FAILURE);
                        }
                    }
                    else{
                        strcpy(opmne, "slt");
                        sscanf(arguments, "%[^,],%[^,],%[^,]", arg1, arg2, arg3);

                        strcat(pseudoArguments, "$at,");
                        strcat(pseudoArguments, arg1);
                        strcat(pseudoArguments, arg2);

                        *isSecondInstruction = true;
                    }

                    strcpy(arguments, pseudoArguments);
                    break;

                default:
                    
                    break;
            }
        }
    }

    char cleanLineBuf[BUF_SIZE_LINE] = "";
    strcat(cleanLineBuf, opmne);
    strcat(cleanLineBuf, " ");
    strcat(cleanLineBuf, arguments);

    strcpy(cleanLine, cleanLineBuf);
}

void instructionParsing(char *line, Instruction *cur_inst){
    // opcode mnemonic for opcode identification
    char opmne[10] = "";
    // instruction arguments
    char arguments[100] = "";
    // gets current instruction's opcode mnemonic and separate it from the rest of the instruction
    sscanf(line, "%9s %99c", opmne, arguments);

    // gets default parameters for the opcode using the lookup table
    getDefaultParams(opmne, cur_inst);

    if(cur_inst->type == INVALID_INSTRUCTION){
        printf("Compilation error at: '%s'\n", line);
        exit(EXIT_FAILURE);
    }

    switch(cur_inst->type){
        case R_TYPE:
            rTypeParsing(arguments, cur_inst);
            break;
        case I_TYPE:
            iTypeParsing(arguments, cur_inst);
            break;
        case J_TYPE:
            jTypeParsing(arguments, cur_inst);
            break;
    }

    if(cur_inst->rd == INVALID_REGISTER || cur_inst->rs == INVALID_REGISTER || cur_inst->rt == INVALID_REGISTER){
        printf("Compilation error at: '%s'\n", line);
        exit(EXIT_FAILURE);
    }
}

void parser(char **msg, Instruction **instructions, unsigned numberOfLines, unsigned *instCount){
    // find each segment of the code and get their respective pointers
    Segment dataSegment;
    Segment codeSegment;
    
    dataSegment = findSegment(".data", msg, numberOfLines);

    if(dataSegment.start != NULL && dataSegment.end != NULL){
        parseData(dataSegment);
    }

    codeSegment = findSegment(".text", msg, numberOfLines);

    if(codeSegment.start == NULL || codeSegment.end == NULL){
        printf("Error: codeSegment pointers can't be null\n");
        exit(EXIT_FAILURE);
    }

    // store every label in the labels array
    parseLabels(&codeSegment);

    // store parsed instructions in the array
    bool isSecondInstruction = false;
    char **linePtr = codeSegment.start;

    // allocate memory for the instructions considering 
    *instructions = (Instruction *)malloc(sizeof(Instruction) * (codeSegment.lineCount));
    Instruction *cur_inst = *instructions;
    char cleanLine[BUF_SIZE_LINE];

    for(int i = 0; linePtr < codeSegment.end; linePtr++, cur_inst++, i++){
        preProcess(*linePtr, cleanLine, &isSecondInstruction);
        cur_inst->index = i + 1;
        instructionParsing(cleanLine, cur_inst);
        if(isSecondInstruction){
            codeSegment.lineCount++;
            i++;
            
            Instruction *newInstructions = (Instruction *)realloc(*instructions, codeSegment.lineCount * sizeof(Instruction));
            *instructions = newInstructions;

            cur_inst = *instructions + i;
            preProcess(*linePtr, cleanLine, &isSecondInstruction);
            cur_inst->index = i + 1;
            instructionParsing(cleanLine, cur_inst);

            isSecondInstruction = false;
        }
    }

    *instCount = codeSegment.lineCount;
}