/**************************
Project 2 from cs4471
Written by liang & jaclyn
Computer Science Dep.
10/14/2015
 ************************/

#include <stdio.h>
#include <stdlib.h>

#include "utility.h"
#include "encryptFile.h"
#include "decryptFile.h"
#include "getKkey.h"
#include "keyVerify.h"

/*
  this function do a preverify for secute file
  check input if is validation
  check the plaintext file if could open
  si and li should be pair and no overlap
  do not oversize the file.
*/

int process_s(int argc,char *argv[]){

  
  int fd;
  if((fd=open(argv[2],O_RDONLY))==-1){
    fprintf(stderr,"Open %s Error:%s\n",argv[2],strerror(errno));
    return -1;
  }

  struct stat fstats;            /* Stats on ciphertext file */
  fstat(fd,&fstats);

  int i=3;
  int totalSize = 0;
   
  // number of classified content
  int N = (argc-3)>>1;
  int offset[N];
  int startPtr[N];


  //put all si and li to its array
  int j = 0;
  for(i=3;i<argc;i=i+2){

    startPtr[j] = atoi(argv[i]); 

    if(!startPtr[j]){
      if(strcmp(argv[i],"0")!=0){
	fprintf(stdout,"input a wrong classified content position %s\n",argv[i]);
	close(fd);
	return -1;
      }
    }
    
    if(j>0){
      if(startPtr[j-1]+offset[j-1] > startPtr[j]){
	fprintf(stdout,"last input %d and %d overflow new input %d\n",startPtr[j-1],offset[j-1],startPtr[j]);
	close(fd);
	return -1;
      }
    }

    offset[j] = atoi(argv[i+1]);
    if(!offset[j]){
      fprintf(stdout,"input a wrong classified content size %s\n",argv[i+1]);
      return -1;
    }

    totalSize += offset[j];

    if(startPtr[j]+offset[j]>fstats.st_size){
      fprintf(stdout,"start %d, offset %d classified content overflows the text file\n",startPtr[j],offset[j]);
      close(fd);
      return -1;
    }

    if(totalSize>fstats.st_size){
      fprintf(stdout," classified content size %d overflows the text file\n",totalSize);
      close(fd);
      return -1;
    }
    j++;
  }

  close(fd);

  encryptFile(argv[2],startPtr,offset,N,totalSize);

  return 0;
}


int process_u(char* fileName){
   
    
  char* encFile=malloc(strlen(fileName)+5); // filename.enc 
  if(encFile == NULL)
    return -1;
  memset(encFile,0,strlen(fileName)+5);
  get_enc_file(fileName,encFile);

  int fd;
  /* Read ciphertext */
  if((fd=open(encFile,O_RDONLY))==-1){
    fprintf(stderr,"Open %s Error:%s\n",encFile,strerror(errno));
    free(encFile);
    return -1;
  }

  struct stat     fstats;            // Stats on ciphertext file 
  fstat(fd,&fstats);
  
  unsigned char kid[BYTES_LENGTH+1]={0};       /* KID  */
  if(read(fd,kid,BYTES_LENGTH)==0){
    fprintf(stderr,"Read %s wrong\n",encFile);
    free(encFile);
    close(fd);
    return -1;
  }
  
  unsigned char magic[BYTES_LENGTH+1]={0};       /* MAGIC  */
  if(read(fd,magic,BYTES_LENGTH)==0){
    fprintf(stderr,"Read %s wrong\n",encFile);
    free(encFile);
    close(fd);
    return -1;
  }

  // use magic number to check if it is a valid "*.enc" file
  char magicTemp[5] = "FISH";  
  if(strncmp(magic,magicTemp,BYTES_LENGTH)!=0){
    printf("File %s is not a secured file \n",encFile);
    close(fd);
    free(encFile);
   return -1;
  }
  

  char start[BYTES_LENGTH+1]={0};
  if(read(fd,start,BYTES_LENGTH)==0){	
    fprintf(stderr,"Read %s wrong\n",encFile);
    free(encFile);
    close(fd);
    return -1;
  }
  
  int startPtr = atoi(start);
  int remain_text_size = fstats.st_size - startPtr;
  
  char* remain_text = malloc(remain_text_size+1);
  memset(remain_text,0,remain_text_size+1);

  if(lseek(fd,startPtr,SEEK_SET) < 0){
    printf("File %s is wrong during lseeking \n",encFile);
    close(fd);
    free(encFile);   
    return -1;
  }

  if(read(fd,remain_text,remain_text_size)==0){
    fprintf(stderr,"Read %s wrong\n",encFile);
    free(encFile);
    free(remain_text);
    close(fd);
    return -1;
  }

  int i;
  /* Assumes we're decrypting ASCII plaintext */
  fprintf(stdout,"Declassified content in ASCII: \n<");
  for (i=0;i<strlen(remain_text);i++) fprintf(stdout,"%c",remain_text[i]);
  fprintf(stdout,">\n");

  close(fd);

  free(encFile);
  free(remain_text);

  return 0;
}

/*
  fge -c
*/
int process_c(char * fileName){

  return decryptFile(fileName);
}

/*
  fge -key
*/
int process_k(char *fileName){  
  return keyVerify(fileName);
}


int main(int argc, char* argv[]){

  if(argc<3){
    printf_usuage();
    exit(0);
  }

  //Set core dump to zero		
  struct rlimit plimits;
  plimits.rlim_cur = 0;
  plimits.rlim_max = 0;
  if(setrlimit(RLIMIT_CORE, &plimits)<0) {
    fprintf(stderr, "Error setting the core dump to zero. \n");
    exit(-1);
  }

  int res=0;

  if(!strncmp(argv[1], "-s",2)){
    // check the validation of input
    // at least one pair s[i] l[i]
    // and s[i] l[i] be pair
    if(argc<5||argc %2 == 0){ 
      printf_usuage();
      exit(0);
    }
    res = process_s(argc,argv);
  }
  else if(!strncmp(argv[1], "-c",2)){
    if(argc!=3){
      printf_usuage();
      exit(0);
    }
    res = process_c(argv[2]);
  }
  else if(!strncmp(argv[1], "-u",2)){
    if(argc!=3){
      printf_usuage();
      exit(0);
    }    
    res = process_u(argv[2]);
  }
  else if(!strncmp(argv[1], "-key",4)){
    if(argc!=3){
      printf_usuage();
      exit(0);
    }
    res = process_k(argv[2]);
 }
  else
    printf_usuage();
  
  return res;
}
