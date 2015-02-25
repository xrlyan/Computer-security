/**************************
Project 2 from cs4471
Written by liang
Computer Science Dep.
10/24/2015
 ************************/

#ifndef ENCRYPTFILE_H
#define ENCRYPTFILE_H

#include <openssl/evp.h>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> 
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

unsigned char* allocate_ciphertext(int mlen);

int encryptFile(char*fileName,int startPtr[],int offset[],int N,int totalSize);

int encryptFile(char*fileName,int startPtr[],int offset[],int N,int totalSize);

#endif
