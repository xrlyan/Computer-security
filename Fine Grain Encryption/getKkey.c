/**************************
Project 2 from cs4471
Written by jaclyn
Computer Science Dep.
10/14/2015
 ************************/

#include "getKkey.h"
#include "utility.h"


// initialReq: If 0, ask for password twice and compare. If 1, just once.
char*
getInput(char* phrase1, int initialReq)
{

  int 	plen;		    // Length of phrase	
  char*   phrase2;          // Passphrase repeated for comparison
  struct termios ts, ots;

  // Get and save current termios settings
  tcgetattr(STDIN_FILENO, &ts);
  ots = ts;

  //Change and set new termios settings to turn off echo
  ts.c_lflag &= ~ECHO;
  ts.c_lflag |= ECHONL;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &ts);

  // Check setting took effect
  tcgetattr(STDIN_FILENO, &ts);
  if (ts.c_lflag & ECHO) {
    fprintf(stderr, "Failed to turn off echo\n");
    tcsetattr(STDIN_FILENO, TCSANOW, &ots);
    return NULL;
  }
  		
  if (initialReq == 0) {
    
    phrase2 = malloc(PASSPHRASELEN);
    if (phrase2 == NULL) return NULL;
    if(mlock(phrase2, PASSPHRASELEN)<0) {
      free(phrase2);
      return NULL;
    }
    memset(phrase2, 0, PASSPHRASELEN);	
        
    /* Get the passphrase */ 
    printf("Input passphrase: ");
    fgets(phrase1, PASSPHRASELEN, stdin);
    if (phrase1 == NULL) {
      /* Turn echo back on and unlock memory*/
      tcsetattr(STDIN_FILENO, TCSANOW, &ots);
      memset(phrase1, 0, PASSPHRASELEN);
      memset(phrase2, 0, PASSPHRASELEN);
      munlock(phrase2, PASSPHRASELEN);
      free(phrase2);
      return NULL;
    }
    
    /* Get the passphrase again and compare to the first*/ 
    printf("Input passphrase again: ");
    fgets(phrase2,PASSPHRASELEN, stdin);
    if (phrase2 == NULL) {
      /* Turn echo back on and unlock memory*/
      tcsetattr(STDIN_FILENO, TCSANOW, &ots);
      memset(phrase1, 0, PASSPHRASELEN);
      
      memset(phrase2, 0, PASSPHRASELEN);
      munlock(phrase2, PASSPHRASELEN);
      free(phrase2);
      return NULL;
    }
    
    /* Compare passphrases and repeat request if they don't match*/
    while (strcmp(phrase1, phrase2) != 0) {
      printf("The passphrases did not match. Please try again.\n");
      
      /* Get the passphrase */ 
      printf("Input passphrase: ");
      fgets(phrase1, PASSPHRASELEN, stdin);
      if (phrase1 == NULL) {
	/* Turn echo back on and unlock memory*/
	tcsetattr(STDIN_FILENO, TCSANOW, &ots);
	memset(phrase1, 0, PASSPHRASELEN);
	return NULL;
      }
      /* Get the passphrase again and compare to the first*/ 
      printf("Input passphrase again: ");
      fgets(phrase2, PASSPHRASELEN, stdin);
      if (phrase2 == NULL) {
	/* Turn echo back on and unlock memory*/
	tcsetattr(STDIN_FILENO, TCSANOW, &ots);
	memset(phrase1, 0, PASSPHRASELEN);
	
	memset(phrase2, 0, PASSPHRASELEN);
	munlock(phrase2, PASSPHRASELEN);
	free(phrase2);
	return NULL;
      }
    }    
    memset(phrase2, 0, PASSPHRASELEN);
    munlock(phrase2, PASSPHRASELEN);
    free(phrase2);    
  } else {   
    /* Get the passphrase */ 
    printf("Input passphrase: ");
    fgets(phrase1, PASSPHRASELEN, stdin);
    if (phrase1 == NULL) {
      /* Turn echo back on and unlock memory*/
      tcsetattr(STDIN_FILENO, TCSANOW, &ots);
      memset(phrase1, 0, PASSPHRASELEN);
      return NULL;
    }
  }
 
  tcsetattr(STDIN_FILENO, TCSANOW, &ots);
  return phrase1;
}
int
getKkey(char*kkey, int initialReq)
{
  unsigned char   *ekey;      /* Key to encipher file encryption key */
  char*   passphrase;          /* Passphrase repeated for comparison*/		
  
  int             mlen;                 /* Length of message to encrypt */
  
  unsigned char  *ciphertext;
  int             ctlen;                /* Length of ciphertext */
  
  EVP_MD_CTX      *mdctx;               /* Context for SHA1 */
  int             mdlen;                /* Length of SHA1 */
  
  int 		fd;		         /* Will write ciphertext to this file */
  
  
  ekey = malloc(SHA_DIGEST_LENGTH+1);
  if(ekey == NULL) return -1;
  if(mlock(ekey, SHA_DIGEST_LENGTH)<0){ 
    free(ekey);
    return -1;
  }
  memset(ekey, 0, SHA_DIGEST_LENGTH);
  
  passphrase = malloc(PASSPHRASELEN);
  if(passphrase == NULL){
    munlock(ekey, SHA_DIGEST_LENGTH);
    free(ekey);
    return -1;
  } 

  if(mlock(passphrase, PASSPHRASELEN)<0){ 
    munlock(ekey, SHA_DIGEST_LENGTH);
    free(ekey);
    free(passphrase);
    return -1;
  }
  memset(passphrase, 0, PASSPHRASELEN);
  
  
  // Get passphrase from the user	
  if(getInput(passphrase, initialReq) == NULL){
    munlock(ekey, SHA_DIGEST_LENGTH);
    free(ekey);
    
    memset(passphrase, 0, PASSPHRASELEN);
    munlock(passphrase, PASSPHRASELEN);
    free(passphrase);
    return -1;
  }
  
  /* Compute the digest */
  mdctx=(EVP_MD_CTX *)malloc(sizeof(EVP_MD_CTX));
  if(mdctx == NULL){
    munlock(ekey, SHA_DIGEST_LENGTH);
    free(ekey);
    
    memset(passphrase, 0, PASSPHRASELEN);
    munlock(passphrase, PASSPHRASELEN);
    free(passphrase);
    return -1;
  }

  EVP_MD_CTX_init(mdctx);
  EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL);

  if(!EVP_DigestUpdate(mdctx, passphrase, strlen(passphrase)-1)){
    munlock(ekey, SHA_DIGEST_LENGTH);
    free(ekey);
    
    memset(passphrase, 0, PASSPHRASELEN);
    munlock(passphrase, PASSPHRASELEN);
    free(passphrase);
 
    EVP_MD_CTX_cleanup(mdctx);
    return -1;
  }

  if(!EVP_DigestFinal_ex(mdctx, ekey, &mdlen)){
    munlock(ekey, SHA_DIGEST_LENGTH);
    free(ekey);
    
    memset(passphrase, 0, PASSPHRASELEN);
    munlock(passphrase, PASSPHRASELEN);
    free(passphrase);
    EVP_MD_CTX_cleanup(mdctx);
    return -1;
  }

  EVP_MD_CTX_cleanup(mdctx);
  
  if (mdlen != SHA_DIGEST_LENGTH) {
    munlock(ekey, SHA_DIGEST_LENGTH);
    free(ekey);
    
    memset(kkey, 0, FILEKEYLEN);
    
    memset(passphrase, 0, PASSPHRASELEN);
    munlock(passphrase, PASSPHRASELEN);
    free(passphrase);
    
    return -1;
  }
   
  if(mystrncpy(kkey, ekey, FILEKEYLEN) == NULL) {
    memset(ekey, 0, SHA_DIGEST_LENGTH);
    munlock(ekey, SHA_DIGEST_LENGTH);
    free(ekey);
    
    memset(kkey, 0, FILEKEYLEN);
    
    memset(passphrase, 0, PASSPHRASELEN);
    munlock(passphrase, PASSPHRASELEN);
    free(passphrase);	
    return -1;
  }
  
  /* Clear and free buffers after use, except kkey*/
  memset(ekey, 0, SHA_DIGEST_LENGTH);
  munlock(ekey, SHA_DIGEST_LENGTH);
  free(ekey);
  
  memset(passphrase, 0, PASSPHRASELEN);
  munlock(passphrase, PASSPHRASELEN);
  free(passphrase);
   
  return 0;
}
