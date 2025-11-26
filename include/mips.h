#ifndef MIPS_H
#define MIPS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "utils.h"
#include "file_manager.h"

#define R_TYPE                  1           // R-TYPE instruction
#define I_TYPE                  2           // I-TYPE instruction
#define J_TYPE                  3           // J-TYPE instruction
#define N_A                     0           // field not applicable
#define INPUT_FIELD             10          // input field
#define INVALID_INSTRUCTION     99          // invalid instruction
#define INVALID_REGISTER        99          // invalid register

#define MOVE                    0           // MOVE pseudo-instruction index
#define LI                      1           // LI pseudo-instruction index

#define MAX_REG_NUM             31          // Maximum number of registers available
#define MAX_OPCODE_NUM          56          // Maximum number of registers available
#define MAX_LABELS              100         // Maximum number of labels provided

typedef unsigned char byte;

typedef struct {
    unsigned index;
    char mnemonic[10];
} Label;

typedef struct {
    // declaring instruction variables
    char mnemonic[20];
    byte type;
    byte op;
    byte rd;
    byte rs;
    byte rt;
    int imm;
    byte sa;
    byte funct;
} Instruction;

typedef struct {
    char mnemonic[20];
    byte argCount;
    bool isTwoInstructions;
} PseudoInstruction;

// defining register structure
typedef struct {
    char mnemonic[6]; // mnemonic
    char alt_mne[6]; // alternate mnemonic
    byte numCode; // opcode number
} Register;

void storeLabels(char **codeLines, unsigned *numberOfLines);

byte getRegister(char *regMne);

Instruction getDefaultParams(char *opmne);

Instruction rTypeParsing(char *msg, Instruction parsedInst);

Instruction iTypeParsing(char *msg, Instruction parsedInst, unsigned index);

Instruction jTypeParsing(char *msg, Instruction parsedInst);

Instruction instructionParsing(char *msg, unsigned index);

unsigned generateInstruction(Instruction inst);

void assemble(char *fileName, char *outputDir);

#endif