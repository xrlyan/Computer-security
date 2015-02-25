#include "header.h"


int find_path(int argc,char **argv,char *availablePath){

  // check for the last parameter for ls and cat
  // find a relative path
  char cwd[1024]={0};
  if (getcwd(cwd, sizeof(cwd)) == NULL){
    perror("getcwd() error");
    return -1;
  }

  
  char* filePath = argv[argc-1];
  char *newPath=malloc(strlen(cwd)+1+strlen(filePath)+1);
  if(newPath == NULL){
    return -1;
  }
  memset(newPath,0,strlen(cwd)+1+strlen(filePath)+1);

  // if not starting by '/' we think it is a relative path
  // we need to cat the current work directory.
  // else do not change anything.

  if (filePath[0] !='/') {   

    strncpy(newPath, cwd,strlen(cwd));
    strncat(newPath,"/",1);
    strncat(newPath,filePath,strlen(filePath));
    strncpy(argv[argc-1],newPath,strlen(newPath));

    // this is because origin file path could be shorter than new one.
    // so add '\0' by mannually
    argv[argc-1][strlen(newPath)] = '\0';
  }

  free(newPath);

  int len = strlen(availablePath);
  int i;
  for(i=0;i<len;i++){
    if(availablePath[i]!=argv[argc-1][i]){
      printf("%s is out of range \n",argv[argc-1]);
      return -1;
    }	
  }

  // track the ".." should not  above host home directory. 
  int level = 0;
  for(i= len;i<strlen(argv[argc-1]);i++){
    if(argv[argc-1][i]=='/')
      level++;
    else{ 
      if(filePath[i-1] == '.' && filePath[i] == '.'){
	level -=2;
	if(level < 0){
	  printf("%s is out of range \n",argv[argc-1]);
	  return -1;
	}
      }
    }
  }

  return 0;

}
