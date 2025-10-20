#include <stdio.h>

int extract(int a){
    return (a & 0xF0) >> 4;
}

int main() {
    int a = 0x2020;
    int result = extract(a);

    printf("Input: %x", a);
    printf("Extracted bits 7-4: %x", result);
}