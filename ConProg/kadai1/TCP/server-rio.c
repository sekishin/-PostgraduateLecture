#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <termios.h>	// 端末制御

#define	SERV_TCP_PORT 20000

struct termios saved_options;

void set_rawterm(void)
{
  struct termios new_options;

  tcgetattr(fileno(stdin), &saved_options);
  new_options = saved_options;
  new_options.c_lflag &= ~(ICANON | ECHO);		// カノニカルモードオフ, エコーオフ
  new_options.c_cc[VTIME] = 0;	  			// バッファリングのタイムアウトを0
  new_options.c_cc[VMIN] = 1;				// バッファサイズを1
  tcsetattr(fileno(stdin), TCSANOW, &new_options);
}

void reset_terminal(void)
{
  tcsetattr(fileno(stdin), TCSANOW, &saved_options);      // 保存していた設定に戻す
}

void err_msg(char *msg)
{
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[])
{
  int port_no;
  int sockid;
  int sockfd;
  struct sockaddr_in serv_addr, cli_addr;
  socklen_t cli_len = sizeof(cli_addr);
  int rflag, wflag, endflag, iobytes;
  char sch, rch;

  atexit(reset_terminal);	// 終了時に端末の設定を戻す
  set_rawterm();		// 端末の設定を raw モードへ

  port_no = (argc > 1) ? atoi(argv[1]) : SERV_TCP_PORT;
  printf("待ち受けのポート番号は %d です．Ctrl-A を押すと終了します．\n", port_no);
  if ((sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    err_msg("srever: can't open datastream socket");
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family	  = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port	  = htons(port_no);

  if (bind(sockid, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    err_msg("srever: can't bind local address");
  }
  
  if (listen(sockid, 5) == -1) {
    err_msg("srever: listen failed");
  }
  
  if ((sockfd = accept(sockid, (struct sockaddr *)&cli_addr, &cli_len)) < 0) {
    err_msg("server: can't accept");
  }
  
  fcntl(fileno(stdin), F_SETFL, O_NONBLOCK);
  fcntl(fileno(stdout), F_SETFL, O_NONBLOCK);
  fcntl(sockfd, F_SETFL, O_NONBLOCK);
  rflag = 1; wflag = 0;
  while (1) {
    if (rflag == 1) {
      if (read(fileno(stdin), &sch, 1) == 1) {
	if (sch == '\001') {
	  endflag = 0;
	  break;	/* ^A */
	}
	rflag = 0;
      }
    }
    if (rflag == 0) {
      if ((iobytes = write(sockfd, &sch, 1)) == 1) {
	rflag = 1;
      } else if (iobytes == -1) {
	if (errno != EAGAIN) {
	  perror("write");
	  break;
	}
      }
    }
    if (wflag == 0) {
      if ((iobytes = read(sockfd, &rch, 1)) == 1) {
	if (rch == '\001') {
	  endflag = 1;
	  break;	/* ^A */
	}
	wflag = 1;
      } else if (iobytes == -1) {
	if (errno != EAGAIN) {
	  perror("read");
	  break;
	}
      }
    }
    if (wflag == 1) {
      if (write(fileno(stdout), &rch, 1) == 1) {
	wflag = 0;
      }
    }
  }
  if (endflag == 0) {
    while (write(sockfd, &sch, 1) != 1);
  }
  close(sockfd);
  close(sockid);
  return 0;
}
