#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void error(char *msg) {
  puts(msg);
  exit(1);
}
int main(int argc, char const *argv[]) {
  char filename[256];
  FILE *fp;
  int n;
  int t;
  char buff[256];
  char *p;
  if (argc < 2) error("input log file name");
  strcpy(filename, argv[1]);
  fp = fopen(filename, "r");
  if (fp == NULL) error("illigal file name");
  while(fgets(buff, sizeof(buff), fp) != NULL) {
    p = strstr(buff, "#item");
    if (p  != NULL) {
      t = sscanf(p, "%*s %*s %d", &n);
      if (t == 1) printf("%d\n", n);
    }
  }
  fclose(fp);
  return 0;
}
