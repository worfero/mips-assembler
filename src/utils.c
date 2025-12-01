#include "../include/utils.h"

int16_t strToInt16t(const char *str){
    char *endptr;
    long val = strtol(str, &endptr, 10);

    if (endptr == str){
        printf("Error: variable is not a number");
        exit(EXIT_FAILURE);
    }

    while (*endptr != '\0') {
        if (!isspace((unsigned char)*endptr))
            perror("Error: variable is not a number");
        endptr++;
    }

    return (int16_t)val;
}

void removeSpaces(char* str_trimmed, const char* str_untrimmed){
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

void trimLeadingWhitespaces(char *str){
    if (str == NULL || *str == '\0') {
        return;
    }

    int i = 0;
    while (str[i] != '\0' && isspace((unsigned char)str[i])) {
        i++;
    }

    if (i > 0) {
        int j = 0;
        while (str[i] != '\0') {
            str[j++] = str[i++];
        }
        str[j] = '\0'; // Null-terminate the new string
    }
}

void removeElement(char **array, int sizeOfArray, int indexToRemove){
    // if the index to be removed is not the last, copy everything after it
    if(indexToRemove != (sizeOfArray - 1)){
        for(unsigned i = indexToRemove; i < sizeOfArray - 1; i++){
            strcpy(array[i], array[i+1]);
        }
    }
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