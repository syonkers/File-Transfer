/*
  ------------------------------------------- #
  # Group Number:       1
  # Names:              Clayton Caron
  #                     Cody Moorhouse
  #		        David James
  #                     Marshall Anzinger
  #                     Shaun Yonkers
  # Group Project:      filemanager.c
  # CMPT 361:           Class Section AS40
  # Instructor's Name:  Dr. Nicholas Boers
  # ------------------------------------------- #
*/

/* Libraries */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include <dirent.h>
#include <errno.h>

#include <linux/limits.h>
#include <sys/stat.h>

/* Custom Libraries */
#include "filemanager.h"
#include "globals.h"

/* Private helper function */
void memTest(void * test) {
    if (test  == NULL) {
	/* Memory allocation error */
	exit(EXIT_FAILURE);
    }
}

/* Changes directory that user wants to move to */
/* https://stackoverflow.com/questions/1496313/returning-c-string-from-a-function */
const char * changeDir(char * dirName) {
    if (chdir(dirName) == -1) {
	
	switch (errno) {
	    
	case EACCES:
	    return "Request denied, Inadequate permissions\n";
	    
	case EFAULT:
	    return "Outside accessible address space\n";
	      
	case ENAMETOOLONG:
	    return "Path is too long\n";
	  
	case ENOENT:
	    return "File does not exist\n";
		
	default:
	    exit(EXIT_FAILURE);
	}
	
    }
    return NULL;
}

/* Gets the file names and returns a double pointer */
char ** getFileNames() {
    struct dirent ** dirListings = NULL;
    char path[PATH_MAX];
    getcwd(path , PATH_MAX);
    
    int size  = scandir(path,  &dirListings, NULL, alphasort);
    if (size < 0) {
	if (errno == ENOTDIR || errno == ENOENT) {
	    /* Directory does not exist or was given a file */
	    return NULL;
	}
	else {
	    /* Memory allocation error */
	    exit(EXIT_FAILURE);
	}
    }
    
    /* Setup so external function can get file names */
    char ** fileNames = malloc(sizeof(char *) * (size + 1));
    memTest(fileNames);
    
    for (int i =0; i <  size ; i++) {
	/* + 1 for null terminator */
	char * name = malloc(sizeof(char) * (strlen(dirListings[i]->d_name) + 1));
	memTest(name);
	strcpy(name , dirListings[i]->d_name);
	fileNames[i] = name;
	free(dirListings[i]);
    }
    
    fileNames[size] = NULL;
    
    return fileNames;
}

/* Extracts a file name from the absolute path and fills it into the buffer */
void getShortFileName(char * buffer, char * absolutePath) {
    char fileName[FILE_PATH_SZ];
    int fileIdx = 0;
    int startIdx = 0;

    while (absolutePath[startIdx] != '\0') {
	if (absolutePath[startIdx] == '/') {
	    fileIdx = 0;
	    startIdx++;
	}
	else {
	    fileName[fileIdx++] = absolutePath[startIdx++];
	}
    }

    fileName[fileIdx++] = '\0';
    strcpy(buffer, fileName);
}

/* Frees the allocated space created for the fileNames */
void freeFileNames(char ** fileNames) {
    for(int i = 0; fileNames[i] != NULL; i++) {
	free(fileNames[i]);
    }
    free(fileNames);
}


/* Determines if file, directory, or other  */
int fileTest(char * fileName) {
    struct stat info;

    if (stat(fileName, &info) == -1) {
	switch(errno) {
	case EACCES: 
	case ENAMETOOLONG:
	case ENOENT:
	case ENOTDIR:
	    /* Recoverable Errors */
	    return -1;
	    
	default:
	    exit(EXIT_FAILURE);
	}
    }
    
    if (S_ISREG(info.st_mode)) {
	/* Regular file */
	return 1;
    }
    else if (S_ISDIR(info.st_mode)) {
	/* Directory */
	return 2;
    }
    else {
	/* Other */
	return 0;
    }
}

/* Opens a file and returns a file pointer */
FILE * openFile(char * fileName) {
    struct stat info;
    FILE * file;
  
    /* Test if file exists */
    if (stat(fileName,&info) == -1) {
	file = fopen(fileName, "wb");
    }
    else {
	file = fopen(fileName, "r+b");
    }
    
    if (file == NULL) {
	/* errno */
	/* printf("error\n"); */
	exit(EXIT_FAILURE);
    }
    return file; 
}

/* Closes a file using a file pointer */
void closeFile(FILE * file) {
    if (fclose(file) == EOF) {
	exit(EXIT_FAILURE);
    }
}

/* Reads a file and returns a pointer to a buffer with its contents */
uint8_t * readFile(FILE * file, int readBlockSize) {
    if (feof(file) != 0) {
	return NULL;
    }
    
    uint8_t * buffer = malloc(sizeof(uint8_t) *  readBlockSize);
    memTest(buffer);
    
    int read = fread(buffer, sizeof(uint8_t), readBlockSize, file); 
    
    if (read != readBlockSize) {
	if (ferror(file) != 0) {
	    exit(EXIT_FAILURE);
	}
    }    
    return buffer;
}

/* Writes data to a file as specified by the file pointer */
void writeFile(FILE * file, uint8_t * data, int dataSize) { 
    int written = fwrite(data, sizeof(char), dataSize, file);
    
    if (written != dataSize) {
    	if (ferror(file) != 0) {
    	    perror("Write file error:");
    	    exit(EXIT_FAILURE);
    	}
    }
}


/* Notes what value the MD5 is being read from on the packet */
void freeKeychain(struct keychain * head) {
    for (struct keychain * worker = head; head != NULL; head = head->next) {
	closeFile(worker->OTP);
	free(worker);
	worker = head;
    }
}

/* Takes the OTP_Dir and sets up the keychain to be used */
struct keychain * setupkeychain(char * OTP_Dir) {
    char * token = NULL;
    char * priorDir = NULL;
    struct keychain * head = NULL;
    struct keychain * current = head;

    if (getcwd(priorDir, PATH_MAX) == NULL) {
	exit(1);
    }
    
    changeDir(OTP_Dir);
    char ** fileNames = getFileNames();
    
    for (int i = 0; fileNames[i] != NULL; i++) {
	if (token != NULL) {
	    current = malloc(sizeof(struct keychain));
	    current->next = head;
	    head = current;
	    
	    strcpy(current->name, fileNames[i]); 
	    current->OTP = openFile(fileNames[i]);

	    token = strtok(fileNames[i], DELIM);
	    memcpy(current->MD5, token, HASH_SZ);
	    current-> MD5[HASH_SZ] = 0;
	    token = strtok(NULL, DELIM);

	    char * end;
	    current->offset = strtol(token, &end, 10);
	    if (errno == ERANGE) {
		exit(1); /* Range error; number larger then buffer size */
	    }
	}
    }

    changeDir(priorDir);
    free(priorDir);
    freeFileNames(fileNames);
	
    return head;
}

/* Find matching MD5s */
struct keychain * findOTP(struct keychain * head, char * MD5) {
    struct keychain * iter;

    for (iter = head; iter != NULL; iter = iter->next) {
	if (strcmp(iter->MD5, MD5) == 0) {
	    break;
	}
    }
    return iter;
}

/* Helper for OTP */
int availableOTP(struct keychain * OTP, int size) {
    struct stat info;
    
    if (stat(OTP->name, &info) == -1) {
	switch (errno) {
	case EACCES: 
	case ENAMETOOLONG:
	case ENOENT:
	case ENOTDIR:
	    /* Recoverable Errors */
	    return 0;
	    
	default:
	    exit(EXIT_FAILURE);
	}
    }
    
    if (info.st_size > (OTP->offset + size)) {
	/* Enough OTP for encryption */
	return 1;
    }
    /* Not enough OTP for encryption */
    return 0;
}
   
/* Head for all OTP; MD5 to match them; Size to change offset on keychain */
FILE * getOTP(struct keychain * head, char * MD5, int size) {
    struct keychain * OTPinfo = findOTP(head, MD5);
    if (OTPinfo == NULL) {
	/* Not OTP with given MD5 */
	return NULL; 
    }
    
    int work = availableOTP(OTPinfo, size);
    if (work == 0) {
	return NULL;
    }

    if (fseek(OTPinfo->OTP, size, SEEK_CUR) != 0) {
	return NULL;
    }
    
    int file = fileno(OTPinfo->OTP);
    if (file == -1) {
	return NULL;
    }

    FILE * OTP  = fdopen(file, "rb");
    if (fseek(OTP, OTPinfo->offset + size, SEEK_SET) != 0) {
	exit(EXIT_FAILURE);
    }
    
    return OTP;
}
