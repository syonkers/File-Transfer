/*
# ------------------------------------------- #
# Group Number:       1
# Names:              Clayton Caron
#                     Cody Moorhouse
#		      David James
#                     Marshall Anzinger
#                     Shaun Yonkers
# Group Project:      packet.h
# CMPT 361:           Class Section AS40
# Instructor's Name:  Dr. Nicholas Boers
# ------------------------------------------- #
*/

#ifndef _packet_h_
#define _packet_h_

/* Libraries */
#include <stdint.h>

/* Custom Libraries */
#include "socket.h"


/*
Overview: Server handling for a meta packet
 
Input:
    Client info struct which contains the variables that 
    need to be updated based on the data held within the meta 
    packet

Output:
    No output given, function is void

Process:
    Extracts the data from a meta packet and sets the filename
    to be used in creating nd writing to the file
    Note: timestamp may also be included later
*/
void handleMetaData(client_info * ci);




/* 

Overview: Server handling for a initialization packet 

Input:
    Client info struct which contains the variables that
    need to be updated based on data in Init packet.

Output:
    No output given, function is void

Process:
    Extracts the data from an Initialization packet
*/
void handleInitData(client_info * ci);




/* 
Overview: Create a data packet 

Input:
    Takes in a uint8_t buffer where the final packet is stored, 
    a character buffer which holds the data, and the size of the
    data buffer

Output:
    Void

Process:
     Create a data packet so a server knows what contents are in the
     packet by adding a 'D' character at the start
*/
void createDataPacket(uint8_t * packet, char * buffer, int buffsize);




/* 
Overview: Create a termination packet 

Input:
    Takes in a uint8_t buffer named packet where the final packet to
    be sent is stored, a uint8_t error code, the buffer if the error code
    is 0x00 and the size of the buffer if applicable

Output:
    Void

Process:
    Create a termination packet so a server or client knows if an error occured
    or if the server has received the file successfully
*/
void createTermPacket(uint8_t * packet, uint8_t iserror, char * buffer, 
		     int buffsize);




/* 
Overview: Create a initialization packet 

Input:
    Takes in the MD5 and the number of blocks of 512 bytes that are required
    in the otp from the server and combines them into the packet buffer

Output:
    Void

Process:
    Creates an initialization packet to request a file to be transfered
    to the server by formatting the packet to have the other arguements
*/
void createInitPacket(uint8_t * packet, uint16_t MD5, uint32_t numblocks);




/*
Overview: Create a meta data packet 

Input:
    Takes in the filename, the filesize, and the timestamp of the file wanting
    to be sent to the server

Output:
    Void

Process:
    Creates a meta packet to send the file desciption to the server in order to
    create 
*/
void createMetaPacket(uint8_t * packet, 
		     char * filename,
		     int filesize, 
		     uint32_t timestamp);




/* 
Overview: Process a received after encryption packet 

Input:
    Takes in a packet that has been received from either client or server

Output:
    No output, function is void

Process:
    A packet is received from the client or server and based on the first byte
    which indicates the type of packet, we send the packet to be processed
    by the function required
*/
void processPacket(client_info * ci);

#endif /* _packet_h_ */
