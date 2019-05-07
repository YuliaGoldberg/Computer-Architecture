#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void assFunc(int x, int y);

char c_checkValidity(int x, int y){
    if(x < 0 || y <= 0 || y > 32768)
        return 0;
    return 1;
}

int main(int argc, char**argv) {
    char firstNum[13]={0};
    char secondNum[13]={0};
    int first=0;
    int second=0;
    fgets(firstNum, 13, stdin);
    first = atoi(&firstNum[0]);
    fgets(secondNum, 1000, stdin);
    second = atoi(&secondNum[0]);
    assFunc(first, second);
    return 0;
}