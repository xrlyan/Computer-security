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
  free all allocated memory.  
 ==================================================================
*/
char **typeList(char *dirName, mode_t typeNum){

    DIR           *dirPtr;      /* Pointer to directory stream for dirName */
    int           i;            /* Loop counter */
    struct dirent entry;        /* Info on next file in directory stream */
    struct dirent *result;      /* Pointer to return from readdir_r */
    char          *fullName;    /* Fully qualified file name */
    int           retCode;      /* Return code */
    struct stat   entryStats;   /* File info given by lstat */
    struct stat   entryStats2;  /* File info given by lstat */
    char          **nameList;   /* Array of pointers; each pointer is to a file name */
    int           ptr;	        /* Next file name goes at this index in array */

    nameList=malloc(40*sizeof(char));       /* Allocate memory for the array of names */
    bzero(nameList,sizeof(nameList));       /* All array elements initially zero */
    if (nameList==NULL) return(nameList);   /* If dynamic allocation failed, return with
					       error indication */

    dirPtr=opendir(dirName);
    if (dirPtr==NULL)return;
    retCode=readdir_r(dirPtr,&entry,&result);
    while ((retCode==0) && (result!=NULL)) {

        /* --- Create fully qualified file name 
           --- Return from readdir_r is relative to dirName
        */
        fullName=malloc(100*sizeof(char));
        strcpy(fullName,dirName);
        strcat(fullName,"/");
        strcat(fullName,entry.d_name);

        /* Get file information
           Use lstat to make sure we get info on symbolic link, not target
        */
        lstat(fullName,&entryStats);
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
        retCode=readdir_r(dirPtr,&entry,&result);
    }
    closedir(dirPtr);
    return(nameList);
}


int main(int argc, char *argv[]){

    mode_t fileType;
    char **nameList;
	int  i;
	
    if (argc!=3){ 
        usage();exit(1);
    }
    fileType=atoi(argv[2]);
    if ((fileType<1)||(fileType>TNL_UNIXSOCK)){
        usage();exit(2);
    }
    nameList=typeList(argv[1],fileType);
    for (i=0;nameList[i]!=(char *)0;i++){printf("[%d] <%s>\n",i,nameList[i]);}
												 
}
