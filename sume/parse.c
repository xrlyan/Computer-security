#include "header.h"

int parse(int argc, char **argv){

  // if no command(ls or cat) is passed, we do nothing but return.
  if(argc < 3){
    return -1;
  }

  // check the command for sume
  if(strcmp("ls",argv[1]) && strcmp("cat",argv[1])
     &&strcmp("/bin/ls",argv[1]) && strcmp("/bin/cat",argv[1])){
    printf("argv[1] is %s, but we need ls or /bin/ls or cat or /bin/cat here\n",argv[1]); 
    return -1;
  }

  // white list for ls
  // we pass cat since no requirement for its options.
  if(!strcmp("ls",argv[1]) || !strcmp("/bin/ls",argv[1])){
    // check for the ls command
    if(argc == 4){
      if(argv[2][0] !='-'){
	return -1;
      }else if(argv[2][1] !='m' && argv[2][1] !='s'){
	return -1;	
      }else if(strlen(argv[2])!=2){
	return  -1;
      }

    }else if(argc == 5){

      if(argv[2][0] !='-'){
	return -1;
      }else if(argv[2][1] !='m' && argv[2][1] !='s'){
	return -1;	
      }else if(strlen(argv[2])!=2){
	return -1;
      }

     if(argv[3][0] !='-'){
	return -1;
      }else if(argv[3][1] !='m' && argv[3][1] !='s'){
	return -1;	
      }else if(strlen(argv[3])!=2){
	return -1;
      }
    }
  }

  return 0;
}
