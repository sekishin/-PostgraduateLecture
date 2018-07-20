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
    char ch;
    char buff[RECORD_SIZE];
    int chnum = 0;
    int status;

    if (argc < 1) {
        err_msg("error : no file_name");
    }
    
    // ファイル作成
    strcpy(file_name, argv[1]);
    fp = fopen(file_name, "w");
    if (fp == NULL) {
        err_msg("server : can't open file");
    }
    fclose(fp);
    
    // ソケット生成
    port_no = atoi(file_name);
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
                fprintf(stderr, "Terminate child process: %d\n", pid);
                chnum--;
            }
        }
        // コネクション受付
        if ((sockfd = accept(sockid, (struct sockaddr *)&cli_addr, &cli_len)) < 0) {
            close(sockid);
            fprintf(stderr, "server: can't accept");
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
        pid = getpid();
        fprintf(stderr, "\nI am child process&d\n",pid);
        if (pid == 0) {
            close(sockid);  // コネクション待ちのソケットを閉じる
            fp = fopen(file_name, O_RDWR | O_CREAT, 0644);  // ファイルオープン
            if (fp == NULL) err_msg("server: can't open file");
            if (flock(fp, LOCK_EX) == -1) { // ファイルロック
                fclose(fp)
                err_msg("server: can't lock file");
            }
            fseek(fp, 0, SEEK_END);
            long size = ftell(fp);  // ファイルサイズ取得
            if (size == 0) {
                //メッセージを 0 NONE NONE に
                sprintf(buff, "0 NONE NONE");
            } else {
                // ファイル末尾のメッセージに
            }
            // クライアントからメッセージが届くまで待つ
            // 届いたメッセージをファイルに追記
            // ファイルのロックを解除
            // ファイルクローズ
            // プロセス修了

        }
    }
    
    while (chnum) {
        if (waitpid(-1, &status, WNOHANG) > 0) chnum--;
    }

    return 0;
}
