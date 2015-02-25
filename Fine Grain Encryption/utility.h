/**************************
Project 2 from cs4471
Written by jaclyn & liang
Computer Science Dep.
10/14/2015
 ************************/

#ifndef UTILITY_H
#define UTILITY_H


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> 
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>



#define BUFFER_SIZE  8
#define MAXKEYLEN   16
#define ACTKEYLEN   16
#define BLOCKSIZE   8  /* Blowfish block size */
#define BYTES_LENGTH 4


#define K_FILE_LENGTH 16 //128bit 16bytes
#define K_KEY_LENGTH 16 //128bit 16bytes
#define MAGIC 0xabcddcba
#define PASSPHRASELEN sizeof(char*) * 100


/*
  output uusuage when user input wrong arguments
*/

void printf_usuage();


/*
  output a char string to a hex
*/
void fprint_string_as_hex(FILE * f, unsigned char *s, int len);



/*
  Generate a random number as Kkey 
*/
void get_random_key(unsigned char *key);

/*
  get the encrypto file name
*/
void get_enc_file(char *oldName,char *newName);

/* 
	Check two KID against each other
*/
int check_KID(char *fileKID, char *decryptKID);

/*
	Copy keys regardless of null terminating character
*/
char* mystrncpy(char * destBuf, char* srcBuf, int len);

char*
mystrncat(char *dest,size_t m, const char *src, size_t n);


#endif
