#include<stdlib.h>
#include<stdio.h>
#include<string.h>

typedef int bool;
#define true 1
#define false 0

char* create_command(char* env_var, char* key_name, char* subnet){//return command
  char buff[256];
  strcpy(buff, "aws ec2 run-instance --image-id ami-08111162 --output text --query 'Instances[0].InstanceId' --key-name ");
  strcat(buff, key_name);
  strcat(buff, " --subnet-id ");
  strcat(buff, subnet);
  strcat(buff, " ");
  strcat(buff, env_var);

  printf("%s\n", buff);
}

  /*FILE *fp;
  char path[1035];

  fp = popen("aws ec2 describe-instances", "r");
  if (fp == NULL) {
    printf("C:Failed to run command\n" );
    exit(1);
  }

  while (fgets(path, sizeof(path)-1, fp) != NULL) {
    printf("%s", path);
  }

  pclose(fp);
  */

char* import_key(bool key, char* key_path){
  char buffer[200];
  strcpy(buffer, "aws ec2 import-key-pair --key-name ec2_backup_key --public-key-material $(openssl rsa -in ");
  if(key)//if a key is specified
    strcat(buffer, key_path);
  else{//otherwise find a valid key to import
    reserved_file(
    FILE *fp;
    fp=popen("ls ~/.ssh/ | grep '[^id_rsa$]'");
    if (fp == NULL) {
      printf("C:Failed to run command\n" );
      exit(1);
    }
    char path[100];
    fgets(path, sizeof(path)-1, fp);
    if(!strcmp(path, "id_rsa")){
      strcat(buffer, key_path)
      break;
    }

    


  strcat(buffer, " -outform DER -pubout | base64 | sed -e :a -e '$!N;s/\\n//;ta')");
  printf("%s\n", buffer);

}
  



//create_instance("--instance-type t2.micro, --security-group-id sg-123456", "jesses_key", "sn-123456");

int main(){

find_key_name(true, "~/.ssh/id_rsa");


  return 0;
}

