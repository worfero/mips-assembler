#ifndef MIPS_H
#define MIPS_H

#include <ctype.h>

#include "parser.h"
#include "utils.h"
#include "file_manager.h"

unsigned generateInstruction(Instruction inst);

void assemble(char *fileName, char *outputDir);

#endif