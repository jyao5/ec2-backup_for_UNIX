#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"create_instance.h"
#include"backup.h"

typedef int bool;
#define true 1
#define false 0

int main(int argc, char *argv[]){
  int i;
  bool H_FLAG = false;
  bool M_FLAG = false;
  bool V_FLAG = false;
  bool D_FLAG = false;
  char m_option[20];
  char vol_id[30];
  char dir[250];
  char instance_id[30];
  char ip[30];


  for(i = 1; i < argc; ++i){
    //printf("value: %s|\n", argv[i]);
    if(!strcmp(argv[i], "-h")){
      if(!H_FLAG){
        //printf("-h found\n");
        H_FLAG = true;
      }
      else{
        printf("duplicate -h option found\n");
        exit(2);
      }
    }
    else if(!strcmp (argv[i], "-m")){
      if(!M_FLAG){
        //printf("-m found\n");
        //printf("next argument is supposed to be a method name: %s\n", argv[++i]);
        strcpy(m_option, argv[++i]);
        if((strstr(m_option, "dd") == NULL) && (strstr(m_option, "rsync") == NULL)){
          printf("Invalid -m option\n");
          exit(1);
        }
        M_FLAG = true;
      }
      else{
        printf("duplicated -m option found\n");
        exit(3);
      }
    }
    else if(!strcmp (argv[i], "-v")){
      if(!V_FLAG){
        //printf("-v found\n");
        //printf("expect next argument to be a volunm ID: %s\n", argv[++i]);
        strcpy(vol_id, argv[++i]);
        if(strstr(vol_id, "vol-") == NULL){
          printf("Invalid vol id\n");
          exit(1);
        }
        V_FLAG = true;
      }
      else{
        printf("duplicated -v option found\n");
        exit(4);
      }
    }
    else{
      if(!D_FLAG){
        strcpy(dir, argv[i]);
        D_FLAG = true;
      }
      else{
        printf("Invalid option: %s\n", argv[i]);
        exit(1);
      }
    }
  } //option process complete


  //parsing directory name
  if(dir[0]=='/')
    ;//printf("dir: %s\n", dir);
  else if(dir[0] == '~'){
    char buffer[250];  
    strcpy(buffer, getenv("HOME"));
    strcat(buffer,dir);
    strcpy(dir, buffer);
    printf("dir: %s\n", dir);
  }
  else{
    char buffer[250];  
    realpath(dir, buffer);
    strcpy(dir, buffer);
    //printf("dir: %s\n", dir);
  }

  if(D_FLAG == false){
    printf("Need dir or file name\n");
    exit(1);
  }

  FILE *output;
  char cmd[200];
  strcpy(cmd, "du -hs --block-size=1G ");
  strcat(cmd, dir);
  strcat(cmd, " 2> /dev/null");
  output = popen(cmd, "r");
  if(output == NULL){
    printf("error executing: %s\n", cmd);
    exit(1);
  }

  fgets(cmd, sizeof(cmd), output);
  pclose(output);
  char size[10];
  char *p = strtok(cmd, "\t ");
  strcpy (size, p);
  //printf("size: %s\n", size);

  if(H_FLAG == true){
    printf("Verbal option is enabled\n");
  } 

  if(H_FLAG == true){
    printf("Create volume and instance\n");
  }

  if(V_FLAG == true){
    //printf("v option true: %s\n", vol_id);
    create_instance(size, vol_id, true, instance_id, ip);
  }
  else{
    create_instance(size, vol_id, false, instance_id, ip);
  }


  if(H_FLAG == true){
    printf("Instance and volume created/founded\n");
    printf("vol_id: %s\n", vol_id);
    printf("instance_id: %s\n", instance_id);
    printf("ip: %s\n", ip);
    printf("\nAttaching volume\n");
  }


  sleep(50);

  attach_volume(vol_id, instance_id);

  if(H_FLAG == true){
    printf("Volume attached\n Connecting to remote host\n");
  }

  if(H_FLAG == true){
    printf("Performing backup\n");
  }

  if(M_FLAG == true){
    //printf("m option true: %s\n", m_option);
    if(strcmp(m_option, "dd") == 0)
      backup(ip, true, dir);
    else
      backup(ip, false, dir);
  }
  else{
      backup(ip, true, dir);
  }

  if(H_FLAG == true){
    printf("Detach volume\n");
  }

  detach_volume(vol_id);

  if(H_FLAG == true){
    printf("Terminating instances\n");
  }

  strcpy(cmd, "aws ec2 terminate-instances --instance-ids ");
  strcat(cmd, instance_id);
  strcat(cmd, " > /dev/null 2> /dev/null");

  if(H_FLAG == true){
    printf("Done!\n");
  } 
  printf("volume_id: %s\n", vol_id);

  exit(0);
}
