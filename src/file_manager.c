#include "file_manager.h"

unsigned countLines(FILE* file)
{
    char buf[BUF_SIZE_FILE];
    unsigned counter = 0;
    for(;;)
    {
        size_t res = fread(buf, 1, BUF_SIZE_FILE, file);
        if(ferror(file))
            return 0;

        for(size_t i = 0; i < res - 1; i++)
            if (buf[i] == '\n'){
                counter++;
            }

        if(feof(file))
            break;
    }
    counter++;
    return counter;
}

char** storeCode(FILE* file, unsigned *numberOfLines){
    // memory allocation for the code lines array, considering current number of lines
    char** codeLines = stringMalloc(*numberOfLines);
    unsigned deletedLines = 0;
    for(unsigned i = 0; i < *numberOfLines; i++){
        // temporary string to store line
        char tempLine[BUF_SIZE_LINE];

        // gets next line from file
        fgets(tempLine, BUF_SIZE_LINE, file);

        // check if it's empty
        if(checkEmptyString(tempLine)){
            // if it's empty, ignore it
            deletedLines++;
        }
        else{
            // if it's not empty, remove comments and line breaks and trim leading whitespaces
            tempLine[strcspn(tempLine, "\n")] = 0;
            tempLine[strcspn(tempLine, "#")] = '\0';
            trimLeadingWhitespaces(tempLine);
            // store it in the array
            strcpy(codeLines[i - deletedLines], tempLine);
        }
    }

    // if there were any deleted lines, realloc memory
    if(deletedLines > 0){
        // adjust line array size
        *numberOfLines = *numberOfLines - deletedLines;
        for(unsigned i = 0; i < deletedLines; i++){
            free(codeLines[i+(*numberOfLines)]);
        }
        codeLines = (char **)realloc(codeLines, *numberOfLines * sizeof(char*));
    }

    return codeLines;
}

char** readFile(unsigned *numberOfLines, char* fileName) {
    FILE *file;

    // gets source file from the assemble() function
    fileName[strcspn(fileName, "\n")] = 0;
    
    // opens assembly source file
    file = fopen(fileName, "r");
    if(file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // updates line counter, considering comments. line breaks and whitespaces
    *numberOfLines = countLines(file);
    
    // rewind file to read lines
    rewind(file);

    // store clean assembly code lines into an array
    char **lines = storeCode(file, numberOfLines);

    fclose(file);

    return lines;
}

void writeFile(unsigned data[], unsigned numberOfLines, char *outputDir){
    FILE *file;
    file = fopen(outputDir, "wb");
    fwrite(data, sizeof(unsigned), numberOfLines, file);

    fclose(file);
}