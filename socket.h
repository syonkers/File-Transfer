/*
# ------------------------------------------- #
# Group Number:       1
# Names:              Clayton Caron
#                     Cody Moorhouse
#		      David James
#                     Marshall Anzinger
#                     Shaun Yonkers
# Group Project:      socket.h
# CMPT 361:           Class Section AS40
# Instructor's Name:  Dr. Nicholas Boers
# ------------------------------------------- #
*/

#ifndef _socket_h_
#define _socket_h_

/* Libraries */
#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>

/* Custom Libraries */
#include "globals.h"

/* Thread Argument Struct */
typedef struct _client_info {
    int sd;                       /* Socket Descriptor */
    FILE * fd;                    /* File Descriptor */
    char fileName[FILE_PATH_SZ];  /* Holds the file name */
    uint8_t * packet;             /* Points to a Packet */
    int packSize;                 /* Size of the packet */
} client_info;




/*
Input:
    Socket descriptor that was created by listenOnSocket
 
Output:
    Returns the same as the accept system call. Will return -1 if an error has 
    occurred. Will return a nonnegative integer that is a descriptor for the
    accepted socket on success.

Process: 
    Accepts a connection on a socket and will return the connected descriptor.
*/
int acceptConnection(int sd);




/*
Input:
    Takes a hostname and port and will create a socket descriptor for a client 
    to use.
 
Output:
    Returns a socket descriptor for a client that was connected to a host and
    port. On failure it will return -1 and will report the error.

Process: 
    Creates a socket and will customize that socket to be used with the
    specified host and port. Will report any errors if they occur.
*/
int createClientSD(const char * hostname, const char * port);




/*
Input:
    Takes a port and will create a socket descriptor for a server to use.
 
Output:
    Returns a socket descriptor for a server that was binded to a port and
    has listening mode enabled. On failure it will return -1 and will report
    the error.

Process: 
    Creates a socket and allows for address reused. It will then bind the socket
    to a port and enable listening. Errors will be reported throughout the 
    socket creation
*/
int createServerSD(const char * port);




/*
Input:
    Takes a socket descriptor to receive bytes from and a buffer to fill the
    data received into. It also takes the buffer size to be used by the recv
    system call.
 
Output:
    Will return the number of bytes that were received. If an error occurs
    it will report the error.

Process: 
    Makes a recv system call and will just determine if an error occurs. Will
    fill the buffer specifed with the data received.
*/
ssize_t recvBytes(int sd, uint8_t * buf, size_t bufSz);




/*
Input:
    Takes a port that the server will be listening on.
 
Output:
    Returns nothing; void

Process: 
    Creates a server socket descriptor and will listen for a connection. When a
    connection occurs will print out that "A Spy Has Connected...". Once a
    connection is established it will create a thread to process the client on.
*/
void listenOnSocket(const char * port);




/*
Input:
    Takes in a client socket descriptor that the server will use to communicate
    with the client.
 
Output:
    Returns nothing; void

Process: 
    Acts as a thread for the server to operate on. It Will create a client info 
    struct and will process files that a client will send to it. It will also send
    errrs to the client if any occur.
*/
void *  processClient(void * arg);




/*
Input:
    Takes in a socket descriptor and a packet with its size.
 
Output:
    Returns -1 if the send failed, otherwise it will return the number of bytes
    that was send over the socket descriptor.

Process: 
    Makes a call to the send system call and will report an error if one occurs.
*/
ssize_t sendPacket(int sd, uint8_t * packet, size_t packSz);

#endif /* _socket_h_ */
