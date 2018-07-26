#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

#define SERV_TCP_PORT 20000
#define RECORD_SIZE 100

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
    int loop_num;   // 繰り返し回数
    char idnt[40]; //  識別子
    char idnt1[40]; // 識別子1
    char idnt2[40]; // 識別子2
    int num;
    int i; // 反復変数 
    char inbuff[RECORD_SIZE];   // 受信メッセージ用バッファ
    char outbuff[RECORD_SIZE];  // 送信メッセージ用バッファ
    
    if (argc < 2) {
        err_msg("client: input server name");
    } else if (argc < 3) {
        err_msg("client: input id sequence");
    } else if (argc < 4) {
        err_msg("client: input loop time");
    } else if (strlen(argv[2]) > 35) {
        err_msg("client: too long id sequence");
    }

    port_no = SERV_TCP_PORT;
    strcpy(idnt, argv[2]);
    loop_num = atoi(argv[3]);
    if ((retrieve = gethostbyname(argv[1])) == NULL) {
        err_msg("client: unknown host name");

    }
    ipaddr = *(unsigned int *)(retrieve->h_addr_list[0]);
    printf("%d.%d.%d.%d (%d) にアクセスします．\n",ipaddr & 0xff, (ipaddr >> 8) & 0xff, (ipaddr >> 16) & 0xff, (ipaddr >> 24) & 0xff,port_no);
  
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = *(unsigned int *)(retrieve->h_addr_list[0]);
    serv_addr.sin_port = htons(port_no);
  
  

    for (i = 0; i < loop_num; i++) {
        // サーバに接続
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            err_msg("client: can't open datastream socket");
        }
        if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            err_msg("client: can't connect server address");
        }
        // メッセージを受信
        read(sockfd, inbuff, RECORD_SIZE);
        // メッセージを表示
        printf("from server: %s\n", inbuff);
        sscanf(inbuff, "%d %s %s", &num, idnt1, idnt2);
        // 一時待機
        sleep(1);
        // メッセージを送信
        memset(outbuff, 0x0, RECORD_SIZE);
        snprintf(outbuff, RECORD_SIZE, "%d %s %s", num+1, idnt, idnt1);
        printf("send server: %s\n", outbuff);
        write(sockfd, outbuff, RECORD_SIZE);
        //  コネクション切断
        close(sockfd);
        // 一時待機
        sleep(1);
    }
    //----
    close(sockfd);
    return 0;
}
