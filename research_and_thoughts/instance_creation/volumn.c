#include<stdlib.h>
#include<stdio.h>
#include<string.h>


void get_region(char *line, char *region){
  char *p;
  p = strtok(line, " \t=");
  p = strtok(NULL, " \t=");
  strcpy(region, p);
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


int cmp_region(char *aws, char* vol){
  printf("%s %s\n", aws, vol);
  if(strstr(vol, aws) != NULL)
    return 0;
  else
    return 1;
}

int get_subnet_id(char *msg, char *s_id){
  char *p = strtok(msg, "\t ");
  while((p = strtok(NULL, "\t ")) != NULL){
    if(strstr(p, "subnet-") != NULL)
      strcpy(s_id, p);
      return 0;
  }
  printf("No subnet id found in msg\n");
  return 1;
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

void volumn(char *size, char* vol_id, char* s_id, bool vol_provided){
  char aws_region[20];
  getAWSzone(aws_region);//read ~/.aws/config for current region
  FILE *out;
  char msg[200];
  if(vol_provided){//if vol_id is given, check if zone matches, size sufficient, determing subnet-id
    char vol_zone[20];
    char vol_size[20];
    getVOLzone(vol-id, vol_zone, vol_size);//given vol-id, get volumn zone
    if(cmp_region(aws_region, vol_zone) != 0){//see if volumn is in the current region
      printf("Volumn provided is not in availability zone\n");
      exit(1);
    }
    if(atoi(size) > atoi(vol-size)){//return true if size greater than volumn size
      printf("Volumn size is not sufficient\n");
      exit(1);
    }

    bool subnet_found = false;
    out=popen("aws ec2 describe-subnets --output text", "r");
    while(fgets(msg, sizeof(msg) - 1, out) != NULL)
      if(strstr(msg, vol-zone) != NULL){
        get_subnet_id(msg, s_id);//read msg, store subnet_id in s_id
        subnet_found = true;
        break;
      }
    pclose(out);
    if(subnet == false){
      printf("Somehow can't determine subnet id of %s\n", vol_id);
      exit(1);
    }
  }//end of vol_id is provided
  else{//when no vol_id is provided, then vol_id is used to store created vol-id for display
    //search for a valid subnet_id
    char avail_zone[20];
    out=popen("aws ec2 describe-subnets --output text", "r");
    if(fgets(msg, sizeof(msg) - 1, out) != NULL){
      get_sub_id(msg, s_id);
      get_avail_zone(msg, avail_zone);
    }
    else{
      printf("No subnet id available\n");
      exit(1);
    }
    pclose(out);
    
    //create a volume
    create_volume(size, vol_id, avail_zone);
  }
}
      

int main(){
}



