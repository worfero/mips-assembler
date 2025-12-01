#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "utils.h"

#define MAX_REG_NUM             31          // Maximum number of registers available
#define MAX_OPCODE_NUM          56          // Maximum number of registers available
#define MAX_LABELS              100         // Maximum number of labels provided
#define MAX_VAR                 100000      // Maximum number of variables provided

#define MOVE                    0           // MOVE pseudo-instruction index
#define LI                      1           // LI pseudo-instruction index
#define LW                      2           // LW pseudo-instruction index

#define R_TYPE                  1           // R-TYPE instruction
#define I_TYPE                  2           // I-TYPE instruction
#define J_TYPE                  3           // J-TYPE instruction
#define N_A                     0           // field not applicable
#define INPUT_FIELD             10          // input field
#define INVALID_INSTRUCTION     99          // invalid instruction
#define INVALID_REGISTER        99          // invalid register

#define BUF_SIZE_LINE           100         // Maximum buffer for a line

typedef unsigned char byte;

typedef struct {
    unsigned index;
    char mnemonic[10];
} Label;

union Value{
    int16_t wordVal;
    int32_t dwordVal;
    float floatVal;
    bool boolVal;
};

typedef struct {
    unsigned addr;
    char name[50];
    union Value value;
} VarLabel;

typedef struct {
    // declaring instruction variables
    char mnemonic[20];
    byte type;
    byte op;
    byte rd;
    byte rs;
    byte rt;
    short imm;
    byte sa;
    byte funct;
} Instruction;

typedef struct {
    char mnemonic[20];
    byte argCount;
} PseudoInstruction;

// defining register structure
typedef struct {
    char mnemonic[6]; // mnemonic
    char alt_mne[6]; // alternate mnemonic
    byte numCode; // opcode number
} Register;

char **findSegment(char *type, char **lines, unsigned *numberOfLines);

void parseData(char **codeLines, char **dataPtr, unsigned *numberOfLines);

void storeLabels(char **codeLines, unsigned *numberOfLines);

byte getRegister(char *regMne);

void getDefaultParams(char *opmne, Instruction *inst);

void rTypeParsing(char *msg, Instruction *parsedInst);

void iTypeParsing(char *msg, Instruction *parsedInst, unsigned index);

void jTypeParsing(char *msg, Instruction *parsedInst);

void instructionParsing(char *msg, unsigned index, Instruction *cur_inst, bool isSecondInstruction);

void parser(char **msg, Instruction *instructions, unsigned *numberOfLines);

#endif