#include<stdio.h>

#define SIZE 4

int main(void){
    char filename[] = "helloworld.txt";
    FILE *fp;
    float f[SIZE] = {0};
    int i;

    fp = fopen(filename, "rb");
    if(fp == NULL){
        printf("cannot open file named '%s'\n", filename);
        return 1;
    }

    printf("%ld\n", fread(f, sizeof(float), SIZE, fp));

    for(i = 0; i < SIZE; i++){
        printf("f[%d] = %.200f\n", i, f[i]);
    }
    fclose(fp);
    printf("%s",(char*)f);
    return 0;
}