/*
------------------------------------------- #
# Group Number:       1
# Names:              Clayton Caron
#                     Cody Moorhouse
#		      David James
#                     Marshall Anzinger
#                     Shaun Yonkers
# Group Project:      filemanager.h
# CMPT 361:           Class Section AS40
# Instructor's Name:  Dr. Nicholas Boers
# ------------------------------------------- #
*/

#ifndef _filemanager_h_
#define _filemanager_h_

/* Libraries */
#include <stdint.h>
#include <linux/limits.h>

/* Custom Libraries */
#include "globals.h"

/* Struct */
struct keychain {
    char MD5[HASH_SZ + 1];
    char name[NAME_MAX];
    FILE * OTP;
    unsigned long long int offset;
    struct keychain * next;
};




/*
Input:
    String that contains desired directory address change 
    (can be relative or absolute path)
    Note: initial starting directory is where program was originally executed 
 
Output:
    Returns NULL on success or an error message related to related to a
    recoverable error
 
Process: 
     Changes current directory to given directory
     On failure, checks if error is recoverable. If so, returns an error message
     or else it will exit program with “EXIT_FAILURE” status
*/
const char * changeDir(char * dirName);




/*
Input:  
    No input needed: void function

Output: 
    Returns an array of strings containing the file names of the 
    current directory
    NOTE: Array is in allocated memory, use 'freeFileNames()' to free array 
    NOTE: The last pointer in the array will always be set to NULL and can 
          be used to find array size
  
    On error, if the error is recoverable it sends back NULL, which indicates
    that the directory passed does not exist. Any fatal error will exit the
    program

Process: 
    Retrieves the current working directory (CWD)
    Attempts to create an array of the files information  contained in the CWD
    It will then create an array of file names from the files information
*/
char ** getFileNames(); 




/*
Input: 
    Takes in 2 dimensional array of pointers that point to allocated memory

Output: 
    Returns nothing; void

Process: 
    Frees all allocated memory within the 2 dimensional array and then 
    frees the 2 dimensional array itself
*/
void freeFileNames(char ** fileNames);




/*
Input:
    Takes in a string that contains a name of a “file” to be determine the type
    of file that it is.

Output:
    Returns 
        -1  = recoverable error such as access denied
         1  = regular file
         2  = directory 
         0 = other file type

Process: 
    Determines if given “file” is a regular file, directory, or other type
*/
int fileTest(char * fileName);




/*
Input:
    Takes in string referring to a file that will be opened

Output:
    Returns file pointer to a given file

Process: 
    Creates a file pointer for a given file and will set the 
    file size variable
*/
FILE * openFile(char * fileName);




/*
Input: 
    Takes in a file pointer

Output:
    Returns nothing; void

Process:
    Closes a file stream; 
*/
void closeFile( FILE * file );




/*
Input: 
    Takes in a file pointer for the file to be read from and an in int that 
    sets how much of the file is to be read

Output:
    Returns an array of chars of the read data that is allocated memory, 
    and must be freed

Process: 
     Reads data from file and decreases fileSize variable 
*/
uint8_t * readFile(FILE * file, int readBlockSize); 




/*
Input:
    Takes in file pointer for file to be written to, a pointer that points to 
    the data to be added to the file and the dataSize.

Output: 
    Returns nothing; void function    

Process:
    Writes the given data to the given file to the given dataSize
*/
void writeFile(FILE * file, uint8_t * data, int dataSize);




/*
Input:
    Takes in a buffer to place the shortened file name into and a 
    buffer that contains the absolute path of a file.
    Note: If a file contains a '/' in it, the it will only receive
          part of the file name

Output:
    Returns nothing; void

Process:
    Loops through the absolute path and will continually reset the buffer
    if a '/' is found. Once no '/' is  found in the absolute path it will be
    assumed that it has reached the short file name.
*/
void getShortFileName(char * buffer, char * absolutePath);




/*
Input:

Output:

Process:
*/
FILE * getOTP( struct keychain * head, char * MD5, int size );




/*
Input:

Output:

Process:
*/
void freeKeychain(struct keychain * head);




/*
Input:

Output:

Process:
*/
struct keychain * setupkeychain(char *OTPDir);

#endif /* _filemanager_h_ */

