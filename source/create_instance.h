#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<unistd.h>


typedef int bool; 
#define true 1
#define false 0

//functions for import_key
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





//functions for create_volume
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


void getVOLzone(char *id, char *zone, char* size){
  char command[250];
  strcpy(command, "aws ec2 describe-volumes --volume-ids ");
  strcat(command, id);
  strcat(command, " --output text --query 'Volumes[0].[AvailabilityZone,Size]' 2> /dev/null");

  FILE *out;
  out = popen(command, "r");
  if(out == NULL){
    printf("Trouble executing:\n %s\n", command);
    exit(1);
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
  //printf("%s %s\n", aws, vol);
  if(strstr(vol, aws) != NULL)
    return 0;
  else
    return 1;
}

int get_subnet_id(char *msg, char *s_id){
  char *p = strtok(msg, "\t ");
  while((p = strtok(NULL, "\t ")) != NULL){
    if(strstr(p, "subnet-") != NULL){
      strcpy(s_id, p);
      return 0;
    }
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

void volume(char *size, char* vol_id, char* s_id, bool vol_provided){
  char aws_region[20];
  getAWSzone(aws_region);//read ~/.aws/config for current region
  FILE *out;
  char msg[200];
  if(vol_provided){//if vol_id is given, check if zone matches, size sufficient, determing subnet-id
    char vol_zone[20];
    char vol_size[20];
    getVOLzone(vol_id, vol_zone, vol_size);//given vol-id, get volumn zone

    if(cmp_region(aws_region, vol_zone) != 0){//see if volumn is in the current region
      printf("Volumn provided is not in availability zone\n");
      exit(1);
    }
    if(atoi(size) > atoi(vol_size)){//return true if size greater than volumn size
      printf("Volumn size is not sufficient\n");
      exit(1);
    }

    bool subnet_found = false;
    out=popen("aws ec2 describe-subnets --output text", "r");
    while(fgets(msg, sizeof(msg) - 1, out) != NULL)
      if(strstr(msg, vol_zone) != NULL){
        get_subnet_id(msg, s_id);//read msg, store subnet_id in s_id
        subnet_found = true;
        break;
      }
    pclose(out);
    if(subnet_found == false){
      printf("Somehow can't determine subnet id of %s\n", vol_id);
      exit(1);
    }
  }//end of vol_id is provided
  else{//when no vol_id is provided, then vol_id is used to store created vol-id for display
    //search for a valid subnet_id
    char avail_zone[50];
    out=popen("aws ec2 describe-subnets --output text", "r");
    if(fgets(msg, sizeof(msg) - 1, out) != NULL){
      char msg_buf[200];
      strcpy(msg_buf, msg);
      get_subnet_id(msg, s_id);
      get_avail_zone(msg_buf, avail_zone);
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

//assembly and create instance

int parse(char *ssh_option, char* id_path){
  if(ssh_option == NULL)
    return 0;
  char* p = strtok(ssh_option, " \t");
  p = strtok(NULL, "\t");
  if(p == NULL){
    printf("Invalid ssh option.\n");
    exit(1);
  }
  strcpy(id_path, p);
  return 0;
}

int create_instance(char *size, char* vol_id, bool volume_provided, char *instance_id, char *ip){//read AWS flag for additional options
                                //read SSH flag for key file location
  char aws_option[300];
  char ssh_option[100];
  char id_path[100];

  char sub_id[30];
  char command[300];

  if(getenv("EC2_BACKUP_FLAGS_SSH") != NULL){
    strcpy(ssh_option, getenv("EC2_BACKUP_FLAGS_SSH"));
    char ssh_option_buff[100];
    strcpy(ssh_option_buff, ssh_option);
    parse(ssh_option_buff, id_path);//parse option for pathname solely
    //printf("ssh env set: %s\n", ssh_option);
    import_keys(id_path);
  }
  else
    import_keys(NULL);


  volume(size, vol_id, sub_id, volume_provided);//errored

  strcpy(command, "aws ec2 run-instances --image-id ami-fce3c696 --instance-type t2.micro --count 1 --output text --key-name ec2-backup-key --subnet-id ");
  //this is an ubuntu instance
  strcat(command, sub_id);
  strcat(command, " --query 'Instances[0].InstanceId' ");

  if(getenv("EC2_BACKUP_FLAGS_AWS") != NULL){
    strcpy(aws_option, getenv("EC2_BACKUP_FLAGS_AWS"));
    strcat(command, aws_option);
  }


  FILE *output;
  output = popen(command, "r");
  if(output == NULL){
    printf("Error executing run-instance command\n");
    exit(1);
  }


  fgets(instance_id, 29, output);
  if(pclose(output) != 0){
    printf("Run instance failed\n");
    exit(1);
  }

  char *p = strstr(instance_id, "\n");
  *p = '\0';
  
  strcpy(command, "aws ec2 describe-instances --instance-id ");
  strcat(command, instance_id);
  strcat(command, " --output text --query 'Reservations[0].Instances[0].PublicIpAddress'");

  sleep(5);

  output = popen(command, "r");
  if(output == NULL){
    printf("Error executing get ip command\n");
    exit(1);
  }

  fgets(ip, 29, output);
  if(pclose(output) != 0){
    printf("get ip failed\n");
    exit(1);
  }

  p = strstr(ip, "\n");
  *p = '\0';



  
  //printf("command: %s\n", command);
}

//finally, attatch volume
int attach_volume(char *vol_id, char *instance_id){
 char command[200]; 
 strcpy(command, "aws ec2 attach-volume --device /dev/xvdc --volume-id ");
 strcat(command, vol_id);
 strcat(command, " --instance-id ");
 strcat(command, instance_id);
 strcat(command, " > /dev/null 2> /dev/null");
 if(system(command) != 0){
   printf("Error executing: %s\n", command);
   exit(1);
 }
}

int detach_volume(char *vol_id){
 char command[200]; 
 strcpy(command, "aws ec2 detach-volume --volume-id ");
 strcat(command, vol_id);
 strcat(command, " > /dev/null 2> /dev/null");
 if(system(command) != 0){
   printf("Error executing: %s\n", command);
   exit(1);
 }
 //printf("command: %s\n", command);
}


//(echo n; echo p; echo 1; echo; echo; echo w) | ssh ubuntu@54.210.38.237 "sudo fdisk /dev/xvdc"
/*
int main(){
  char vol_id[30];
  strcpy(vol_id, "vol-ed214b45");
  char instance_id[30];
  char ip[30];
  create_instance("1", vol_id, true, instance_id, ip);
  printf("vol_id: %s\n", vol_id);
  printf("instance_id: %s\n", instance_id);
  printf("ip: %s\n", ip);
  attach_volume(vol_id, instance_id);
  detach_volume(vol_id);

}
*/

