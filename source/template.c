#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef int bool;
#define true 1;
#define false 0;

int main(int argc, char *argv[]){
  int i;
  bool H_FLAG = false;
  bool M_FLAG = false;
  bool V_FLAG = false;

  for(i = 1; i < argc; ++i){
    printf("value: %s|\n", argv[i]);
    if(!strcmp(argv[i], "-h")){
      if(!H_FLAG){
        printf("-h found\n");
        H_FLAG = true;
      }
      else{
        printf("duplicate -h option found\n");
        exit(2);
      }
    }
    else if(!strcmp (argv[i], "-m")){
      if(!M_FLAG){
        printf("-m found\n");
        printf("next argument is supposed to be a method name: %s\n", argv[++i]);
        M_FLAG = true;
      }
      else{
        printf("duplicated -m option found\n");
        exit(3);
      }
    }
    else if(!strcmp (argv[i], "-v")){
      if(!V_FLAG){
        printf("-v found\n");
        printf("expect next argument to be a volunm ID: %s\n", argv[++i]);
        V_FLAG = true;
      }
      else{
        printf("duplicated -v option found\n");
        exit(4);
      }
    }
    else{
      printf("Invalid option: %s\n", argv[i]);
      exit(1);
    }
  }

      
 
  char* buff = getenv("TES");
  printf("TES=%s\n", buff);
  exit(0);
}
