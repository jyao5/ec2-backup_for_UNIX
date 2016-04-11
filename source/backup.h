#include<stdlib.h>
#include<stdio.h>
#include<string.h>

typedef int bool;
#define true 1
#define false 0

void ssh_exe(char *ip, char *command, char *exec){
  strcpy(command, "ssh ubuntu@");
  strcat(command, ip);
  strcat(command, " \"");
  strcat(command, exec);
  strcat(command, "\" 2> /dev/null > /dev/null");
  if(system(command) != 0)
    printf("error in executing: %s\n", command);
  //printf("exec: %s\n", command);
}


void backup(char *ip, bool dd, char* dir){
  char command[200];
  if(dd){
    ssh_exe(ip, command, "ls > /dev/null");
    strcpy(command, "tar cpf - ");
    strcat(command, dir);
    strcat(command, " | ssh ubuntu@");
    strcat(command, ip);
    strcat(command, " \"sudo dd of=/dev/xvdc\" > /dev/null 2> /dev/null");

    //printf("command: %s\n", command);
    if(system(command) != 0)
      printf("error in executing: %s\n", command);
  }
  else{ //see if there are partitions
    strcpy(command, "ssh ubuntu@");
    strcat(command, ip);
    strcat(command, " \"ls /dev | grep xvdc1\" > /dev/null");
    if(system(command) != 0){//creat a partition and format
      strcpy(command, "(echo n; echo p; echo 1;  echo;  echo; echo w;) | ssh ubuntu@");
      strcat(command, ip);
      strcat(command, " \"sudo fdisk /dev/xvdc\" > /dev/null 2> /dev/null");
      if(system(command) != 0)
        printf("error in executing: %s\n", command);
      //(echo n; echo p; echo 1; echo; echo; echo w) | ssh ubuntu@54.210.38.237 "sudo fdisk /dev/xvdc"
    ssh_exe(ip, command, "sudo mkfs /dev/xvdc1");
    }

    ssh_exe(ip, command, "mkdir ~/back_media");
    ssh_exe(ip, command, "sudo mount /dev/xvdc1 ~/back_media");
    ssh_exe(ip, command, "sudo chmod 777 ~/back_media");

    strcpy(command, "rsync -avz ");
    strcat(command, dir);
    strcat(command, " ubuntu@");
    strcat(command, ip);
    strcat(command, ":~/back_media/ 2> /dev/null > /dev/null");
    if(system(command) != 0)
      printf("error in executing: %s\n", command);
    //printf("command: %s\n", command);



    ssh_exe(ip, command, "sudo umount ~/back_media > /dev/null");
    ssh_exe(ip, command, "sudo rm -r ~/back_media > /dev/null");
  }
}
/*
int main(){
  char ip[30];
  char command[200];

  strcpy(ip, "54.175.39.222");
  backup(ip, true, "./hihi");

}
 */ 
