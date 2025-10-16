#ifndef MIPS_H
#define MIPS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define R_TYPE                  1           // R-TYPE instruction
#define I_TYPE                  2           // I-TYPE instruction
#define J_TYPE                  3           // J-TYPE instruction
#define N_A                     0           // field not applicable
#define INPUT_FIELD             10          // input field
#define INVALID_INSTRUCTION     99          // invalid instruction

#define MAX_REG_NUM             31          // Maximum number of registers available
#define MAX_OPCODE_NUM          56          // Maximum number of registers available
#define MAX_FILE_NAME           200         // Maximum number of characters in a file name
#define MAX_LABELS              100         // Maximum number of labels provided

#define BUF_SIZE_FILE           65536       // Maximum buffer for a file
#define BUF_SIZE_LINE           100         // Maximum buffer for a line

typedef struct {
    unsigned index;
    char mnemonic[10];
} Label;

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
} Instruction;

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

// defining register structure
typedef struct {
    char mnemonic[5]; // mnemonic
    char alt_mne[5]; // alternate mnemonic
    unsigned numCode; // opcode number
} Register;

void removeSpaces (char* str_trimmed, const char* str_untrimmed);

void removeElement(char*** array, int sizeOfArray, int indexToRemove);

bool checkEmptyString(const char *str);

unsigned countLines(FILE* file);

char** stringMalloc(unsigned size);

char** storeCode(FILE* file, unsigned *numberOfLines);

char** readFile(unsigned *numberOfLines, char* fileName);

void writeFile(unsigned data[], unsigned numberOfLines);

unsigned getRegister(char *regMne);

Instruction getDefaultParams(char *opmne);

Instruction rTypeParsing(char *msg, Instruction parsedInst);

Instruction iTypeParsing(char *msg, Instruction parsedInst, unsigned index);

Instruction jTypeParsing(char *msg, Instruction parsedInst);

Instruction instructionParsing(char *msg, unsigned index);

unsigned generateInstruction(Instruction inst);

void assemble(char* fileName);

#endif