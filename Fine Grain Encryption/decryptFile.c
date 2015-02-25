/**************************
Project 2 from cs4471
Written by liang
Computer Science Dep.
10/26/2015
 ************************/

#include "decryptFile.h"
#include "utility.h"
#include "getKkey.h"

/*
  the main decrypt function
*/
int decrypte(EVP_CIPHER_CTX *dctx,unsigned char*ct, int ctlen,unsigned char* pt){

  int             outlen = 0;
  unsigned char  *res = (unsigned char *)malloc(ctlen);
  int             nbytes = 0;
  

  if(!EVP_DecryptUpdate(dctx, res, &outlen, ct, ctlen)){
    free(res);
    return -1;
  }
    
  nbytes+=outlen;
  
  if(!EVP_DecryptFinal_ex(dctx, &res[outlen], &outlen)){
    free(res);
    return -1;	  
  }
  
  nbytes+=outlen;

  mystrncpy(pt,res,nbytes);
  free(res);

  return 0;
}


/*
  the main function for fge -c
*/
int decryptFile(char* fileName){
  
    
  char* encFile=malloc(strlen(fileName)+5); // filename.enc 
  if(encFile == NULL)
    return -1;
  memset(encFile,0,strlen(fileName)+5);
  get_enc_file(fileName,encFile);
  
  /* Read ciphertext */
  int fd=open(encFile,O_RDONLY);
  if(fd<0){
    fprintf(stderr,"Open %s Error:%s\n",encFile,strerror(errno));
    free(encFile);
    return -1;
  }

  struct stat     fstats;            /* Stats on ciphertext file */
  fstat(fd,&fstats);
  
  unsigned char kid[BYTES_LENGTH+1]={0};       /* KID  */
  if(read(fd,kid,BYTES_LENGTH)==0){
    fprintf(stdout,"read error for %s \n",encFile);
    close(fd);
    free(encFile);
    return -1;
  }	
  
  unsigned char magic[BYTES_LENGTH+1]={0};       /* MAGIC  */
  if(read(fd,magic,BYTES_LENGTH)==0){
    fprintf(stdout,"read error for %s \n",encFile);
    close(fd);
    free(encFile);
    return -1;
  }	
  
  char start[BYTES_LENGTH+1]={0};                // start
  if(read(fd,start,BYTES_LENGTH)==0){	
    fprintf(stdout,"read error for %s \n",encFile);
    close(fd);
    free(encFile);
    return -1;
  }	
  
  int startPtr = atoi(start);
  if(!start){
    fprintf(stdout,"could not get start position \n");
    close(fd);
    free(encFile);
    return -1;
  }	
  
  int cipherText_size = startPtr - 36;
  int remain_text_size = fstats.st_size - startPtr;

  int ctlen = 24;  /* Length of ciphertext */
  
  // malloc here
  char *cipherKfile = malloc(ctlen+1);
  if(cipherKfile==NULL){
    fprintf(stdout,"malloc error for cipher \n");
    close(fd);
    free(encFile);
    return -1;    
  }
  memset(cipherKfile,0,ctlen+1);
  if(read(fd,cipherKfile,ctlen)==0){
    fprintf(stdout,"read error \n");
    close(fd);
    free(encFile);
    free(cipherKfile);
    return -1;    
  }

  unsigned char *plainKfile = malloc(20+1);
  if(plainKfile==NULL){
    fprintf(stdout,"malloc error for cipher \n");
    close(fd);
    free(encFile);
    free(cipherKfile);
    return -1;    
  }
  memset(plainKfile,0,20+1);
  

  EVP_CIPHER_CTX  *dctx;              /* Context for decryption */
  EVP_CIPHER     *cipher;            /* Cipher for decryption */
  unsigned char   ivec[EVP_MAX_IV_LENGTH] = {0};
  
  /* Decrypt */
  cipher=(EVP_CIPHER *)EVP_bf_cbc();
  dctx=(EVP_CIPHER_CTX *)malloc(sizeof(EVP_CIPHER_CTX));
  
  EVP_DecryptInit_ex(dctx,cipher,NULL,NULL,NULL);

  char *key = malloc(K_KEY_LENGTH+1);
  if(key == NULL){ 
     close(fd);
     free(encFile);
    return -1;
  }
  if(mlock(key, K_FILE_LENGTH+1)<0){ 
    close(fd);
    free(encFile);
    free(key);
    return -1;
  }
  memset(key,0,K_FILE_LENGTH+1);

  
  getKkey(key,1);

  
  EVP_CIPHER_CTX_set_key_length(dctx,K_FILE_LENGTH);
  EVP_DecryptInit_ex(dctx,NULL,NULL,key,ivec);
  

  if(decrypte(dctx,cipherKfile,24,plainKfile)<0){
    fprintf(stdout,"Incorrect passphrase, please try again! \n");
    close(fd);
    free(encFile);
    memset(key,0,K_FILE_LENGTH+1);
    mlock(key, K_FILE_LENGTH+1);
    free(key);
    return -1;

  }

  memset(key,0,K_FILE_LENGTH+1);
  mlock(key, K_FILE_LENGTH+1);
  free(key);
  
  char mykid[BYTES_LENGTH+1] = {0};
  mystrncpy(mykid,plainKfile,BYTES_LENGTH);

  char *mykfile = malloc(K_KEY_LENGTH+1);
  if(mykfile == NULL){ 
     close(fd);
     free(encFile);
    return -1;
  }
  if(mlock(mykfile, K_FILE_LENGTH)<0){ 
    close(fd);
    free(encFile);
    free(mykfile);
    return -1;
  }
  mystrncpy(mykfile,plainKfile+BYTES_LENGTH,16);
  
  free(plainKfile);
  free(cipherKfile);

  // check the kid if match or not
  if(strncmp(mykid,kid,BYTES_LENGTH)!=0){
    memset(mykfile, 0, FILEKEYLEN+1);
    munlock(mykfile, FILEKEYLEN+1);
    free(mykfile);
    close(fd);
    free(encFile);
    fprintf(stdout,"Incorrect passphrase, please try again! \n");   
  return -1;
  }
  
  unsigned char kkey[FILEKEYLEN]= {0};
  char *cipherText = malloc(cipherText_size+1);
  if(cipherText == NULL){
    fprintf(stderr,"Malloc error for cipherText! \n");
    close(fd);
    free(encFile);
    return -1;
  } 

  memset(cipherText,0,cipherText_size+1);
  if(read(fd,cipherText,cipherText_size)==0){
    fprintf(stderr,"Malloc error for cipherText! \n");
    close(fd);
    free(encFile);
    free(cipherText);
    return -1;
  }
  
  
  char *plainText = malloc(cipherText_size);
  if(plainText == NULL){
    fprintf(stderr,"Malloc error for plainText! \n");
    close(fd);
    free(encFile);
    return -1;
  } 
  memset(plainText,0,cipherText_size);
  
  EVP_DecryptInit_ex(dctx,NULL,NULL,mykfile,ivec);

  if(decrypte(dctx,cipherText,cipherText_size,plainText)<0){
    close(fd);
    free(encFile);
    memset(mykfile, 0, FILEKEYLEN+1);
    munlock(mykfile, FILEKEYLEN+1);
    free(mykfile);
    free(plainText);
    free(cipherText);
    return -1;
  }
  
  // clean the blowfish 
  EVP_CIPHER_CTX_cleanup(dctx);

  memset(mykfile, 0, FILEKEYLEN+1);
  munlock(mykfile, FILEKEYLEN+1);
  free(mykfile);
  free(cipherText);

    
  char* remainText = malloc(remain_text_size+1);
  if(remainText == NULL){
    fprintf(stderr,"Malloc error for remainText! \n");
    close(fd);
    free(encFile);
    free(plainText);
    return -1;
  } 

  memset(remainText,0,remain_text_size+1);
  
  if(read(fd,remainText,remain_text_size)==0){
    fprintf(stdout,"Read error \n");
    close(fd);
    free(encFile);
    free(plainText);
    free(remainText);
    return -1;
  }
  
  close(fd);
  free(encFile);
 
  // output the result to stdout
  fprintf(stdout,"Plaintext is : \n<");
  
  char count[5] = {0};
  mystrncpy(count,plainText,BYTES_LENGTH);

  int N = atoi(count);
  int segment[N];
  int offset[N];
  int i = 0;
  
  char *ptrR = remainText;
  char *ptrP = plainText+BYTES_LENGTH;//skip N

  while(i<N){
    char a[5] = {0};
    char b[5] = {0};
    mystrncpy(a,ptrP,BYTES_LENGTH);
    ptrP = ptrP+BYTES_LENGTH;
    mystrncpy(b,ptrP,BYTES_LENGTH);
    ptrP = ptrP+BYTES_LENGTH;
    
    segment[i] = atoi(a);
    offset[i] = atoi(b);
    
    char *content = malloc(offset[i]+1);
    if(content==NULL){
      fprintf(stderr,"Malloc error for content! \n");
      free(remainText);
      free(plainText);
      return -1;
    } 
          
    memset(content,0,offset[i]+1);
    mystrncpy(content,ptrP,offset[i]);
    ptrP = ptrP + offset[i];
    
    if(i==0){
      if(segment[i]==0){
	fprintf(stdout,"%s",content);
	ptrR++;
      }else{
	char *temp = malloc(segment[i]+1);
	if(temp==NULL){
	  fprintf(stderr,"Malloc error for content! \n");
	  free(remainText);
	  free(plainText);
	  free(content);
	  return -1;
	} 

	memset(temp,0,segment[i]+1);
	mystrncpy(temp,ptrR,segment[i]);
	fprintf(stdout,"%s",temp);
	ptrR += segment[i];
	free(temp);
	fprintf(stdout,"%s",content);
	ptrR++;
      }
    }else{
      if(segment[i]==segment[i-1]+offset[i-1]){
	fprintf(stdout,"%s",content);
	ptrR++;
      }else{
	char *temp = malloc(segment[i]-segment[i-1]-offset[i-1]+1);
	if(temp==NULL){
	  fprintf(stderr,"Malloc error for content! \n");
	  free(remainText);
	  free(plainText);
	  free(content);
	  return -1;
	} 
	memset(temp,0,segment[i]-segment[i-1]-offset[i-1]+1);
	mystrncpy(temp,ptrR,segment[i]-segment[i-1]-offset[i-1]);
	fprintf(stdout,"%s",temp);
	ptrR = ptrR + segment[i]-segment[i-1]-offset[i-1];
	free(temp);
	fprintf(stdout,"%s",content);
	ptrR++;
      }
    }
    free(content);
    i++;
  }


  // cat the last thing in remain_text;
  if(ptrR !='\0')
    fprintf(stdout,"%s",ptrR);
  fprintf(stdout,">\n");


  free(plainText);
  free(remainText);

  return 0;
  
}


