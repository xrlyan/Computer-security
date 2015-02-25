/**************************
Project 2 from cs4471
Written by jaclyn & liang
Computer Science Dep.
10/14/2015
 ************************/

#include "utility.h"

/*
    output uusuage when user input wrong arguments
*/
void printf_usuage(){
  fprintf(stdout,"--------------------------------------------------------\n");
  fprintf(stdout,"Usage: fge, please use the arguments below \n");
  fprintf(stdout,"fge -s file s1 l1 s2 l2 \n");
  fprintf(stdout,"fge -c file \n");
  fprintf(stdout,"fge -u file \n");
  fprintf(stdout,"fge -key file \n");
  fprintf(stdout,"--------------------------------------------------------\n");
}

/*
  output a char string to a hex
*/
void fprint_string_as_hex(FILE * f, unsigned char *s, int len)
{
  int             i;
  for (i = 0; i < len; i++)
    fprintf(f, "%02x", s[i]);
  
}

/*
Generate a random number as Kkey 
*/
void get_random_key(unsigned char *key) 
{ 
  FILE *rng; 
  int num = 0; 
  rng = fopen("/dev/urandom", "r"); 
  while (num < K_FILE_LENGTH) { 
    num += fread(&key[num], 
		 1, 
		 K_FILE_LENGTH - num, 
		 rng); 
    if(key[num-1]=='\0')
      num -= 1;
  } 

  fclose(rng); 
} 
 
void get_enc_file(char *oldName,char *newName){
  strcpy(newName,oldName);
  strncat(newName,".enc",4);
 }

int check_KID(char *fileKID, char *decryptKID) {
  return strncmp(fileKID, decryptKID, K_KEY_LENGTH);
}

/*
Generate a random number as Kkey 
*/
void get_random_kid(unsigned char *key) 
{ 
  FILE *rng; 
  int num = 0; 
  rng = fopen("/dev/urandom", "r"); 
  while (num < BYTES_LENGTH) { 
    num += fread(&key[num], 
		 1, 
		 BYTES_LENGTH - num, 
		 rng); 
    if(key[num-1]=='\0')
      num -= 1;

  } 
  fclose(rng); 
} 

char* mystrncpy(char * destBuf, char* srcBuf, int len) {
  
  size_t i;
  char *ptr = destBuf;
  for(i = 0; i < len; i++){
    *ptr++ = *srcBuf++;
  }
  
  return destBuf;
}

char*
mystrncat(char *dest,size_t m, const char *src, size_t n)
{
  size_t dest_len = m;
  size_t i;
  
  for (i = 0 ; i < n  ; i++)
    dest[dest_len + i] = src[i];
  dest[dest_len + i] = '\0';
  
  return dest;
}
