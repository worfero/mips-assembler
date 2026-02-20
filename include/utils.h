#ifndef UTILS_H
#define UTILS_H

#define BUF_SIZE_LINE           100         // Maximum buffer for a line
#define MAX_DIGITS_32BIT        11          // Maximum number of digits in a 32-bit number

#define INT16_MIN               -32768
#define INT16_MAX               32767

//#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

void printStrings(char **arr, unsigned count);

int16_t getUpper16Bits(int32_t num);

int16_t getLower16Bits(int32_t num);

int16_t strToInt16t(const char *str);

int32_t strToInt32t(const char *str);

void removeSpaces (char* str_trimmed, const char* str_untrimmed);

void trimLeadingWhitespaces(char *str);

void removeElement(char **array, int sizeOfArray, int indexToRemove);

bool checkEmptyString(const char *str);

char** stringArrayMalloc(unsigned size);

#endif