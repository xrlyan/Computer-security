/**************************
Project 3 CS4471
Written by Jaclyn Barnes & Liang Yan
Computer Science Dep.
Dec 2, 2014
************************/

#include "header.h"

int main(int argc, char** argv){

  if(clearenv()){
    printf("could not clean enviroment succcessfully\n");
  }	
  uid_t mysuid,myeuid,myruid;
  /* Once running sume, set the euid = guest ruid
  Make sure only use host euid when necessary */
  getresuid(&myruid,&myeuid,&mysuid);
  seteuid(myruid);


  /* We assume the first argument is ~/hostuser/sume, 
  we get the host's home directory by removing /sume */
  int len = strlen(argv[0])-5;
  if(len < 2) {
    printf_usuage();
    return -1;
  }
  char* availablePath = malloc(len+1);
  if(availablePath == NULL){
    return -1;
  }

  memset(availablePath,0,len+1);
  strncpy(availablePath,argv[0],len);

  /*Get the host's username from available path
  Go backward until '/' */
  int index = len;
  while(1){
    if(availablePath[index] == '/')
      break;
    else
      index--;
    if(index < 0){
      printf_usuage();
     return -1;
    }
  }
  char* hostUsr = malloc(len-index);
  if(hostUsr == NULL){
    free(availablePath);
    return 0;
  }
  memset(hostUsr,0,len-index+1);

  strncpy(hostUsr,&availablePath[index+1],len-index);

  /*Check whitelist */
  if(parse(argc,argv)<0){
    free(availablePath);
    free(hostUsr);
    printf_usuage();
    exit(0);
  }

  // Check that the configuration file (.sumeCfg) is secure
  if(checkConfig(availablePath,hostUsr)<0) {
    free(availablePath);
    free(hostUsr);
    fprintf(stdout, "checkConfig failed \n");		
    exit(0);
  }

  free(hostUsr);

  // change the relative path in argv[argc-1]
  if(find_path(argc,argv,availablePath)<0){
    free(availablePath);
    printf_usuage();
    exit(0);
  }
  free(availablePath);

  getresuid(&myruid,&myeuid,&mysuid);
  seteuid(mysuid);

  /* NULL as the final arg here executes the command with an empty environment */
  if(execvpe(argv[1],&argv[1],NULL)<0){
    printf(" Command %s could not execute here.\n",argv[1]);
  }

  return 0;
}

