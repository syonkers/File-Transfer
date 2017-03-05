/*
------------------------------------------- #
# Group Number:       1
# Names:              Clayton Caron
#                     Cody Moorhouse
#		      David James
#                     Marshall Anzinger
#                     Shaun Yonkers
# Group Project:      testsocket.c
# CMPT 361:           Class Section AS40
# Instructor's Name:  Dr. Nicholas Boers
# ------------------------------------------- #
*/

/* Libraries */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <pthread.h>

#include <sys/socket.h>

/* Custom Libraries */
#include "socket.h"
#include "globals.h"
#include "packet.h"
#include "filemanager.h"
#include "usefuloutput.h"

/* Thread Arguments */
typedef struct _test_client_info {
  int sd;
} test_client_info;

/* Thread for Server to Listen within the same executable */
void * testServerListen(void * arg) {
    test_client_info * ta = (test_client_info *) arg;
    
    size_t clientBufSz = TEST_BUFSZ;
    uint8_t clientBuf[clientBufSz];
    ssize_t msgBytes;
    int client = -1;
    
    while(1) {
	/* Wait for Connetion */
	if ((client = acceptConnection(*(&ta->sd))) == -1)
	    continue;
	
	/* Receive from Client */
	msgBytes = recvBytes(client, clientBuf, clientBufSz);
	
	if (msgBytes > 0) {
	    printf("Received on Server:\t%s\n", clientBuf);
	    
	    /* Send to Client */
	    printf("Sending from Server:\t%s\n", TEST_TO_CLIENT_MSG);
	    if (send(client, TEST_TO_CLIENT_MSG, strlen(TEST_TO_CLIENT_MSG), 0) == -1) {
		perror("send - server");
	    }
	    break;
	}
    }
    return NULL;
}

/* Tests sockets by creating and sending/receiving in both directions */
int main() {
    /* Testing createServerSD */
    printf("Create ServerSD:");
    int servSD = createServerSD("4444");
    if (servSD == -1) {
	printf("\tFAIL\n");
	exit(1);
    } else {
    printf("\tPASS\n");
    }
  
    /* Testing createClientSD */
    printf("Create ClientSD:");
    int clientSD = createClientSD("localhost", "4444");
    if (clientSD == -1) {
	printf("\tFAIL\n");
	exit(1);
    } else {
	printf("\tPASS\n");
    }

    /* Testing Sockets */
    pthread_t tid;
    thread_args ta = {servSD};
    printf("Server Listening:");
    if (pthread_create(&tid, NULL, testServerListen, &ta) != 0) {
	printf("\tFAIL\n");
	exit(1);
    } else {
	printf("\tPASS\n");
    }   
    
    /* Sending to Server From Client */
    printf("Sending from Client:\t%s\n", TEST_TO_SERVER_MSG);
    if (send(clientSD, TEST_TO_SERVER_MSG, strlen(TEST_TO_SERVER_MSG), 0) == -1) {
	perror("send - client");
	exit(1);
    };
    
    /* Delay - Allow Server to Process Response */
    useconds_t delay = 50;
    usleep(delay);
    
    /* Receiving on CLient from Server */
    size_t serverBufSz = TEST_BUFSZ;
    char serverBuffer[serverBufSz];
    if (recv(clientSD, serverBuffer, serverBufSz, 0) == -1) {
	perror("recv - client");
	exit(1);
    };
    printf("Recveived on Client:\t%s\n", serverBuffer);
    
    close(servSD);
    close(clientSD);
    pthread_detach(tid);
    
    return 0;
}

