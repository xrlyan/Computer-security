/**************************
Project 2 from cs4471
Written by liang
Computer Science Dep.
10/26/2015
 ************************/

#ifndef DECRYPTFILE_H
#define DECRYPTFILE_H

#include <openssl/evp.h>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> 
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int decrypte(EVP_CIPHER_CTX *dctx,unsigned char*ct, int ctlen,unsigned char* pt);
int decryptFile(char* fileName);


#endif
