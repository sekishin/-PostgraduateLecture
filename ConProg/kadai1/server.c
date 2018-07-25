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

#define SERV_TCP_PORT 20000
#define RECORD_SIZE 100

void err_msg(char *msg)
{
    puts(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    char file_name[64];    // 対象ファイル
    int fp;   // ファイルポインタ
    int port_no;    // ポート番号
    int sockid; // ソケットIDa
    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    int pid;
    char outbuff[RECORD_SIZE];  // 送信メッセージ用バッファ
    char inbuff[RECORD_SIZE];   // 受信メッセージ用バッファ
    int chnum = 0;
    int size;
    int status;
    int num;
    char idnt1[40]; // 識別子1
    char idnt2[40]; // 識別子2

    if (argc <= 1) {
        err_msg("error : no file_name");
    }
    
    // ファイル作成
    strcpy(file_name, argv[1]);
    fclose(fopen(file_name, "w"));
    
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
            //fprintf(stderr, "server: can't accept");
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
        fprintf(stderr, "\nI am child process%d\n", getpid());
        if (pid == 0) {
            close(sockid);  // コネクション待ちのソケットを閉じる
            fp = open(file_name, O_RDWR | O_CREAT, 0644);  // ファイルオープン
#ifdef WITH_FLOCK
            if (flock(fp, LOCK_EX) == -1) { // ファイルロック
                close(fp)
                err_msg("server: can't lock file");
            }
#endif
            lseek(fp, -(RECORD_SIZE+1), SEEK_END);
            size = read(fp, inbuff, RECORD_SIZE);  // ファイルサイズ取得(この時点でファイル末尾のデータはinbuffに格納される)
            memset(outbuff, 0x0, RECORD_SIZE);  // パティング
            if (size == 0) {
                //メッセージを 0 NONE NONE に
                sprintf(outbuff, "0 NONE NONE");
            } else {
                // ファイル末尾のメッセージに
                sprintf(outbuff, "%s", inbuff);
            }
            printf("send client: %s\n", outbuff);
            write(sockfd, outbuff, RECORD_SIZE);  // メッセージをクライアントに送る
            read(sockfd, inbuff, RECORD_SIZE);  // クライアントからメッセージが届くまで待つ
            // 届いたメッセージをファイルに追記
            printf("from client: %s\n", inbuff);
            lseek(fp, 0, SEEK_END);
            write(fp, inbuff, RECORD_SIZE);
            write(fp, "\n", 1);
#ifdef WITH_FLOCK
            flock(fp, LOCK_UN);  // ファイルのロックを解除
#endif
            close(fp); // ファイルクローズ
            close(sockfd);  // プロセス修了

        }
    }
    
    return 0;
}
