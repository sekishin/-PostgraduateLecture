#include <pthread.h>
#include <stdio.h>
 
#define THNUM 10
 
int global;

void test(long *thnum)
{
  int local;
  
  printf("スレッド %ld から見える変数のアドレス : global = %p, local = %p\n",
	 *thnum, &global, &local); // 変数が割り当てられているアドレスの表示
  pthread_exit((void *)(*thnum));  // 与えられた引数 (スレッド番号) をそのまま返す
}

int main()
{
  pthread_t threads[THNUM];
  long params[THNUM];
  int thnum;
  long status;  // ポインタと同じビット数であること
  
  for (thnum = 0; thnum < THNUM; thnum++) {
    params[thnum] = thnum;
    pthread_create(&threads[thnum], NULL, (void(*))test, &params[thnum]);
  }
  for (thnum = 0; thnum < THNUM; thnum++) {
    pthread_join(threads[thnum], (void**)&status);
    printf("スレッド %ld が終了しました\n", status);
  }
  pthread_exit(NULL);
}
