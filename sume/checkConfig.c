#include "header.h"

int checkConfig(char* fp, char* hostUsr) {

 
  char username[100];
  char ch;
  int fd, fdhd;
  char* cfgfile;
  struct stat stats, statssume;
  struct passwd *user;
  uid_t ruid, euid, suid;

  memset(username, 0, 101);

  char *cfgFilePath = malloc(strlen(fp)+9+1);
  if (cfgFilePath == NULL) {
    fprintf(stdout, "Unable to allocate mem for cfgFilePath");
    return -1;
  }

  strncpy(cfgFilePath, fp, strlen(fp)+9+1);
  strncat(cfgFilePath, "/.sumeCfg", 9);
  if ((fd = open(cfgFilePath, O_RDONLY)) == -1) {
    free(cfgFilePath);
    fprintf(stderr, "Open %s Error:%s\n", cfgFilePath, strerror(errno));
    return -1;
  }

  /* Check that the configuration file is not group or world writeable */
  if (fstat(fd, &stats) == -1) {
    free(cfgFilePath);    
    fprintf(stdout, "fstat on fd failed");
    return -1;
  }
  if(stats.st_mode & S_IWGRP) {
    free(cfgFilePath);
    fprintf(stdout, "Configuration file has group write permission.");
    return -1;
  }
  if (stats.st_mode & S_IWOTH) {
    free(cfgFilePath);
    fprintf(stdout, "Configuration file has world write permission.");
    return -1;
  }
  
  /* Check that the config file is owned
     by the same user as home directory containing sume */
  user = getpwnam(hostUsr);
  if (user->pw_uid != stats.st_uid) {
    free(cfgFilePath);
    fprintf(stdout, "UIDs of sume directory and .sumeCfg did not match");
    return -1;
  }

  /* Get process RUID, i.e. the UID of the guest user executing sume */
  int gooduser = -1;
  if (getresuid(&ruid, &euid, &suid) == -1) {
    free(cfgFilePath);
    fprintf(stderr, "getresuid error:%s\n", strerror(errno));
  }

  /* Open fd as a file object to use fgetc; does not create a TOCTOU error
     as fd is not closed */
  FILE * ffd;
  char mode[2] = "r";
  uid_t alloweduid;
  ffd = fdopen(fd, mode);
  if (ffd == NULL) {
    free(cfgFilePath);
    fprintf(stderr, "Open .sumeCfg as a file object failed. Error:%s\n",
	    strerror(errno));
    return -1;
  }

  /* Read characters from config file to create usernames.
     Compare UIDs for usernames to guest user UID */
  int i = 0;
  while ((ch = fgetc(ffd)) != EOF) {
    // if alphabet char
    if ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122)) {
      if (i < 100) {
	username[i] = ch ;
	//strncat(*username, *ch, 1);
      } else {
	fprintf(stdout, "Username in config file too long\n");
	return -1;
      }
      i++;
    } else if (ch == 32 || ch == 10 || ch == 9) {  
      // If space, tab, or new line  
      user = getpwnam(username);
      alloweduid = user->pw_uid;
      if(alloweduid == ruid){
	memset(username, 0, 101);
	gooduser = 0;
	break;
      }
	    
      i = 0;
      memset(username, 0, 101);
    } else {
      fprintf(stdout,
	      "The configuration file contained an invalid character.");
      return -1;
    }
  }

  fclose(ffd); // Automatically closes both ffd and fd

  if (gooduser != 0) {
    if(strlen(username)){
      user = getpwnam(username);
      alloweduid = user->pw_uid;
      if(alloweduid == ruid) {
	      gooduser = 0;
        return 0;
      }
    }
	    
    fprintf(stdout, "User does not have permission to use sume \n");
    return -1;
  }
  return gooduser;
}
	
