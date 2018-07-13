#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERV_TCP_PORT 20000
#define RECORD_SIZE 100

void err_msg(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    char *file_name;    // 対象ファイル
    FILE *fp;   // ファイルポインタ
    int port_no;    // ポート番号
    int sockid; // ソケットIDa
    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    int pid;
    char buf[RECORD_SIZE];
    int i, size;
    int fd;
    char *old_idnt;

    if (argc < 1) {
        err_msg("error : no file_name");
    }
    
    // ファイル作成
    strcopy(file_name, argv[1]);
    fp = fopen(file_name, "w");
    if (fp == NULL) {
        err_msg("server : can't open file");
    }
    fclose(fp)
    
    // ソケット生成
    port_no = atoi(file_name);
    printf("待ち受けポートは %d 番です\n", port_no);
    if ((sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err_msg("server : can't open datastream socket");
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port_np);
    
    // ソケット登録    
    if (bind(sockid, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        err_msg("server : can't bind local address");
    }
    
    // ソケット待ち受け
    if (listen(sockid, 5) == -1) {
        err_msg("server : listen fails")
    }

    while (1) {
        break;
    }

    return 0;
}
