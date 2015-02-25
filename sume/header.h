/**************************
Project 2 from cs4471
Written by jaclyn & liang
Computer Science Dep.
11/24/2015
 ************************/

#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


/****************
utility.c
 ***************/
void printf_usuage();


/****************
checkConfig.c
 ***************/
#define _GNU_SOURCE
int checkConfig(char* fp, char* hostUsr);



/****************
parse.c
 ***************/

int parse(int argc, char** argv);

/****************
findPath.c
 ***************/

int find_path(int argc,char **argv,char *availablePath);


#endif
