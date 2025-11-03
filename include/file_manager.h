#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

#define BUF_SIZE_FILE           65536       // Maximum buffer for a file
#define BUF_SIZE_LINE           100         // Maximum buffer for a line

unsigned countLines(FILE* file);

char** storeCode(FILE* file, unsigned *numberOfLines);

char** readFile(unsigned *numberOfLines, char* fileName);

void writeFile(unsigned data[], unsigned numberOfLines);

#endif