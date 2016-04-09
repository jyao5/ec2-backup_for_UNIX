#include<stdlib.h>
#include<stdio.h>

int main(){
  FILE *fp;
  char path[1035];

  fp = popen("ls b.out > /tmp/cstdin 2> /tmp/cstderr", "r");
  if (fp == NULL) {
    printf("C:Failed to run command\n" );
    exit(1);
  }

  while (fgets(path, sizeof(path)-1, fp) != NULL) {
    printf("%s", path);
  }

  /* close */
  pclose(fp);

  return 0;
}
