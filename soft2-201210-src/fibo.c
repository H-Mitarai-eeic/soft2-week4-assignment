#include<stdio.h>
#include<stdlib.h>

#define D 2

int fibo(int n);
void cal_A(const int n, int A[D][D]);
void dot(int N1, int N2, int N3, const int mat1[][N2], const int mat2[][N3], int dst[][N3]);
void copy(int N1, int N2, const int mat[][N2], int dst[][N2]);
void print_mat(int N1, int N2, int mat[][N2]);

int main(void){
    int n = 0;
    int f;
    while(n != -1){
        printf("input int num (-1 for quit): ");
        scanf("%d", &n); 
        f = fibo(n);
        printf("%d -> %d\n", n, f);
    }
    return 0;
}
int fibo(int n){
    int F_init[D][1] = {1, 1};
    int F_fin[D][1];
    int A[D][D];

    cal_A(n - 1, A);
    dot(D, D, 1, A, F_init, F_fin);

    return F_fin[0][0];

}
void cal_A(const int n, int A[D][D]){
    //printf("n = %d\n", n);
    int (*A_temp1)[D];
    int (*A_temp2)[D];

    A_temp1 = (int (*)[D])malloc(sizeof(int) * D * D);
    A_temp2 = (int (*)[D])malloc(sizeof(int) * D * D);

    if(n <= 0){
        int I[D][D] = {{1, 0}, {0, 1}}; //単位行列
        copy(D, D, I, A);
    }
    else if(n % 2 == 0){
        cal_A(n/2, A_temp1);
        dot(D, D, D, A_temp1, A_temp1, A); 
    }
    else if(n % 2 == 1){
        int A1[D][D] = {{1, 1}, {1, 0}};
        cal_A((n-1)/2, A_temp1);
        dot(D, D, D, A_temp1, A_temp1, A_temp2);
        dot(D, D, D, A_temp2, A1, A); 
    }
    free(A_temp1);
    free(A_temp2);
    //print_mat(D, D, A);
}
void dot(int N1, int N2, int N3, const int mat1[][N2], const int mat2[][N3], int dst[][N3]) {
    // ここに、N1 x N2 の行列mat1, N2 x N3 の行列mat2、N1 x N3 の行列dstを受け取り、

    int i, j, k;

    for(i = 0; i < N1; i++){
        for(j = 0; j < N3; j++){
            dst[i][j] = 0; //dstの初期化
            for(k = 0; k < N2; k++){
                dst[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }
}
void copy(int N1, int N2, const int mat[][N2], int dst[][N2]) {
    // ここに、N1 x N2 の行列matおよびdstを受け取り、matの内容をdstにコピーするコードを書いてください。
    int i, j;
    for(i = 0; i < N1; i++){
        for(j = 0; j < N2; j++){
            dst[i][j] = mat[i][j];
        }
    }
}
void print_mat(int N1, int N2, int mat[][N2]) {
    // ここに、縦N1 x 横N2の大きさの行列matの中身を表示するコード
    int i, j;

    for(i = 0; i < N1; i++){
        for(j = 0; j < N2; j++){
            if(j == N2 - 1){
                printf("%d\n", mat[i][j]);
            }
            else{
                printf("%d ", mat[i][j]);
            }
        }
    }
}