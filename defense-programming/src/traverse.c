#include <dirent.h>
#include <sys/types.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define TNL_REGULAR  1
#define TNL_DIR      2
#define TNL_CHARSPCL 3
#define TNL_BLKSPCL  4
#define TNL_SYMLINK  5
#define TNL_FIFO     6
#define TNL_UNIXSOCK 7


/*==================================================================
 void usage()
 
 Prints out correct usage
 ==================================================================
 */
void usage(){
    printf("--------------------------------------------------------\n");
    printf("Usage: traverse directory type\n");
    printf("Valid types: \n");
    printf("\t%1d Regular\n\t%1d Directory\n\t%1d Character special\n\t%1d Block special\n\t%1d Symbolic link\n\t%1d Fifo\n\t%1d Socket\n",TNL_REGULAR,TNL_DIR,TNL_CHARSPCL,TNL_BLKSPCL,TNL_SYMLINK,TNL_FIFO,TNL_UNIXSOCK);
    printf("--------------------------------------------------------\n");
}

/*==================================================================
  char **typeList(char *dirName, mode_t typeNum)

  Returns array containing names of all files of the specified type typeNum
  within the specified directory dirName.
 
  Memory is allocated for an array of character pointers (nameList) and
  memory is allocated to hold each file name. On error, NULL is returned 
  and any previously allocated memory is freed before the return.  On success, 
  a pointer to an array of character pointers is returned; the caller must
  free all allocatentryStats2ed memory.  
 ==================================================================
*/
char **typeList(char *dirName, mode_t typeNum){

    DIR           *dirPtr;      /* Pointer to directory stream for dirName */
    struct dirent entry;        /* Info on next file in directory stream */
    struct dirent *result;      /* Pointer to return from readdir_r */
    char          *fullName;    /* Fully qualified file name */
    int           retCode;      /* Return code */
    struct stat   entryStats;   /* File info given by lstat */
    char          **nameList;   /* Array of pointers; each pointer is to a file name */
    int           ptr;	        /* Next file name goes at this index in array */

    ptr = 0; // never initiliaze it before using 
    //no usage    int           i;            /* Loop counter */
    //no usage  struct stat   entryStats2;  /* File info given by lstat */
    
    nameList= (char **)malloc(40*sizeof(char* ));       /* Allocate memory for the array of names */
  
    if (nameList==NULL) {
      perror("run out of memory \n");
      return(nameList);   /* If dynamic allocation failed, return with error indication */
    }     
    bzero(nameList,40*sizeof(char*));       /* All array elements initially zero */

    dirPtr=opendir(dirName);

    if (dirPtr==NULL){ // if could not open the directory
      perror("open dir error");
      free(nameList);
      nameList = NULL;
      return nameList; 
    } 

    retCode=readdir_r(dirPtr,&entry,&result);
    
    int threshold = 40;
    int i = 0;
    while ((retCode==0) && (result!=NULL)) {

      // The whole part is responsable for resize of dynamic array.
      if(ptr == threshold-1){  // threshold-1 is because array start from zero.
	threshold += threshold;  
	char ** temp =(char **)malloc(threshold*sizeof(char* ));       /* Allocate memory for the array of names */
	if (temp==NULL) {
	  perror("run out of memory \n");
	  for(i = 0; i< ptr;i++){
	    if(nameList[i]!=NULL)
	      free(nameList[i]);
	    nameList[i]=NULL; 
	  }
	  free(nameList);
	  nameList = NULL;
	  return(nameList);   /* If dynamic allocation failed, return with error indication */
	}
	bzero(temp,threshold*sizeof(char*));       /* All array elements initially zero */
	memcpy(temp,nameList,(threshold/2)*sizeof(char* ));
	free(nameList);
	nameList = temp;
	temp = NULL;
      }


      int length = (strlen(dirName)+strlen("/")+strlen(entry.d_name)+1); // get the dynamic size. add by leon
      
      fullName=(char *)malloc(length*sizeof(char)); // make sure fullName won't overflow.
      if (fullName==NULL) {
	perror("run out of memory \n");
	for(i = 0; i< ptr;i++){
	  if(nameList[i]!=NULL)
	    free(nameList[i]);
	  nameList[i]=NULL; 
	}
	free(nameList);
	nameList = NULL;	
	return nameList;   /* If dynamic allocation failed, return with error indication */
      }     

      bzero(fullName,length*sizeof(char));       /* All array elements initially zero */


      /* --- Create fully qualified file name 
	 --- Return from readdir_r is relative to dirName
      */
      strncpy(fullName,dirName,length);
      strncat(fullName,"/",1);
      strncat(fullName,entry.d_name,length);
      
      /* Get file information
	 Use lstat to make sure we get info on symbolic link, not target
      */
      if(lstat(fullName,&entryStats)<0){
	perror("lstat error");
	for(i = 0; i< ptr;i++){
	  if(nameList[i]!=NULL)
	    free(nameList[i]);
	  nameList[i]=NULL; 
	}
	free(nameList);
	nameList = NULL;
	return nameList;
      }
      
      switch (typeNum){
            case TNL_REGULAR:  if (S_ISREG(entryStats.st_mode)) {nameList[ptr]=fullName;ptr++;}break;    /* Regular */
            case TNL_DIR:      if (S_ISDIR(entryStats.st_mode)) {nameList[ptr]=fullName;ptr++;}break;    /* Directory */
            case TNL_CHARSPCL: if (S_ISCHR(entryStats.st_mode)) {nameList[ptr]=fullName;ptr++;}break;    /* Character special */
            case TNL_BLKSPCL:  if (S_ISBLK(entryStats.st_mode)) {nameList[ptr]=fullName;ptr++;}break;    /* Block special */
            case TNL_FIFO:     if (S_ISFIFO(entryStats.st_mode)){nameList[ptr]=fullName;ptr++;}break;    /* FIFO */
            case TNL_SYMLINK:  if (S_ISLNK(entryStats.st_mode)) {nameList[ptr]=fullName;ptr++;}break;    /* Symbolic link */
            case TNL_UNIXSOCK: if (S_ISSOCK(entryStats.st_mode)){nameList[ptr]=fullName;ptr++;}break;    /* UNIX domain socket */
	    default: {printf("<%s> Error\n",fullName); }return(NULL);                                    /* Something's very wrong */ 
        }

      // free the fullname which is equal to typeNum; 
      if(ptr==0 && fullName){
	free(fullName);
      }else if(fullName != nameList[ptr-1] && fullName ){
	free(fullName);
      }

      retCode=readdir_r(dirPtr,&entry,&result);
    }  

    if(retCode){
      perror("read dir error");
      for(i = 0; i< ptr;i++){
	if(nameList[i]!=NULL)
	  free(nameList[i]);
	nameList[i]=NULL; 
      }
      free(nameList);
      nameList = NULL;
      closedir(dirPtr);
      return nameList; 
    }
	
    if( closedir(dirPtr)){
      perror("close dir error");
      for(i = 0; i< ptr;i++){
	if(nameList[i]!=NULL)
	  free(nameList[i]);
	nameList[i]=NULL; 
      }
      free(nameList);
      nameList = NULL;
      return nameList;       
    }
    return(nameList);
}


int main(int argc, char *argv[]){

    mode_t fileType;
    char **nameList;
    int  i;
	
    if (argc!=3){ 
        usage();
	exit(1);
    }


    fileType=atoi(argv[2]);
    
    if ((fileType<1)||(fileType>TNL_UNIXSOCK)){
        usage();
	exit(2);
    }
    
    nameList=typeList(argv[1],fileType);

    if(nameList){  // may return null from typeList, so check is necessary here.
      for (i=0;nameList[i]!=(char *)0;i++){
	printf("[%d] <%s>\n",i,nameList[i]);
	if(nameList[i]!=NULL)
	  free(nameList[i]);
	nameList[i]=NULL;
      }
      free(nameList);    
      nameList = NULL;
    } else{
      exit(1);
    }
									 
    return 0; // this main function need a return value;
}
