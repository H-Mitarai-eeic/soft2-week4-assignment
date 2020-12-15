#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h> // strtol, strtod, strerror
#include <errno.h> // strtol, strtod でerror を補足したい

// 以下は構造体の定義と関数のプロトタイプ宣言

// 構造体 Item
// 価値valueと重さweightが格納されている
//
typedef struct item
{
  double value;
  double weight;
}Item;

// 構造体 Itemset
// number分のItemを保持するための構造体
// Itemポインタをmallocする必要あり
typedef struct itemset
{
  int number;
  Item *item;
} Itemset;

struct solution{
  double val;
  unsigned char *flags;
};

// 関数のプロトサイプ宣言

// Itemset *init_itemset(int, int);
//
// itemsetを初期化し、そのポインタを返す関数
// 引数:
//  品物の個数: number (int)
//  乱数シード: seed (int) // 品物の値をランダムにする
// 返り値:
//  確保されたItemset へのポインタ

void free_itemset(Itemset *list);

// Itemset *load_itemset(char *filename)
//
// ファイルからItemset を設定し、確保された領域へのポインタを返す関数 [未実装, 課題1]
// 引数:
//  Itemsetの必要パラメータが記述されたバイナリファイルのファイル名 filename (char*)
// 返り値:
//  Itemset へのポインタ
Itemset *load_itemset(char *filename);

// void print_itemset(const Itemset *list)
//
// Itemsetの内容を標準出力に表示する関数
void print_itemset(const Itemset *list);

// void save_itemset(char *filename)
//
// Itemsetのパラメータを記録したバイナリファイルを出力する関数 [未実装, テスト用]
// 引数:
// Itemsetの必要パラメータを吐き出すファイルの名前 filename (char*)
// 返り値:
//  なし
void save_itemset(char *filename);

// double solve()
//
// ソルバー関数: 指定された設定でナップサック問題をとく [現状、未完成]
// 引数:
//   品物のリスト: Itemset *list
//   ナップサックの容量: capacity (double)
// 返り値:
//   最適時の価値の総和を返す
//
struct solution solve(const Itemset *list, double capacity);

// double search()
//
// 探索関数: 指定されたindex以降の組み合わせで、最適な価値の総和を返す
//  再帰的に実行する
// 引数:
//  指定index : index (int)
//  品物リスト: list (Itemset*)
//  ナップサックの容量: capacity (double)
//  実際にナップサックに入れた品物を記録するフラグ: flags (unsigned char*)
//  途中までの価値と重さ (ポインタではない点に注意): sum_v, sum_w
// 返り値:
//   最適時の価値の総和を返す
struct solution search(int index, const Itemset *list, double capacity, unsigned char *flags, double sum_v, double sum_w);

// エラー判定付きの読み込み関数
double load_double(const char *argvalue);

// main関数
// プログラム使用例: ./knapsack 10 20
//  10個の品物を設定し、キャパ20 でナップサック問題をとく
int main (int argc, char**argv)
{
    const int max_items = 100;
    const double W = load_double(argv[2]);; //max capacity
    Itemset *items;
    
  /* 引数処理: ユーザ入力が正しくない場合は使い方を標準エラーに表示して終了 */
    if ( argc != 3 ) {
        fprintf(stderr, "usage: %s [filename for init] [max capacity of a knapsack]\n", argv[0]);
        return EXIT_FAILURE;
    }
    else if (argc == 3) {
        items = load_itemset(argv[1]);
    }
    if(items == NULL){
        printf("error: failed to read binary data.\n");
        return 1;
    }
    print_itemset(items);
    assert( items->number <= max_items ); // assert で止める
    assert( W >= 0.0);
    printf("max capacity: W = %.f, # of items: %d\n",W, items->number);

    // ソルバーで解く
    struct solution sol = solve(items, W);

    // 表示する
    printf("----\nbest solution:\n");
    for (int i = 0 ; i < items->number ; i++){
        printf("%d", sol.flags[i]);
    }
    printf(", value: %4.1f\n",sol.val);
    free(sol.flags);
    free_itemset(items);
    return 0;
}

// 構造体をポインタで確保するお作法を確認してみよう

Itemset *load_itemset(char *filename){
    //失敗したらNULLを返す
    FILE *fp;
    Itemset *list = (Itemset*) malloc(sizeof(Itemset));
    Item *item;
    int num;
    int i;  //counter

    fp = fopen(filename, "rb");
    if(fp == NULL){
        return NULL;
    }

    if(fread(&num, sizeof(int), 1, fp) != 1){
        return NULL;
    }

    item = (Item*) malloc(sizeof(Item) * num);

    for(i = 0; i < num; i++){
        if(fread(&(item[i].value), sizeof(double), 1, fp) != 1){
            return NULL;
        }
    }
    for(i = 0; i < num; i++){
        if(fread(&(item[i].weight), sizeof(double), 1, fp) != 1){
            return NULL;
        }
    }

    list->item = item;
    list->number = num;

    fclose(fp);
    return list;
}
double load_double(const char *argvalue)
{
  double ret;
  char *e;
  errno = 0; // errno.h で定義されているグローバル変数を一旦初期化
  ret = strtod(argvalue,&e);
  if (errno == ERANGE){
    fprintf(stderr,"%s: %s\n",argvalue,strerror(errno));
    exit(1);
  }
  if (*e != '\0'){
    fprintf(stderr,"%s: an irregular character '%c' is detected.\n",argvalue,*e);
    exit(1);
  }
  return ret;
}

// itemset の free関数
void free_itemset(Itemset *list)
{
    free(list->item);
    free(list);
}

// 表示関数
void print_itemset(const Itemset *list)
{
  int n = list->number;
  const char *format = "v[%d] = %4.1f, v[%d] = %4.1f\n";
  for(int i = 0 ; i < n ; i++){
    printf(format, i, list->item[i].value, i, list->item[i].weight);
  }
  printf("----\n");
}

// ソルバーは search を index = 0 で呼び出すだけ
struct solution solve(const Itemset *list,  double capacity)
{
  // 品物を入れたかどうかを記録するフラグ配列 => !!最大の組み合わせが返ってくる訳ではない!!
  unsigned char *flags = (unsigned char*)calloc(list->number, sizeof(unsigned char));
  struct solution sol = search(0,list,capacity,flags, 0.0, 0.0);
  free(flags);
  return sol;
}

// 再帰的な探索関数
struct solution search(int index, const Itemset *list, double capacity, unsigned char *flags, double sum_v, double sum_w)
{
  int new_weight;
  int max_index = list->number;

  assert(index >= 0 && sum_v >= 0 && sum_w >= 0);
  // 必ず再帰の停止条件を明記する (最初が望ましい)
  if (index == max_index){
    struct solution sol;
    const char *format_ok = ", total_value = %5.1f, total_weight = %5.1f\n";
    const char *format_ng = ", total_value = %5.1f, total_weight = %5.1f NG\n";
    for (int i = 0 ; i < max_index ; i++){
      printf("%d", flags[i]);
    }
    if (sum_w <= capacity){
      printf(format_ok, sum_v, sum_w);
      sol.val = sum_v;
      sol.flags = (unsigned char*)malloc(list->number * sizeof(unsigned char));
      return sol;
    } else{
        sol.val = 0;
        sol.flags = NULL;
        printf(format_ng, sum_v, sum_w);
      return sol;
    }
  }

  // 以下は再帰の更新式: 現在のindex の品物を使う or 使わないで分岐し、index をインクリメントして再帰的にsearch() を実行する
  struct solution v0, v1;

  flags[index] = 0;
  v0 = search(index+1, list, capacity, flags, sum_v, sum_w);
  
  new_weight =sum_w + list->item[index].weight;
  if(new_weight > capacity){
    v0.flags[index] = 0;
    return v0;
  }
  else{
    flags[index] = 1;
    v1 = search(index+1, list, capacity, flags, sum_v + list->item[index].value, new_weight);
  }
  // 使った場合の結果と使わなかった場合の結果を比較して返す

  if(v0.val > v1.val){
    v0.flags[index] = 0;
    free(v1.flags);
  }
  else{
    v1.flags[index] = 1;
    free(v0.flags);
  }
  return (v0.val > v1.val) ? v0 : v1;
}