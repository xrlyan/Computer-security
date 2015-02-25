/**************************
Project 2 from cs4471
Written by liang & jaclyn
Computer Science Dep.
10/14/2015
 ************************/

#include "keyVerify.h"
#include "utility.h"

/*

*/
int keyVerify(char* fileName) {

        
  struct stat     fstats;            // Stats on ciphertext file 
  
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
  
 char magicTemp[5] = "FISH";  
  if(strncmp(magic,magicTemp,BYTES_LENGTH)!=0){
    fprintf(stdout,"This is not a secure file \n");
    close(fd);
    free(encFile);
    return -1;
  }
  
 
  char start[BYTES_LENGTH+1]={0};
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


  int cipherText_size = startPtr - 36; // 36 is the crypto size of kfile and kid 24
                                       //  + kid 4 + magic 4 + start 4
  // the disclassified content size
  int remain_text_size = fstats.st_size - startPtr;

  int  ctlen=6*BYTES_LENGTH;             // Length of ciphertext 
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
  
  EVP_CIPHER_CTX  *dctx;              /* Context for decryption */
  EVP_CIPHER     *cipher;            /* Cipher for decryption */
  unsigned char   ivec[EVP_MAX_IV_LENGTH] = {0};
  
  /* Decrypt */
  cipher=(EVP_CIPHER *)EVP_bf_cbc();
  dctx=(EVP_CIPHER_CTX *)malloc(sizeof(EVP_CIPHER_CTX));
  
  EVP_DecryptInit_ex(dctx,cipher,NULL,NULL,NULL);

  char* kkey = malloc(FILEKEYLEN+1); 
  if(kkey == NULL) return -1;
  if(mlock(kkey, FILEKEYLEN) < 0) return -1;
  memset(kkey, 0, FILEKEYLEN+1);
  
  getKkey(kkey,1);

  if(kkey == NULL) {
    memset(kkey, 0, FILEKEYLEN);
    munlock(kkey, FILEKEYLEN);
    free(kkey);
    return 1;
  }
  
  EVP_CIPHER_CTX_set_key_length(dctx,K_FILE_LENGTH);
  EVP_DecryptInit_ex(dctx,NULL,NULL,kkey,ivec);
  
  unsigned char *plainKfile = malloc(20+1);
  memset(plainKfile,0,20+1);  

  if(decrypte(dctx,cipherKfile, ctlen,plainKfile)<0){
   fprintf(stderr,"Incorrect passphrase, please try again! \n");
    memset(kkey, 0, FILEKEYLEN);
    munlock(kkey, FILEKEYLEN);
    free(kkey);
    free(plainKfile);
    return -1;
  }
  
  char mykid[BYTES_LENGTH+1] = {0};
  strncpy(mykid,plainKfile,BYTES_LENGTH);

  free(plainKfile);

  close(fd);

  if(strncmp(mykid,kid,K_KEY_LENGTH)!=0){
    fprintf(stderr,"Incorrect passphrase, please try again! \n");
    memset(kkey, 0, FILEKEYLEN);
    munlock(kkey, FILEKEYLEN);
    free(kkey);
    return -1;
  }else{
    fprintf(stdout,"kkey is: \n <");
    fprint_string_as_hex(stdout, kkey, FILEKEYLEN);
    fprintf(stdout,">\n");
    memset(kkey, 0, FILEKEYLEN);
    munlock(kkey, FILEKEYLEN);
    free(kkey);
    return 0;
  }

}
