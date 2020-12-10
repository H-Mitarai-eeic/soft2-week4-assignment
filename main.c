#include<stdio.h>
#include<stdlib.h>
void test (int *a){
	printf("%d\n", (*a)++);
	test(a);
}
int main(int argc, char *argv[]){
    /*
    {
        if(argc != 2){
            fprintf(stderr, "usege: %s filename\n", argv[0]);
            return 1;
        }
        FILE *fp;
        if((fp = fopen(argv[1], "rb")) == NULL){
            perror(argv[1]);
            return 1;
        }
        char buf[100];
        size_t rsize = fread(buf, sizeof(char), 100, fp);

        printf("%zu Byte read\n", rsize);

        buf[rsize] = '\0';
        printf("%s", buf);

        return 0;
    }
    */
    /*
    {
        if(argc != 2){
            fprintf(stderr, "usege: %s filename\n", argv[0]);
            return 1;
        }
        FILE *fp;
        if((fp = fopen(argv[1], "rb")) == NULL){
            perror(argv[1]);
            return 1;
        }
        double *buf;
        size_t bsize, rsize;
        int i = 0;

        fread(&bsize, sizeof(double), 1, fp);
        buf = (double*) malloc(bsize * sizeof(double));
        rsize = fread(buf, sizeof(double), bsize, fp);

        printf("%zu Byte read\n", rsize);

        for(i = 0; i < rsize; i += rsize/10){
            printf("buf[%d] = %f\n", i, buf[i]);
        }
        free(buf);
        return 0;        
    }*/
    /*
    {
        unsigned short a = 65500;
        for (int b = 0 ; b < 16 ; b++){
            if(a & 1 << (15-b)){
                printf("1");
            }
            else{
                printf("0");
            }
	    }
	    printf("\n");
        // unsigned char型ポインタを用意する
        // 上記の先頭アドレスをキャストして代入
        unsigned char *p = (unsigned char*)&a;
        
        for (int i = 0 ; i < sizeof(unsigned short) ; i++){
            printf("%p\n", p+i);
            for (int b = 0 ; b < 8 ; b++){
                if(p[i] & 1 << (7-b)){
                    printf("1");
                }
                else{
                    printf("0");
                }
            }
            printf("\n");
        }
        return 0;
    }*/
    {
	int a = 0;
	test(&a);
	return 0;
    }
}