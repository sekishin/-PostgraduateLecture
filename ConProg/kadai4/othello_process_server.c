#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/file.h>
#include <time.h>
#include "othello.h"

#define SERV_TCP_PORT 20471

void AI(GameState *gs, THands t_hands);

void err_msg(char *msg)
{
  puts(msg);
  exit(1);
}

int main(int argc, char const *argv[])
{
  //-- 宣言
  Board brd;          // 盤面
  bool passBLK, passWHT;
  THands t_hands;
  int port_no;    // ポート番号
  int sockid; // ソケットIDa
  int sockfd;
  struct sockaddr_in serv_addr, cli_addr;
  socklen_t cli_len = sizeof(cli_addr);
  int pid;
  int chnum = 0;
  int status;

  srand((unsigned)time(NULL));

  // ソケット生成
  port_no = SERV_TCP_PORT;
  printf("待ち受けポートは %d 番です\n", port_no);
  if ((sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    err_msg("server : can't open datastream socket");
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port_no);

  // ソケット登録
  if (bind(sockid, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    err_msg("server : can't bind local address");
  }

  // ソケット待ち受け
  if (listen(sockid, 5) == -1) {
    err_msg("server : listen fails");
  }

  while (1) {
    // 子プロセスの生成
    if (chnum) {
      while(chnum && ((pid = waitpid(-1, &status, WNOHANG)) > 0)) {
        //fprintf(stderr, "Terminate child process: %d\n", pid);
        chnum--;
      }
    }
    // コネクション受付
    if ((sockfd = accept(sockid, (struct sockaddr *)&cli_addr, &cli_len)) < 0) {
      close(sockid);
      fprintf(stderr, "server: close process %d\n", getpid());
      break;
    }
    // fork実行
    pid = fork();
    if (pid < 0) {  // fork error
      close(sockfd);
      close(sockid);
      break;
    } else if (pid > 0) {   // parent process
      // 通信用のソケットを閉じ、次のループへ
      close(sockfd);
      chnum++;
      continue;
    }

    // child process
    fprintf(stderr, "Terminate child process: %d\n", getpid());
    //fprintf(stderr, "\nI am child process%d\n", getpid());
    if (pid == 0) {
      close(sockid);  // コネクション待ちのソケットを閉じる
      GameState gs = game_init();   // 初期盤面の生成
      Hand hand;
      write(sockfd, &gs, sizeof(GameState));    // 初期盤面を送る
      while (1) {
        read(sockfd, &gs, sizeof(GameState));    // 更新盤面の受け取り
        fprintf(stderr, "\nI am child process%d\n", getpid());
        puts("from client");
        brd_output(gs, &t_hands);
        if (gs.isEnd) break;
        AI(&gs, t_hands);    // AI実行
        fprintf(stderr, "\nI am child process%d\n", getpid());
        puts("send client");
        brd_output_simple(gs);
        turn_change(&gs);
        write(sockfd, &gs, sizeof(GameState));    // 盤面送る
        if (gs.isEnd) break;
      }
      puts("Game Over");
      close(sockfd);  // プロセス修了
      fprintf(stderr, "\nchild process%d finished\n", getpid());
      break;
    }
  }

  //-- 終了
  return 0;
}

void AI(GameState *gs, THands t_hands) {
  //-- 宣言
  Hand hand;
  int num;

  //( gs->turn == BLK ) ? printf("黒のターン\n") : printf("白のターン\n");

  //-- パス判定
  if ( pass_check(gs->brd, gs->turn) ) {
    printf("打てる場所がありません。パスします。\n");
    if (gs->isPass) {
      puts("連続パス");
      gs->isEnd = true;
    }
    gs->isPass = true;
    return;
  }
  printf("AI's Hand > ");
  //-- 打ち手決定（乱数）
  while (1) {
    num = rand()%t_hands.size;
    hand = t_hands.targets[num];
    if ( brd_turn(&(gs->brd), hand) ) {   // 盤面の更新と判定
      break;
    }
  }
  printf("%d\n", num);

  //-- 盤面更新
  brd_turn(&(gs->brd), hand);
  //printf("%d %d\n", hand.x, hand.y);
  gs->isPass = false;
  gs->isEnd = end_check(gs->brd);
  gs->last = hand;
}
