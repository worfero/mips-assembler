#include "utils.h"

void removeSpaces (char* str_trimmed, const char* str_untrimmed){
    // logic to remove space characters from a string, a surprisingly difficult task (got a little help from stack overflow)
    while (*str_untrimmed != '\0'){
        // if the character is not a space, just copy it to the new string and update both pointers
        if(!isspace(*str_untrimmed)){
            *str_trimmed = *str_untrimmed;
            str_trimmed++;
        }
        // if it is, just point to the next character
        str_untrimmed++;
    }
    *str_trimmed = '\0';
}

void removeElement(char*** array, int sizeOfArray, int indexToRemove){
    // allocate an array with a size 1 less than the current one
    char** temp = stringMalloc(sizeOfArray - 1);

    // if the index to be removed is not the first, copy everything before it
    if(indexToRemove != 0){
        for(unsigned i = 0; i < indexToRemove; i++){
            strcpy(temp[i], (*array)[i]);
        }
    }

    // if the index to be removed is not the last, copy everything after it
    if(indexToRemove != (sizeOfArray - 1)){
        for(unsigned i = indexToRemove; i < sizeOfArray - 1; i++){
            strcpy(temp[i], (*array)[i+1]);
        }
    }

    // free previous array memory
    for (int i = 0; i < sizeOfArray; i++) {
        free((*array)[i]);
    }
    free(*array);

    // point array to the new values
    *array = temp;
}

bool checkEmptyString(const char *str){
    // if string is null or empty, return true
    if(str == NULL || strlen(str) == 0){
        return true;
    }
    for(int i = 0; str[i] != '\0'; i++) {
        // if a '#' is found before any other character, the line is just a comment so return true
        if(str[i] == '#'){
            return true;
        }
        // if a character is found before the end of line, return false
        if(!isspace((unsigned char)str[i])){
            return false;
        }
    }
    return true;
}

char** stringMalloc(unsigned size){
    char **str = (char **)malloc(size * sizeof(char*));
    for(unsigned i = 0; i < size; i++){
        str[i] = (char *)malloc((BUF_SIZE_LINE+1) * sizeof(char));
    }

    return str;
}