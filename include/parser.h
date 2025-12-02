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

#define MIPS_DATA_ADDR          0x10010000  // Memory address for data storage

typedef unsigned char byte;

typedef struct {
    char **start;
    char **end;
    unsigned lineCount;
} Segment;

Segment findSegment(char *type, char **lines, unsigned numberOfLines);

typedef struct {
    unsigned index;
    char mnemonic[10];
} Label;

union Value{
    int8_t byteVal;
    int16_t halfVal;
    int32_t wordVal;
    int64_t dwordVal;
    float floatVal;
    bool boolVal;
};

typedef struct {
    unsigned addr;
    char name[50];
    enum {
        BYTE,
        HALF,
        WORD,
        DWORD,
        FLOAT,
    } type;
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
    unsigned index;
} Instruction;

void printInstructions(Instruction *instructions, int count);

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

char **findStart(char *type, char **lines, unsigned numberOfLines);

void parseData(Segment dataSegment);

void parseLabels(Segment *codeSegments);

byte getRegister(char *regMne);

void getDefaultParams(char *opmne, Instruction *inst);

void rTypeParsing(char *msg, Instruction *parsedInst);

void iTypeParsing(char *msg, Instruction *parsedInst);

void jTypeParsing(char *msg, Instruction *parsedInst);

void preProcess(char *line, char *cleanLine, bool *isSecondInstruction);

void instructionParsing(char *line, Instruction *cur_inst);

void parser(char **msg, Instruction **instructions, unsigned numberOfLines, unsigned *codeSize);

#endif