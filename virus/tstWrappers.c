#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


int main(int argc, char ** argv){

  if(argc < 2)
    return -1;

  int fd;
  if((fd=open(argv[1],O_RDWR))==-1){
    fprintf(stderr,"Open %s Error:%s\n",argv[1],strerror(errno));
    return -1;
  }

  struct stat fstats;      
  fstat(fd,&fstats);


  char content[8] = {0};
  if(read(fd,content,8)==0){
    fprintf(stdout,"Read error for %s \n",argv[1]);
    close(fd);
    return -1;
  }
	
  fprintf(stdout, "The file size is %d:\n",fstats.st_size);
  fprintf(stdout, "The first 8 bytes are: <<");
  int i;
  for(i = 0; i < 8; i++)
    fprintf(stdout, "%02x", content[i]);
  fprintf(stdout, ">>\n");
  

  if(close(fd)<0)
    return -1;

  return 0;
}
