/**************************
Project 4 CS4471
Written by Jaclyn Barnes
MTU Computer Science Dep.
Dec 11, 2014
************************/
#include <stdio.h>

int main(int argc, char** argv){
  if (argv[1]) {
    printf("You have executed the host program with argument %s \n", argv[1]);
  } else {
    printf("Please run host with a filename as an argument \n");
  }
  return 0;
}
