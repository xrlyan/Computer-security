/**************************
Project 2 from cs4471
Written by jaclyn
Computer Science Dep.
10/14/2015
 ************************/


#ifndef GETKKEY_H
#define GETKKEY_H

#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <inttypes.h>
#include <termios.h>
#include <unistd.h>

#define SHA_DIGEST_LENGTH  20
#define FILEKEYLEN 16
#define BLOCKSIZE   8

void remove_null_and_newline(char* phrase) ;

char*
getInput(char* phrase1, int initialReq);
int
getKkey(char*kkey, int initialReq);
#endif

