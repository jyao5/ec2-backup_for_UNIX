#include<stdlib.h>
#include<string.h>
#include<stdio.h>


typedef int bool; 
#define true 1
#define false 0

bool find_default_key(){
  if(system("ls ~/.ssh/id_rsa 2>/dev/null > /dev/null") == 0)
    return true;
  return false;
}

bool key_type_check(char* path){
  char command[200];
  strcpy(command, "grep");
  if(system( strcat( strcat( strcat( command, " RSA " ), path ), " 2>/dev/null >/dev/null")) == 0)
    return true;
  return false;
}


int import_keys(char* VAR){//if VAR==NULL search ~/.ssh/
                           //otherwise use file pointed by VAR
  char command[250];
  strcpy(command, "aws ec2 import-key-pair --key-name ec2-backup-key --public-key-material $(openssl ");

  if(VAR != NULL){
    if(key_type_check(VAR)){
        strcat(command, "rsa -in ");
        strcat(command, VAR);
    }
    else{
      printf("Unknown key file: %s\n", VAR);
      exit(1);//identity file not recognized
    } 
  }
  else{
    int type = find_default_key();
    if(find_default_key())
      strcat(command, "rsa -in ~/.ssh/id_rsa");
    else{
      printf("Couldn't find proper key file in ~/.ssh/\n");
      exit(1);
    }
  }

  strcat(command, " -outform DER -pubout 2>/dev/null | base64 | sed -e :a -e '$!N;s/\\n//;ta') 2>/dev/null >/dev/null");
  system(command);
  //printf("%s", command);
  return 0;
}

int main(){
  import_keys(NULL);
}
