#include <stdio.h>

unsigned extract_lsbits(unsigned num, unsigned x) {
    printf("First %d bits of integer %d are: ", x, num);
    for (int i = x-1; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
    }
    printf("\n");
    return 0;
}

void main() {
    //addi
    unsigned op = 8;
    unsigned rs = 0;
    unsigned rt = 16;
    unsigned imm = 40;
    extract_lsbits(op, 6);
    extract_lsbits(rs, 5);
    extract_lsbits(rt, 5);
    extract_lsbits(imm, 16);
}