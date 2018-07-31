#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include "othello.h"

#define SERV_TCP_PORT 20471

void err_msg(char *msg)
{
  puts(msg);
  exit(1);
}

int main(int argc, char *argv[])
{
  int port_no;
  int ipaddr;
  int sockfd;
  struct sockaddr_in serv_addr;
  struct hostent *retrieve;
  GameState gs;
  THands t_hands;

  if (argc < 2) {
    err_msg("client: input server name");
  }

  port_no = SERV_TCP_PORT;
  if ((retrieve = gethostbyname(argv[1])) == NULL) {
    err_msg("client: unknown host name");
  }

  ipaddr = *(unsigned int *)(retrieve->h_addr_list[0]);
  printf("%d.%d.%d.%d (%d) にアクセスします．\n",ipaddr & 0xff, (ipaddr >> 8) & 0xff, (ipaddr >> 16) & 0xff, (ipaddr >> 24) & 0xff,port_no);

  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = *(unsigned int *)(retrieve->h_addr_list[0]);
  serv_addr.sin_port = htons(port_no);

  // サーバに接続
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    err_msg("client: can't open datastream socket");
  }
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    err_msg("client: can't connect server address");
  }
  // メッセージを受信
  read(sockfd, &gs, sizeof(GameState));    // 初期盤面受け取り
  brd_output(gs, &t_hands);    // 初期盤面出力
  while (1) {
    player(&gs, t_hands);  // ハンド入力、盤面更新
    brd_output_simple(gs);
    write(sockfd, &gs, sizeof(GameState));    // 更新盤面送信
    if (gs.isEnd) break;
    read(sockfd, &gs, sizeof(GameState));    // 更新盤面受取
    brd_output(gs, &t_hands);
    if (gs.isEnd) break;
  }
  puts("Game Over");
  //  コネクション切断
  close(sockfd);
  return 0;
}
