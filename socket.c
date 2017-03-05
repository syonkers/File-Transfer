/*
------------------------------------------- #
# Group Number:       1
# Names:              Clayton Caron
#                     Cody Moorhouse
#		              David James
#                     Marshall Anzinger
#                     Shaun Yonkers
# Group Project:      socket.c
# CMPT 361:           Class Section AS40
# Instructor's Name:  Dr. Nicholas Boers
# ------------------------------------------- #
*/

/* Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#include <pthread.h>

/* Customer Libraries */
#include "socket.h"
#include "globals.h"
#include "packet.h"
#include "usefuloutput.h"

/* Creats a server socket descriptor */
int createServerSD(const char * port) {
    struct addrinfo hints, * iter, * res;
    int errorCode, optionValue, sd;
    
    /* Set up Options */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE | AI_V4MAPPED;
    
    /* Input Options */
    errorCode = getaddrinfo(NULL, port, &hints, &res);
    if (errorCode != 0) {
        fprintf(stderr, "error: %s\n", gai_strerror(errorCode));
        exit(1);
    }
    
    for (iter = res; iter; iter = iter->ai_next) {
        /* Create Socket */
        sd = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
        if (sd == -1) {
            perror("socket");
            continue;
        }
	
        /* Enable Local Address Reuse */
        errorCode = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));
        if (errorCode == -1) {
            perror("setsockopt");
            close(sd);
            sd = -1;
            continue;
        }
	
	/* Bind Socket with Adress & Port */
        errorCode = bind(sd, iter->ai_addr, iter->ai_addrlen);
        if (errorCode == -1) {
            perror("bind");
            close(sd);
            sd = -1;
            continue;
        }
	
        /* Enable Listening */
        errorCode = listen(sd, BACKLOG);
        if (errorCode == -1) {
            perror("listen");
            close(sd);
            sd = -1;
            continue;
        }
	
        /* Socket Created Successfully */
        break;
    }
    
    freeaddrinfo(res);
    if (sd == -1) {
        fprintf(stderr, "unable to create socket for server\n");
        exit(1);
    }

    return sd;
}

/* Creates a client socket descriptor */
int createClientSD(const char * hostname, const char * port) {
    struct addrinfo hints, * iter, * res;
    int errorCode, sd;

    /* Set up Options */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_V4MAPPED;
    
    /* Input Options */
    errorCode = getaddrinfo(hostname, port, &hints, &res);
    
    if (errorCode != 0) {
        fprintf(stderr, "error: %s\n", gai_strerror(errorCode));
        exit(1);
    }
    
    for (iter = res; iter; iter = iter->ai_next) {
        /* Create Socket */
        sd = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
        if (sd == -1) {
            perror("socket");
            continue;
        }
	
        /* Connect to Socket */
        errorCode = connect(sd, iter->ai_addr, iter->ai_addrlen);
        if (errorCode == -1) {
            perror("connect");
            close(sd);
            sd = -1;
            continue;
        }
	
        /* Socket Created Successfully */
        break;
    }

    freeaddrinfo(res);
    if (sd == -1) {
        fprintf(stderr, "unable to connect to server\n");
        exit(1);
    }
    
    return sd;
}

/* Turns a socket descriptor into listening mode, socket must be configured for listening */
void listenOnSocket(const char * port) {
    int servSD = createServerSD(port);
    int client, errCode;

    while (1) {
        if ((client = acceptConnection(servSD)) == -1) {
            continue;
        }

        fprintf(stdout, "A Spy Has Connected...\n");
	
        /* Create Thread for Client Connection */
        pthread_t tid;
        errCode = pthread_create(&tid, NULL, processClient, &client);
        if (errCode != 0) {
            fprintf(stderr, "Error: %d", errCode);
            continue;
        }
    }
}

/* Thread for processing a client when connected */
void * processClient(void * client_desc) {
    client_info * ci = malloc(sizeof(client_info));
    uint8_t packet[PACKET_SZ];
    ssize_t msgBytes;

    bool tFlag = true;
    bool timeout = false;

    ci->sd = *((int*)client_desc);
    ci->fd = NULL;
    strcpy(ci->fileName, UPLOAD_DIR);
    ci->packet = packet;

    while (!timeout) {
	int rdy;
	fd_set rfds;

	FD_ZERO(&rfds);
	FD_SET(ci->sd, &rfds);

	struct timeval sel_timeout;
	sel_timeout.tv_sec = 60; //time out value to disconnect a client

	rdy = select((ci->sd) + 1, &rfds, NULL, NULL, &sel_timeout);
	
	if (rdy == -1) {
	    if (errno == EINTR) {
		continue;
	    }
	    perror("Select");
	    exit(EXIT_FAILURE);
	}
	else if (rdy == 0){
	    printf("Timeout, Client Disconnected\n");
	    timeout = true;
	}
	else{
	    if (FD_ISSET(ci->sd, &rfds)){
		//if we have yet to receive an I packet indicating a file transfer
		if (tFlag == true){
		    msgBytes = recvBytes(ci->sd, ci->packet, PACKET_SZ);
		    if (msgBytes > 0) {
			ci->packSize = (int)msgBytes;
			switch(ci->packet[0]) {
			case 'I':
			    processPacket(ci);
			    tFlag = false;
			    break;
			    
			case 'T':
			    processPacket(ci);
			    break;

			default:
			    createTermPacket(ci->packet, 0x02, NULL, 0);
			    send(ci->sd, ci->packet, sizeof(uint8_t) * 4, 0);
			    break;
			    
			}
		    }
		    continue;
		    
		}
		//while we are recieving a file
		if (tFlag == false) {
		    msgBytes = recvBytes(ci->sd, ci->packet, PACKET_SZ);
		    if (msgBytes > 0) {
			ci->packSize = (int)msgBytes;
			
			// decrypt(*(&ta->packet),getOTP(clientBuf[md5-area-need-buffer-probably]), bufLen-ish);
			if (ci->packet[0] == 'T') {
			    tFlag = true;
			    			    
			}
			processPacket(ci);
			
			//reset buffers to original form for next file and send client ok message to send
			//another file

			if (tFlag == true){
			    createTermPacket(ci->packet, 0x01, NULL, 0);
			    send(ci->sd, ci->packet, sizeof(uint8_t) * 4, 0);
			    memset(ci->packet, 0, sizeof(uint8_t) * PACKET_SZ);
			    memset(ci->fileName, '\0', sizeof(char) * FILE_PATH_SZ);
			    strcpy(ci->fileName, UPLOAD_DIR);
			}
		    }
		}
	    }
	}
    }
    //if client timed out, send a 'T' packet indicating that they are disconnected, return out of thread
    createTermPacket(ci->packet, 0x05, NULL, 0);
    send(ci->sd, ci->packet, sizeof(uint8_t) * 4, 0);
    free(ci);
    return NULL;
}

/* Receives bytes from a connected socket */
ssize_t recvBytes(int sd, uint8_t * buf, size_t bufSz) {
    ssize_t msgBytes = recv(sd, buf, bufSz, 0);

    if (msgBytes == -1) {
        perror("recvBytes - recv");
    }

    return msgBytes;
}

/* Accepts a connection over a socket descriptor */
int acceptConnection(int sd) {
    struct sockaddr_storage clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    int client = accept(sd, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (client == -1) {
        perror("recv - could not accept connection");
        return -1;
    }

    return client;
}

/* Sends a packet to the specified socket descriptor */
ssize_t sendPacket(int sd, uint8_t * packet, size_t packSz) {
    ssize_t bytesSent = send(sd, packet, packSz, 0);
    if (bytesSent == -1) {
	perror("send");
    }
    
    return bytesSent;
}

