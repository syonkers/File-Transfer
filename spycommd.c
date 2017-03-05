/*
# ------------------------------------------- #
# Group Number:       1
# Names:              Clayton Caron
#                     Cody Moorhouse
#		              David James
#                     Marshall Anzinger
#                     Shaun Yonkers
# Group Project:      spycommd.c
# CMPT 361:           Class Section AS40
# Instructor's Name:  Dr. Nicholas Boers
# ------------------------------------------- #
*/

/* Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <unistd.h>
#include <string.h>

/* Custom Libaries */
#include "spycommd.h"
#include "usefuloutput.h"
#include "socket.h"

/* Runs a server for clients/spies to connect to in order to transfer files */
int main(int argc, char * argv[]) {
    char * port = DEFAULT_PORT;
    uint8_t packet[PACKET_SZ];
    memset(packet, 0, PACKET_SZ);

    int ch;
    opterr = 0;
    
    while ((ch = getopt(argc, argv, "hp:")) != -1) {
	switch (ch) {
	    
	    case 'p':
		port = optarg;
		break;

            case 'h':
                print_usage();
                exit(0);
                
   	    case '?':
		if (optopt == 'p') {
		    fprintf(stderr, 
			    "Option -%c requires port\n", 
			    optopt);
                    exit(EXIT_FAILURE);
                } else{
                    fprintf(stderr, "Unknown option -%c\n", optopt);
                    print_usage();
                    exit(EXIT_FAILURE);
                }   
        }
    }

    listenOnSocket(port);
    
    return 0;
}

/* Prints out a usage message */
void print_usage(void) {
    printf("usage: spycomm [-h] [-p port]\n");
}
