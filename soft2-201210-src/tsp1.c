#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h> // strtol のエラー判定用

//#define M 100 //初期解の数

// 町の構造体（今回は2次元座標）を定義
typedef struct
{
  int x;
  int y;
} City;

// 描画用
typedef struct
{
  int width;
  int height;
  char **dot;
} Map;

// 整数最大値をとる関数
int max(const int a, const int b)
{
  return (a > b) ? a : b;
}

// プロトタイプ宣言
// draw_line: 町の間を線で結ぶ
// draw_route: routeでの巡回順を元に移動経路を線で結ぶ
// plot_cities: 描画する
// distance: 2地点間の距離を計算
// solve(): TSPをといて距離を返す/ 引数route に巡回順を格納

void draw_line(Map map, City a, City b);
void draw_route(Map map, City *city, int n, const int *route);
void plot_cities(FILE* fp, Map map, City *city, int n, const int *route);
double distance(City a, City b);
double solve(const City *city, int n, int *route, int *visited, int M);
double solve_not_print(const City *city, int n, int *route, int *visited, int M);
Map init_map(const int width, const int height);
void free_map_dot(Map m);
City *load_cities(const char* filename,int *n);

void gen_initial_rote(int n, int *route_init, int *visited);
double climbing(const City *city, int n, const int *route_init, int *route_sol);
void swap_n_cpy(int n, int a, int b, const int *origin, int *to);
void cpy_list(int n, const int *origin, int *to);
double total_distance(const City *city, int n, const int *route);

Map init_map(const int width, const int height)
{
  char **dot = (char**) malloc(width * sizeof(char*));
  char *tmp = (char*)malloc(width*height*sizeof(char));
  for (int i = 0 ; i < width ; i++)
    dot[i] = tmp + i * height;
  return (Map){.width = width, .height = height, .dot = dot};
}
void free_map_dot(Map m)
{
  free(m.dot[0]);
  free(m.dot);
}

City *load_cities(const char *filename, int *n)
{
  City *city;
  FILE *fp;
  if ((fp=fopen(filename,"rb")) == NULL){
    fprintf(stderr, "%s: cannot open file.\n",filename);
    exit(1);
  }
  fread(n,sizeof(int),1,fp);
  city = (City*)malloc(sizeof(City) * *n);
  for (int i = 0 ; i < *n ; i++){
    fread(&city[i].x, sizeof(int), 1, fp);
    fread(&city[i].y, sizeof(int), 1, fp);
  }
  fclose(fp);
  return city;
}
int main(int argc, char**argv)
{
    // const による定数定義
    const int width = 70;
    const int height = 40;
    const int max_cities = 100;

    int M; //初期解の数
    int seed; // 初期解配置のためのシード
    int print_yn;

    Map map = init_map(width, height);
    
    FILE *fp = stdout; // とりあえず描画先は標準出力としておく
    if (argc != 2){
        fprintf(stderr, "Usage: %s <city file>\n", argv[0]);
        exit(1);
    }
    int n;

    City *city = load_cities(argv[1],&n);
    assert( n > 1 && n <= max_cities); // さすがに都市数100は厳しいので

    //初期解の数とseedを受付    
    printf("初期解の数 : ");
    scanf("%d", &M);

    printf("乱数のseed(初期解の配置用) : ");
    scanf("%d", &seed);

    printf("局所最適解を逐次表示する -> 1 / しない -> 1以外 : ");
    scanf("%d", &print_yn);

    srand(seed);

    // 町の初期配置を表示
    plot_cities(fp, map, city, n, NULL);
    sleep(1);

    // 訪れる順序を記録する配列を設定
    int *route = (int*)calloc(n, sizeof(int));
    // 訪れた町を記録するフラグ
    int *visited = (int*)calloc(n, sizeof(int));

    double d;
    if(print_yn == 1){
      d = solve(city, n, route, visited, M);
    }else{
      d = solve_not_print(city, n, route, visited, M);
    }
    plot_cities(fp, map, city, n, route);
    printf("total distance = %f\n", d);
    for (int i = 0 ; i < n ; i++){
        printf("%d -> ", route[i]);
    }
    printf("0\n");

    // 動的確保した環境ではfreeをする
    free(route);
    free(visited);
    free(city);
    
    return 0;
}

// 繋がっている都市間に線を引く
void draw_line(Map map, City a, City b)
{
  const int n = max(abs(a.x - b.x), abs(a.y - b.y));
  for (int i = 1 ; i <= n ; i++){
    const int x = a.x + i * (b.x - a.x) / n;
    const int y = a.y + i * (b.y - a.y) / n;
    if (map.dot[x][y] == ' ') map.dot[x][y] = '*';
  }
}

void draw_route(Map map, City *city, int n, const int *route)
{
  if (route == NULL) return;

  for (int i = 0; i < n; i++) {
    const int c0 = route[i];
    const int c1 = route[(i+1)%n];// n は 0に戻る必要あり
    draw_line(map, city[c0], city[c1]);
  }
}

void plot_cities(FILE *fp, Map map, City *city, int n, const int *route)
{
  fprintf(fp, "----------\n");

  memset(map.dot[0], ' ', map.width * map.height); 

  // 町のみ番号付きでプロットする
  for (int i = 0; i < n; i++) {
    char buf[100];
    sprintf(buf, "C_%d", i);
    for (int j = 0; j < strlen(buf); j++) {
      const int x = city[i].x + j;
      const int y = city[i].y;
      map.dot[x][y] = buf[j];
    }
  }

  draw_route(map, city, n, route);

  for (int y = 0; y < map.height; y++) {
    for (int x = 0; x < map.width; x++) {
      const char c = map.dot[x][y];
      fputc(c, fp);
    }
    fputc('\n', fp);
  }
  fflush(fp);
}

double distance(City a, City b)
{
  const double dx = a.x - b.x;
  const double dy = a.y - b.y;
  return sqrt(dx * dx + dy * dy);
}
void gen_initial_rote(int n, int *route_init, int *visited){
    int i = 1, j = 0, k = 0;
    double R;

    for(k = 0; k < n; k++){
        visited[k] = 0;
    }
    route_init[0] = 0;
    visited[0] = 1;

    while(i < n){
        R = (double)rand() / RAND_MAX * n;
        for(j = 1; j < n; j++){
            if(R >= j - 1 && R < j){
                if(visited[j] == 0){
                    route_init[i] = j;
                    visited[j] = 1;
                    i++;
                    break;
                }
                else{
                    break;
                }
            }
        }
    }
}

double solve(const City *city, int n, int *route_sol, int *visited_sol, int M){
    // 以下はとりあえずダミー。ここに探索プログラムを実装する
    // 現状は町の番号順のルートを回っているだけ
    // 実際は再帰的に探索して、組み合わせが膨大になる。
    int i = 0;
    double dis_temp, min_dis;
    int *route_init, *route_sol_temp, *visited_temp;
    route_init = (int*) malloc(sizeof(int) * n);
    route_sol_temp = (int*) malloc(sizeof(int) * n);
    visited_temp = (int*) malloc(sizeof(int) * n);

    gen_initial_rote(n, route_init, visited_sol);
    min_dis = climbing(city, n, route_init, route_sol);

    printf("local optimum solution %d : total distance %f\n route: ", i, min_dis);
    for (int i = 0 ; i < n ; i++){
        printf("%d->", route_sol[i]);
    }
    printf("0\n");

    for(i = 1; i < M; i++){
        gen_initial_rote(n, route_init, visited_temp);
        dis_temp = climbing(city, n, route_init, route_sol_temp);
        
        printf("local optimum solution %d ; total distance %f\n route: ", i, dis_temp);
        for (int i = 0 ; i < n ; i++){
            printf("%d->", route_sol_temp[i]);
        }
        printf("0\n");
        
        if(dis_temp < min_dis){
            min_dis = dis_temp;
            cpy_list(n, route_sol_temp, route_sol);
            cpy_list(n, visited_temp, visited_sol);
        }
    }

    free(route_init);
    free(route_sol_temp);
    free(visited_temp);
    return min_dis;
}
double solve_not_print(const City *city, int n, int *route_sol, int *visited_sol, int M){
    // 以下はとりあえずダミー。ここに探索プログラムを実装する
    // 現状は町の番号順のルートを回っているだけ
    // 実際は再帰的に探索して、組み合わせが膨大になる。
    int i = 0;
    double dis_temp, min_dis;
    int *route_init, *route_sol_temp, *visited_temp;
    route_init = (int*) malloc(sizeof(int) * n);
    route_sol_temp = (int*) malloc(sizeof(int) * n);
    visited_temp = (int*) malloc(sizeof(int) * n);

    gen_initial_rote(n, route_init, visited_sol);
    min_dis = climbing(city, n, route_init, route_sol);

    for(i = 1; i < M; i++){
        gen_initial_rote(n, route_init, visited_temp);
        dis_temp = climbing(city, n, route_init, route_sol_temp);
        
        if(dis_temp < min_dis){
            min_dis = dis_temp;
            cpy_list(n, route_sol_temp, route_sol);
            cpy_list(n, visited_temp, visited_sol);
        }
    }

    free(route_init);
    free(route_sol_temp);
    free(visited_temp);
    return min_dis;
}
double climbing(const City *city, int n, const int *route_init, int *route_sol){
    //printf("1\n");
    int i, j;
    double dis_init, min_dis, dis_temp;
    int *route_new_init, *route_temp;
    route_new_init = (int*) malloc(sizeof(int) * n);
    route_temp = (int*) malloc(sizeof(int) * n);

    cpy_list(n, route_init, route_sol);
    dis_init = total_distance(city, n, route_init);
    min_dis = dis_init;
    for(i = 1; i < n - 1; i++){
        for(j = i + 1; j < n; j++){
            swap_n_cpy(n, i, j, route_init, route_temp);
            dis_temp = total_distance(city, n, route_temp);
            if(dis_temp < min_dis){
                min_dis = dis_temp;
                cpy_list(n, route_temp, route_new_init);
            }
        }
    }
    if(min_dis < dis_init){
        min_dis = climbing(city, n, route_new_init, route_sol);
    }
    
    free(route_new_init);
    free(route_temp);
    return min_dis;
}
void swap_n_cpy(int n, int a, int b, const int *origin, int *to){
    //長さn の配列の インデックスa と b を入れ替えたものを to に入れる
    int i;
    for(i = 0; i < n; i++){
        to[i] = origin[i];
    }
    to[a] = origin[b];
    to[b] = origin[a];
}
void cpy_list(int n, const int *origin, int *to){
    //長さn の配列をtoにコピー
    int i;
    for(i = 0; i < n; i++){
        to[i] = origin[i];
    }
}
double total_distance(const City *city, int n, const int *route){
    double sum_d = 0;
    for (int i = 0 ; i < n ; i++){
        const int c0 = route[i];
        const int c1 = route[(i+1)%n]; // nは0に戻る
        sum_d += distance(city[c0],city[c1]);
    }
    return sum_d;
}

