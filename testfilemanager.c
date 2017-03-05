/*
  ------------------------------------------- #
  # Group Number:       1
  # Names:              Clayton Caron
  #                     Cody Moorhouse
  #		      David James
  #                     Marshall Anzinger
  #                     Shaun Yonkers
  # Group Project:      testfilemanager.c
  # CMPT 361:           Class Section AS40
  # Instructor's Name:  Dr. Nicholas Boers
  # ------------------------------------------- #
*/

/* Libaries */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

/* Custom Libraries */
#include "filemanager.h"

/* Static Variables */
static bool verbose = false;
static bool verbose2 = false;

/* Helper functions */
void printNames(char ** fileNames) {
    printf("\n\n");

    for (int i = 0; fileNames[i] != NULL; i++) {
	printf("[%s]\n", fileNames[i]);
    }
}

/* Helper testing function */
bool testDirList(char ** fileNames, char testNames[25][15]) {
    for (int i = 0; fileNames[i] != NULL; i++) {
	if( strcmp( fileNames[i], testNames[i]) != 0) {
	    return false;
	}
    }
    return true;
}

void testRecognition() {
    if (verbose)
	printf("testing file/Directory recognition ...");
    
    /* File Test */
    if (verbose2) 
	printf("\ntesting regular file ...");  
    if (fileTest("testDir/testfile.txt") != 1) {
	exit(EXIT_FAILURE);
    }
    if (verbose2) 
	printf("pass!\n");

    /* Directory Test */
    if (verbose2) 
	printf("testing directory ...");
    if (fileTest("testDir") != 2) {
	exit(EXIT_FAILURE);
    }    
    if (verbose2) 
	printf("pass!\n");
    
    /* Other Test */
    if (verbose2) 
	printf("testing other ...");
    if (fileTest("/dev/null") != 0) {
	exit(EXIT_FAILURE);
    }
    if (verbose2) 
	printf("pass!\n");
    
    /* Everything Passed */
    if (verbose) 
	printf("pass!\n");
}

void testFileFunctions() {
    char testReadFile[]  = "testDir/testfile.txt";
    char testWriteFile[] = "testDir/testwritefile.txt";

    if (verbose) 
	printf("testing file open {%s}...", testReadFile);
    FILE * read = openFile(testReadFile);
    FILE * write = openFile(testWriteFile);
    
    uint8_t * block;
    
    while ((block = readFile(read, TEST_MAX_BLOCK_SZ)) != NULL) {
	/* block = readFile(read, 10); */
	if (verbose2) 
	    printf("%s\n",block);
	writeFile(write, block, strlen((char *)block));
    }   
    
    closeFile(read);
    closeFile(write);

    int diff = system("diff testDir/testfile.txt  testDir/testwritefile.txt");

    if( diff != 0){
	fprintf(stderr,"ERROR: files do not match or no shell avalable");
	exit(EXIT_FAILURE);
    }

    int deleted = remove(testWriteFile);

    if(deleted != 0){
	fprintf(stderr,"ERROR: could not delete file %s",testWriteFile);
    }
    
    if (verbose) 
	printf("pass!\n");
    /* End of file function testing */
}

void testDirectoryFuncions() {

    /* Test variables */
    char testNames[25][15] = {
	".", "..", ".readahead", ".snapshots", "bin", "boot","dev", "etc", "home",
	"lib", "lib64", "mnt", "opt", "proc", "root", "run", "sbin", "selinux", 
	"srv", "sys", "tmp", "usr", "var"
    };
    
    char testDirTrav[] = "/etc";
    /* End of test variables */
    
    /* Holds current directory file names */
    char ** fileNames;
    const char * error;
    
    if (verbose) 
	printf("testing file retrieval for {'/'}...");
    
    /* Testing changing directories */
    error = changeDir("/");
    if( error != NULL) {
	fprintf(stderr,"%s\n",error);
	exit(EXIT_FAILURE);
    }
    
    /* Testing file name retrieval */
    if ((fileNames = getFileNames()) == NULL) {
	exit(EXIT_FAILURE);
    }
    
    if (verbose2) 
	printNames(fileNames);
    
    /* Testing if getFileNames retrieved names correctly */
    if (!testDirList(fileNames, testNames)) 
	exit(EXIT_FAILURE);
    else if(verbose) 
	printf("Pass!\n");
    freeFileNames(fileNames);
    
    
    if (verbose) 
	printf("testing directory Traversal from '/' -> '/var' -> '../etc'...");
    if((error = changeDir("var")) != NULL) {
	fprintf(stderr,"%s\n",error);
	exit(EXIT_FAILURE);
    }
    
    if ((error = changeDir("../etc")) != NULL) {
	fprintf(stderr,"%s\n",error);
	exit(EXIT_FAILURE);
    }
    
    fileNames = getFileNames();
    if (verbose2) 
	printNames(fileNames);
    
    freeFileNames(fileNames);
        
    char testedDirTrav[30];
    if (getcwd(testedDirTrav, 30) == NULL) {
	exit(EXIT_FAILURE);
    }
    if (strcmp(testedDirTrav, testDirTrav) != 0) {
	exit(EXIT_FAILURE);
    }
    if (verbose) 
	printf("Pass!\n");
    /* End of directory test */
}

int main(int argc, char **argv) {
    int flag;

    while ((flag = getopt (argc, argv, "v")) != -1) {
	switch (flag) {
	case 'v':
	    if(verbose) 
		verbose2 = true;
	    verbose = true;
	    break;
		
	}
    }
    
    /* Tests for file/directory recognition */ 
    testRecognition();
    
    /* Tests for file functions */
    testFileFunctions();
    
    /* Test directory function testing */
    testDirectoryFuncions();
    
    return EXIT_SUCCESS;
}
