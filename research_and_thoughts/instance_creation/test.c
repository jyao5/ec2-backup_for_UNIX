#include<stdlib.h>
#include<stdio.h>
#include<string.h>

typedef int bool;
#define true 1
#define false 0

void getVOLzone(char *id, char *zone, char* size){
  char command[250];
  strcpy(command, "aws ec2 describe-volumes --volume-ids ");
  strcat(command, id);
  strcat(command, " --output text --query 'Volumes[0].[AvailabilityZone,Size]' 2> /dev/null");

  FILE *out;
  out = popen(command, "r");
  if(out == NULL){
    printf("Trouble executing:\n %s\n", command);
    exit(0);
  }
  char content[100];
  fgets(content, sizeof(content) - 1, out);

  char *p = strtok(content, " \t");
  strcpy(zone, p);

  p = strtok(NULL, " \t");
  strcpy(size, p);
  p = strstr(size, "\n");//find and delete the newline
  *p = '\0';
}


void get_region(char *line, char *region){
  char *p;
  p = strtok(line, " \t=");
  p = strtok(NULL, " \t=");
  strcpy(region, p);
  p = strstr(region, "\n");
  *p = '\0';
}
  

void getAWSzone(char* region){
  FILE *fp;
  //
  char path[200];
  strcpy(path, getenv("HOME"));
  strcat(path, "/.aws/config");
  fp = fopen(path, "r");
  if(fp == NULL){
    printf("Can't open following path for zone settings:%s\n", path);
    exit(1);
  }
  char line[100];
  bool found_flag = false;
  while (fgets(line, sizeof(line) - 1, fp) != NULL){
    if(strstr(line, "region") != NULL){
      get_region(line, region);
      found_flag = true;
      return;
    }
  }

  if(!found_flag){
    printf("Coundn't find region info in config\n");
    exit(1);
  }
  
  fclose(fp);

  return;
}

int cmp_region(char *aws, char* vol){
  printf("%s %s\n", aws, vol);
  if(strstr(vol, aws) != NULL)
    printf("YES\n");
  else
    printf("NO\n");
}

void get_subnet_id(char *msg, char *s_id){
  char *p = strtok(msg, "\t ");
  while((p = strtok(NULL, "\t ")) != NULL){
    if(strstr(p, "subnet-") != NULL)
      strcpy(s_id, p);
  }
}

void get_avail_zone(char *msg, char *a_zone){
  char *p = strtok(msg, "\t ");
  p = strtok(NULL, "\t ");
  strcpy(a_zone, p);
}

int create_volume(char* size, char *vol_id, char *avail_zone){
  char command[200];
  strcpy(command, "aws ec2 create-volume --size ");
  strcat(command, size);
  strcat(command, " --availability-zone ");
  strcat(command, avail_zone);
  strcat(command, " --output text --query 'VolumeId'");
  //printf("%s\n", command);
  FILE *output;
  output = popen(command, "r");
  if(output == NULL){
    printf("error executing:\n %s\n", command);
    exit(1);
  }
  char buff[20];
  fgets(buff, sizeof(buff) - 1, output);
  pclose(output);
  char *p = strstr(buff, "\n");
  *p = '\0';
  strcpy(vol_id, buff);
  return 0;
}


int main(){

  char avail_zone[100];
  char msg[200];
  strcpy(msg, "SUBNETS  us-east-1d  4091  172.31.0.0/20 True  True  available subnet-f547fb83 vpc-e4c38b80\n");
  get_avail_zone(msg, avail_zone);

  char vol_id[20];
  create_volume("2", vol_id, "us-east-1a");
  //printf("%s\n", vol_id);

/*
  char sub_id[100];
  char msg[200];
  strcpy(msg, "SUBNETS  us-east-1d  4091  172.31.0.0/20 True  True  available subnet-f547fb83 vpc-e4c38b80\n");
  get_subnet_id(msg, sub_id);
  printf("%s\n", sub_id);*/
 // printf("%d\n", atof("1.4") > atof("2"));
/*
  char volzone[20];
  char size[20];

  getVOLzone("vol-aff0970d", volzone, size);
  printf("volzone: %s\n", volzone);
  printf("size: %s\n", size);


  char awszone[20];
  getAWSzone(awszone);
  printf("%s\n", awszone);

  cmp_region(awszone, volzone);
 */ 

  return 0;
}

