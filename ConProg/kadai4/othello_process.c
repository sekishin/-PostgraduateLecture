#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <time.h>
#include "othello.h"


int main(int argc, char const *argv[]) {
	//-- 宣言
    Board brd;          // 盤面
    bool passBLK, passWHT;
    THands t_hands;

    //-- 盤面入力
    brd = brd_init();  // brdのアドレスを渡す
    brd_output(brd, BLK, &t_hands);  // brdのデータを渡す

    //-- ゲームループ
    while ( ! end_check(brd) ) {
        player(&brd,BLK,&passBLK, t_hands);
        brd_output(brd, WHT, &t_hands);
        if ( passBLK && passWHT ) { break; }
        player(&brd,WHT,&passWHT, t_hands);
        brd_output(brd, BLK, &t_hands);
        if ( passBLK && passWHT ) { break; }
    }

    //-- 勝敗判定
    winner_check(brd);

    //-- 終了
    return 0;

}

