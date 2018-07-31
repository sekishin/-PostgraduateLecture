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
  GameState gs;
  THands t_hands;

  //-- 盤面入力
  gs = game_init();
  brd_output(gs, &t_hands);  // brdのデータを渡す

  //-- ゲームループ
  while ( 1 ) {
    player(&gs, t_hands);
    turn_change(&gs);
    brd_output(gs, &t_hands);
    if (gs.isEnd) break;
    player(&gs, t_hands);
    turn_change(&gs);
    brd_output(gs, &t_hands);
    if (gs.isEnd) break;
  }

  //-- 勝敗判定
  gs.turn = OUT;
  brd_output(gs, &t_hands);
  winner_check(gs.brd);

  //-- 終了
  return 0;

}
