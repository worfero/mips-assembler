#include "mips.h"

int main(int argc, char *argv[]){
    char *asmDir = argv[1];
    char *outputDir = argv[2];
    assemble(asmDir, outputDir);
    return 0;
}