/*
  ------------------------------------------- #
  # Group Number:       1
  # Names:              Clayton Caron
  #                     Cody Moorhouse
  #		        David James
  #                     Marshall Anzinger
  #                     Shaun Yonkers
  # Group Project:      packet.c
  # CMPT 361:           Class Section AS40
  # Instructor's Name:  Dr. Nicholas Boers
  # ------------------------------------------- #
*/

/* Libraries */
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>

/* Custom Libraries */
#include "packet.h"
#include "globals.h"
#include "socket.h"
#include "filemanager.h"
#include "usefuloutput.h"

/* Handle the meta data to create a file */
void handleMetaData(client_info * ci) {
    struct stat info;
    char filename[FILE_PATH_SZ];
    char tmpBuf[FILE_PATH_SZ];
    int tmpidx;
    int fileidx = 0;
    int count = 1;
    int packetidx = 1;
    
    //get the filename from the meta packet
    while (ci->packet[packetidx] != '\0') {  
	filename[fileidx++] = ci->packet[packetidx++];	
    }
    
    filename[fileidx++] = '\0';
        
    strcat(ci->fileName, filename);
    strcpy(tmpBuf, ci->fileName);

    //if the file already exists
    while (stat(tmpBuf, &info) == 0) {
	memset(tmpBuf, 0, FILE_PATH_SZ);
	tmpidx = strlen(ci->fileName);
	strcpy(tmpBuf, ci->fileName);
	tmpBuf[tmpidx++] = '_';
	tmpBuf[tmpidx++] = (char)(count + 48);
	tmpBuf[tmpidx++] = '\0';
	count++;
    }
    
    strcpy(ci->fileName, tmpBuf);
    ci->fd = fopen(ci->fileName,"w+b");
    if(ci->fd == NULL){
	perror("Creating file: ");
	exit(EXIT_FAILURE);
    }
    
    //    closeFile(ci->fd);
    log_start(ci->fileName);

}

/* Handle the initialization data to see if we can upload file */
void handleInitData(client_info * ci) {
    
    /* Check OTP if there is enough room */
    /* If accepted send back an A */

}

/* Create a data packet to be sent out */
void createDataPacket(uint8_t * packet, char * buffer, int buffsize) {
    int idx = 1;

    packet[0] = 'D';
    for (int i = 0; i <= buffsize; i++) {
	packet[idx++] = buffer[i];
    }
}

/* Create the termination packet */
void createTermPacket(uint8_t * packet, uint8_t iserror,
		      char * buffer, int buffsize) {

    int idx = 0;

    packet[idx++] = 'T';
    packet[idx++] = iserror;
    if (buffer == NULL) {
	
    }
    else {
	for (int i = 0; i <= buffsize; i++) {
	    packet[idx++] = buffer[i];
	}
    }
}

/* Create the initialization packet */
void createInitPacket(uint8_t * packet, uint16_t MD5, uint32_t numblocks) {

    packet[0] = 'I';
    packet[1] = MD5 >> 8;
    packet[2] = MD5 & 0xff;
    packet[3] = numblocks >> 24;
    packet[4] = numblocks >> 16;
    packet[5] = numblocks >> 8;
    packet[6] = numblocks & 0xff;

}

/* Create a 'M'eta packet */
void createMetaPacket(uint8_t * packet, char * filename,
		      int filesize, uint32_t timestamp) {

    int idx = 0;

    packet[idx++] = 'M';

    for (int i = 0; i <= filesize; i++) {
	packet[idx++] = filename[i];
    }

    packet[idx++] = '\0';
    packet[idx++] = timestamp >> 24;
    packet[idx++] = timestamp >> 16;
    packet[idx++] = timestamp >> 8;
    packet[idx++] = timestamp & 0xff;

}

void errorCheck(client_info * ci){
    switch(ci->packet[1]){
    case 0xFF:
	//client disconnected, close thread
	printf("A Spy Has Disconnected.\n");
	pthread_exit(0);
	break;

    default:
	fprintf(stderr, "unspecified error from client\n");
	pthread_exit(0);
	break;
    }
}

/* After a packet is received, processPacket will interpret the contents */
void processPacket(client_info * ci) {
    int i;
    uint8_t type;
    uint8_t data[BLOCK_SZ];
    memset(data, 0, BLOCK_SZ + 1);

    type = ci->packet[0];

    switch (type) {
    case 'D':
	for (i = 0; i < BLOCK_SZ; i++) {
	    data[i] = ci->packet[i + 1];
	}
	writeFile(ci->fd, data, BLOCK_SZ);
	break;
	    
    case 'M':
	handleMetaData(ci);
	break;
	
    case 'I':
	handleInitData(ci);
	break;
	
    case 'T':
	//if an error packet
	if (ci->packet[1] != 0x00) {
	    errorCheck(ci);
	    log_fail(ci->fileName);
	}
	//else, its the last packet and so we write to the file
	else {
	    uint8_t remainder[ci->packSize];
	    memset(remainder, 0, sizeof(remainder) + 1);
	    for (i = 0; i < ci->packSize; i++) {
		remainder[i] = ci->packet[i + 2];
	    }
	    writeFile(ci->fd, remainder, ci->packSize - 2);
	    printf("File transfer complete\n");
	    log_success(ci->fileName);
	}
	break;
    }
}
