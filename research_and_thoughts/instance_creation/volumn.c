#include<stdlib.h>
#include<stdio.h>
#include<string.h>

void volumn(int size, char* vol_id, char* s_id){
  char aws_region[20];
  getAWSzone(aws_region);//read ~/.aws/config for current region
  if(vol_id != NULL){
    char vol_zone[20];
    getVOLzone(vol-id, vol_zone);//given vol-id, get volumn zone
    if(strcmp(aws_region, vol_zone) != 0){//see if volumn is in the current region
      printf("Volumn provided is not in availability zone\n");
      exit(1);
    }
    if(small(size, vol-id)){//return true if size greater than volumn size
      printf("Volumn size is not sufficient\n");
      exit(1);
    }

    FILE *out;
    char msg[200];
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
      printf("Somehow can't determin subnet id of %s\n", vol_id);
      exit(1);
    }
  }//end of vol_id is provided
  else{//when no vol_id is provided, then vol_id is used to store created vol-id for display
    out=popen("aws ec2 describe-subnets --output text", "r");
    if(fgets(msg, sizeof(msg) - 1, out) != NULL)
      get_sub_id(msg, s_id);
    else{
      printf("No subnet id available\n");
      exit(0);
    }
  }
}
      

    

    






  }

int main(){



