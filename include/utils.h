#ifndef UTILS_H
#define UTILS_H

#define BUF_SIZE_LINE           100         // Maximum buffer for a line

//#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

void removeSpaces (char* str_trimmed, const char* str_untrimmed);

void removeElement(char*** array, int sizeOfArray, int indexToRemove);

bool checkEmptyString(const char *str);

char** stringMalloc(unsigned size);

#endif