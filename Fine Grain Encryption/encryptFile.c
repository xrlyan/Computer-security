/**************************
Project 2 from cs4471
Written by liang
Computer Science Dep.
10/24/2015
 ************************/


#include "utility.h"
#include "encryptFile.h"
#include "getKkey.h"

unsigned char* allocate_ciphertext(int mlen){
  /* Alloc enough space for any possible padding. */
  return (unsigned char*)malloc(mlen + BLOCKSIZE);
}

/*
  encrypt the input plaintext with blowfish, and output ciphertext and
  its size

*/
int encrypt(const unsigned char* plaintext,unsigned char key[],int len,
		       unsigned char* result){

  EVP_CIPHER_CTX  *ctx;                 // Context for encryption 
  EVP_CIPHER      *cipher;              // Cipher 
  char            ivec[K_FILE_LENGTH] = {0}; 
  unsigned char  *ciphertext;           // Pointer to ciphertext 

 size_t bytes_text = len ;
 int bytes_cipher = 0;  // Length of ciphertext 

  // initilize the blowfish api, assigh the key length
  cipher=(EVP_CIPHER *)EVP_bf_cbc();
  ctx = (EVP_CIPHER_CTX *)malloc(sizeof(EVP_CIPHER_CTX));
  if(ctx == NULL)
    return -1;


  EVP_CIPHER_CTX_init(ctx);
  EVP_EncryptInit_ex(ctx,cipher,NULL,NULL,NULL);
  EVP_CIPHER_CTX_set_key_length(ctx,K_FILE_LENGTH);
  EVP_EncryptInit_ex(ctx,NULL,NULL,key,ivec);
  
  // create enough space for plaintext by blowfish
  ciphertext=allocate_ciphertext(bytes_text+1); 
  if(ciphertext==NULL){
    return -1;
  }

  if(!EVP_EncryptUpdate(ctx, ciphertext, &bytes_cipher, plaintext, bytes_text)){
    EVP_CIPHER_CTX_cleanup(ctx);
    return -1;
  }

  //strncpy(result,ciphertext,bytes_cipher);
  mystrncpy(result,ciphertext,bytes_cipher);

  int total = bytes_cipher;

  if(!EVP_EncryptFinal_ex(ctx,ciphertext, &bytes_cipher)){
    EVP_CIPHER_CTX_cleanup(ctx);
    return -1;
  }
  
  //strncat(result,ciphertext,bytes_cipher);
  mystrncat(result,total,ciphertext,bytes_cipher);

  // clean the blowfish 
  EVP_CIPHER_CTX_cleanup(ctx);

  return 0;
}

/*
  the main function for encrypting.
*/

int encryptFile(char*fileName,int startPtr[],int offset[],int N,int totalSize){

  int              dec_fd;                  /* Plaintext from this file */
  int              enc_fd;                  /* ciphertext from this file */
  int bytes_write;
  int bytes_read;


  // check the magic 
  if((dec_fd=open(fileName,O_RDONLY))==-1){
    fprintf(stderr,"Open %s Error:%s\n",fileName,strerror(errno));
    return -1;
  }

  char m[5] = {0}; 
  if(lseek(dec_fd,BYTES_LENGTH,SEEK_SET) < 0){
    close(dec_fd);
    return -1;
  }
  if((bytes_read=read(dec_fd,m,BYTES_LENGTH))==0){     
    close(dec_fd);
    return -1;
  }

  char magicTemp[5] = "FISH";  
  if(strncmp(m,magicTemp,BYTES_LENGTH)==0){
    fprintf(stdout,"This file %s is already secured!\n",fileName);
    close(dec_fd);
    return -1;
  }

  // 4 for the first N
  // N*8 for meta data
  // total size for classified content
  int total_plaintext = N*8 + 4 + totalSize;
  
  int total_cypher = (total_plaintext/8 + 1 )*8; // size of cypto for plaintext above
    
  int start_ptr = BYTES_LENGTH * 9 + total_cypher; // value of start 

  unsigned char kid[BYTES_LENGTH+1]={0};       /* KID  */
  get_random_kid(kid);
  
  unsigned char magic[BYTES_LENGTH+1]={0};       /* MAGIC  */
  strncpy(magic,"FISH",BYTES_LENGTH);

  char* encFile=malloc(strlen(fileName)+5); // filename.enc 
  if(encFile == NULL){
    close(dec_fd);
    return -1;
  }
  memset(encFile,0,strlen(fileName)+5);
  get_enc_file(fileName,encFile);

  if((enc_fd=open(encFile,O_WRONLY|O_CREAT|O_TRUNC|O_NOFOLLOW,S_IRUSR|S_IWUSR))==-1){
    fprintf(stderr,"Open %s Error:%s\n",encFile,strerror(errno));
    close(dec_fd);
    free(encFile);
    return -1;
  }
 
  // convert int to char array
  char start[5]={0};
  snprintf(start, 4,"%d", start_ptr);
   
  if((bytes_write=write(enc_fd,kid,BYTES_LENGTH))==0){
     printf("write wrong %d\n");
     close(dec_fd);
     close(enc_fd);
     free(encFile);
     return -1;
 }
  if((bytes_write=write(enc_fd,magic,BYTES_LENGTH))==0){
     printf("write wrong %d\n");
     close(dec_fd);
     close(enc_fd);
     free(encFile);
     return -1;
  }
  
  if((bytes_write=write(enc_fd,start,BYTES_LENGTH))==0){
     printf("write wrong %d\n");
     close(dec_fd);
     close(enc_fd);
     free(encFile);
     return -1;
  }
 
  
  char *kkey = malloc(K_KEY_LENGTH+1);
  if(kkey == NULL){ 
     close(dec_fd);
     close(enc_fd);
     free(encFile);
    return -1;
  }
  if(mlock(kkey, K_FILE_LENGTH)<0){ 
    close(dec_fd);
    close(enc_fd);
    free(encFile);
    free(kkey);
    return -1;
  }
  memset(kkey,0,K_FILE_LENGTH+1);
  getKkey(kkey,0);

  char *kfile = malloc(K_KEY_LENGTH+1);
  if(kfile == NULL){ 
     close(dec_fd);
     close(enc_fd);
     free(encFile);
     memset(kkey,0,K_FILE_LENGTH+1);
     munlock(kkey,K_FILE_LENGTH);     
     free(kkey);
    return -1;
  }
  if(mlock(kfile, K_FILE_LENGTH)<0){ 
    close(dec_fd);
    close(enc_fd);
    free(encFile);
    memset(kkey,0,K_FILE_LENGTH+1);
    munlock(kkey,K_FILE_LENGTH);     
    free(kkey);
    free(kfile);
    return -1;
  }

  get_random_key(kfile);
 
  //20 = kid 4 + kfile 16
  char plainK[20+1] = {0};
  mystrncpy(plainK,kid,4);
  mystrncat(plainK,4,kfile,16);

  // blow fish will pad 20 to 24
  // this is the algorithm
  char cipherK[24+1]={0};

  if(encrypt(plainK,kkey,20,cipherK)<0){
    close(dec_fd);
    close(enc_fd);
    free(encFile);
    memset(kkey,0,K_FILE_LENGTH+1);
    munlock(kkey,K_FILE_LENGTH);     
    free(kkey);
    memset(kfile,0,K_FILE_LENGTH+1);
    munlock(kfile,K_FILE_LENGTH);     
    free(kfile);
    return -1;
  }

  memset(kkey,0,K_FILE_LENGTH+1);
  munlock(kkey,K_FILE_LENGTH);     
  free(kkey);


  // write encryption of kfile and kid
  if((bytes_write=write(enc_fd,cipherK,24))==0){
     printf("write wrong %d\n");
     close(dec_fd);
     close(enc_fd);
     free(encFile);
     memset(kfile,0,K_FILE_LENGTH+1);
     munlock(kfile,K_FILE_LENGTH);     
     free(kfile);
     return -1;
  }

  struct stat fstats_enc;            // Stats on ciphertext file 
  fstat(enc_fd,&fstats_enc);
   
  struct stat fstats;            //Stats on plaintext file 
  fstat(dec_fd,&fstats);
  int remain_text_size = fstats.st_size - totalSize + N;

  // using malloc
  char* remain_text = malloc(remain_text_size+1);
  if(remain_text==NULL){
    close(dec_fd);
    close(enc_fd);
    free(encFile);
    memset(kfile,0,K_FILE_LENGTH+1);
    munlock(kfile,K_FILE_LENGTH);     
    free(kfile);   
  }
  memset(remain_text,0,remain_text_size+1);

  //using malloc
  char * plaintext = malloc(total_plaintext+1);
  if(plaintext==NULL){
    close(dec_fd);
    close(enc_fd);
    free(encFile);
    memset(kfile,0,K_FILE_LENGTH+1);
    munlock(kfile,K_FILE_LENGTH);     
    free(kfile);
    free(remain_text);
    return -1;
  }

  memset(plaintext,0,total_plaintext+1);

  char c[5]={0};
  snprintf(c, BYTES_LENGTH,"%d", N);
  strncpy(plaintext,c,BYTES_LENGTH);
  int plaintext_size = BYTES_LENGTH;

  // request the segment form the text and
  // replace it with X
  int i;
  int prevStartPtr = 0;
  char x[2] = "X";
  if(lseek(dec_fd,0,SEEK_SET) < 0){ 
    close(dec_fd);
    close(enc_fd);
    free(encFile);
    memset(kfile,0,K_FILE_LENGTH+1);
    munlock(kfile,K_FILE_LENGTH);     
    free(kfile);
    free(plaintext);
    free(remain_text);   
    return -1;
  }

  for(i=0;i<N;i++){
    // handle the disclassified content at the same time   
   if(startPtr[i]==prevStartPtr){
     if(i==0)// encrypt from the first character.
	strncpy(remain_text,x,1);
      else
	strncat(remain_text,x,1);
      
      prevStartPtr = startPtr[i]+offset[i];
    }else{
      char temp[startPtr[i]+1];

      if((bytes_read=read(dec_fd,temp,startPtr[i]-prevStartPtr))==0){
	printf("read file %s wrong %d\n",fileName);
	close(dec_fd);
	close(enc_fd);
	free(encFile);
	memset(kfile,0,K_FILE_LENGTH+1);
	munlock(kfile,K_FILE_LENGTH);     
	free(kfile);
	free(plaintext);
	free(remain_text);
	return -1;
      }

      if(i==0){
	strncpy(remain_text,temp,bytes_read);
	strncat(remain_text,x,1);
      }else{
	strncat(remain_text,temp,bytes_read);
	strncat(remain_text,x,1);
      }
      prevStartPtr = startPtr[i]+offset[i];
    }

    char buffer[offset[i]+1];    
    char a[5]={0}; // a for si
    char b[5]={0}; // b for li
    snprintf(a, 4,"%d", startPtr[i]);
    snprintf(b, 4,"%d", offset[i]);

    mystrncat(plaintext,plaintext_size,a,4);
    plaintext_size += 4;

    mystrncat(plaintext,plaintext_size,b,4);
    plaintext_size += 4;
    
    if(lseek(dec_fd,startPtr[i],SEEK_SET) < 0){
      close(dec_fd);
      close(enc_fd);
      free(encFile);
      memset(kfile,0,K_FILE_LENGTH+1);
      munlock(kfile,K_FILE_LENGTH);     
      free(kfile);
      free(plaintext);
      free(remain_text);   
      return -1;
    }

    if((bytes_read=read(dec_fd,buffer,offset[i]))!=0){
      mystrncat(plaintext,plaintext_size,buffer,bytes_read);
      plaintext_size += bytes_read;
    }else{
      fprintf(stdout,"read file %s wrong %d\n",fileName);
      close(dec_fd);
      close(enc_fd);
      free(encFile);
      free(plaintext);
      memset(kfile,0,K_FILE_LENGTH+1);
      munlock(kfile,K_FILE_LENGTH);     
      free(kfile);
      free(remain_text);
      return -1;
    }

  }

  if(prevStartPtr<fstats.st_size){
    char temp[fstats.st_size-prevStartPtr+1];
    if((bytes_read=read(dec_fd,temp,fstats.st_size-prevStartPtr+1))==0){
	printf("read file %s wrong %d\n",fileName);
	close(dec_fd);
	close(enc_fd);
	memset(kfile,0,K_FILE_LENGTH+1);
	munlock(kfile,K_FILE_LENGTH);     
	free(kfile);
	free(encFile);
	free(plaintext);
	free(remain_text);
	return -1;
    }
   strncat(remain_text,temp,bytes_read);
  }
  
  char *ciphertext = malloc(total_cypher+1);
  if(ciphertext==NULL){
    close(dec_fd);
    close(enc_fd);
    free(encFile);
    memset(kfile,0,K_FILE_LENGTH+1);
    munlock(kfile,K_FILE_LENGTH);     
    free(kfile);
    free(remain_text);
    free(plaintext);
    return -1;
  }

  memset(ciphertext,0,total_cypher+1);

  if( encrypt(plaintext,kfile,total_plaintext,ciphertext)<0){
    close(dec_fd);
    close(enc_fd);
    memset(kfile,0,K_FILE_LENGTH+1);
    munlock(kfile,K_FILE_LENGTH);     
    free(kfile);
    free(encFile);
    free(plaintext);
    free(remain_text);
    return -1;
  }

  memset(kfile,0,K_FILE_LENGTH+1);
  munlock(kfile,K_FILE_LENGTH);     
  free(kfile);


  if((bytes_write=write(enc_fd,ciphertext,total_cypher))==0){
    printf("write file %s wrong %d\n",encFile);
    close(dec_fd);
    close(enc_fd);
    free(encFile);
    free(plaintext);
    free(remain_text);
    free(ciphertext);
    return -1;
  }

  if((bytes_write=write(enc_fd,remain_text,remain_text_size))==0){
    printf("write file %s wrong %d\n",encFile);
    close(dec_fd);
    close(enc_fd);
    free(encFile);
    free(remain_text);
    free(plaintext);
    free(ciphertext);
    return -1;
  }

  close(dec_fd);
  close(enc_fd);

  free(plaintext);
  free(remain_text);
  free(ciphertext);

  fprintf(stdout,"Encryption is Done \n");

  return 0;
}

