/*
  ------------------------------------------- #
  # Group Number:       1
  # Names:              Clayton Caron
  #                     Cody Moorhouse
  #                     David James
  #                     Marshall Anzinger
  #                     Shaun Yonkers
  # Group Project:      spycomm.c
  # CMPT 361:           Class Section AS40
  # Instructor's Name:  Dr. Nicholas Boers
  # ------------------------------------------- #
*/

/* Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

#include <curses.h>

#include <dirent.h>
#include <string.h>
#include <strings.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>

#include <time.h>
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>

/* Custom Libraries */
#include "spycomm.h"
#include "socket.h"
#include "globals.h"
#include "filemanager.h"
#include "packet.h"
#include "usefuloutput.h"

/* Static Variables */
static struct Transfers f;
static int sending = 0;
static bool serverReady = true;

int main(int argc, char * argv[]) {
    char port[PORT_BF_SZ] = DEFAULT_PORT;
    char hostname[HOST_BF_SZ] = DEFAULT_HOST;
    hostname[strlen(DEFAULT_HOST)] = '\0';

    int ch;
    int sd;
    char * pch;
    opterr = 0;
    
    while ((ch = getopt(argc, argv, "hc:")) != -1) {
	switch (ch) {
	    
	case 'c':
	    pch = strtok(optarg, ":");
		
	    if(pch != NULL) {
		strcpy(hostname, pch);
		pch = strtok (NULL, "\n");
	    }
	    
	    if(pch != NULL) {
		strcpy(port, pch);
		strcpy(port, pch);
	    }
	    break;
		 
	case 'h':
	    print_usage();
	    exit(0);
	    
	case '?':
	    if (optopt == 'c') {
		fprintf(stderr, 
			"Option -%c requires hostname, port optional\n", 
			optopt);
		exit(EXIT_FAILURE);
	    } else{
		fprintf(stderr, "Unknown option -%c\n", optopt);
		print_usage();
		exit(EXIT_FAILURE);
	    }	
	}
    }
    
    sd = createClientSD(hostname, port);
    
    setup_ncurses();
    
    f.transfer_list = NULL;
    f.num_transfers = 0;
    f.percent_complete = NULL;
    f.speed = NULL;
    f.sent = 0;

    int selected_file = 0;
    int page = 0;
    int t_selected_file = 0;
    int t_page = 0;
    int tab = 0;

    char ** filenames = getFileNames();

    pthread_t send_file_thread = NULL;
    
    while (1) {
	int rdy, num_files;
	fd_set rfds;
		
	FD_ZERO(&rfds);
	FD_SET(sd, &rfds);
	FD_SET(0, &rfds);

	struct timeval sel_timeout;
    	sel_timeout.tv_sec = 0;
    	sel_timeout.tv_usec = PROGRESS_RR;

	werase(stdscr);

	print_transfer_list(f.transfer_list,
			    f.num_transfers,
			    f.percent_complete,
			    f.speed,
			    t_selected_file,
			    t_page,
			    MAX_LINES,
			    tab);
	
	num_files = print_dir_contents(filenames,
				       selected_file, 
				       page, 
				       MAX_LINES,
				       tab);
	wrefresh(stdscr);
	
	rdy = select(sd + 1, &rfds, NULL, NULL, &sel_timeout);
		
	if (rdy == -1) {
	    if (errno == EINTR) {
		continue;
	    }
	    endwin();
	    perror("Select");
	    exit(EXIT_FAILURE);
	}
	else {
	    if (FD_ISSET(sd, &rfds)) {
	   	processServerPacket(sd);
	    }
		    
	    if (FD_ISSET(0, &rfds)) {
		filenames = processInput(filenames,
					 num_files, 
					 &tab,
					 &page, 
					 &selected_file, 
					 &t_page,
					 &t_selected_file,
					 send_file_thread, 
					 sd);
	    }
	}
    }
    endwin();
    return 0;
}

void processServerPacket(int sd) {
    uint8_t packet[PACKET_SZ];
    ssize_t msgbytes;
    
    msgbytes = recvBytes(sd, packet, PACKET_SZ);
    if (msgbytes > 0) {
	if (packet[0] == 'T') {
	    switch(packet[1]) {
		
	    case 0x01:
		serverReady = true;
		break;

	    case 0x02:
		fprintf(stderr, "Packet read error, please try again\n");
		break;

	    case 0x03:
		endwin();
		fprintf(stderr, "Authentication failed\n");
		exit(-1);
		break;

	    case 0x04:
		endwin();
		fprintf(stderr, "Not enough room in one time pad\n");
		exit(-1);
		break;

	    case 0x05:
		endwin();
		printf("Timeout, please reconnect again\n");
		exit(0);
		break;

	    default:
		endwin();
		fprintf(stderr, "Unspecified error\n");
		exit(-1);
	    }
	}
    }   
}

void append_transfer_list(char *filename) {
    /* allocate memory for transfer_list */
    f.transfer_list = realloc(f.transfer_list, 
			      (f.num_transfers+1)*sizeof(char*));
    if (f.transfer_list == NULL) {
	endwin();
	perror("realloc()");
	exit(EXIT_FAILURE);
    }

    /* allocate memory for transfer_list[i] */
    f.transfer_list[f.num_transfers] = malloc(strlen(filename)+1);
    if (f.transfer_list[f.num_transfers] == NULL) {
	endwin();
	perror("malloc()");
	exit(EXIT_FAILURE);
    }
    strcpy(f.transfer_list[f.num_transfers], filename);

    /* allocate memory for percent_complete */
    f.percent_complete = realloc(f.percent_complete, 
    	                         (f.num_transfers + 1) * sizeof(int));
    if (f.percent_complete == NULL) {
    	endwin();
    	perror("realloc()");
    	exit(EXIT_FAILURE);
    }
    f.percent_complete[f.num_transfers] = 0;

    /* allocate memory for speed */
    f.speed = realloc(f.speed, 
		      (f.num_transfers+1) * sizeof(int));
    if (f.speed == NULL) {
    	endwin();
    	perror("realloc()");
    	exit(EXIT_FAILURE);
    }
    f.speed[f.num_transfers] = 0;

    f.num_transfers++;
}

void update_progress(char * filename, int percent_complete) {
    char shortName[FILE_PATH_SZ];
    int i;
    for (i = 0; i < f.num_transfers; i++) {
    	getShortFileName(shortName, f.transfer_list[i]);
	if (strcmp(filename, shortName) == 0) {
	    f.percent_complete[i] = percent_complete;
	    break;
	}
    }
}

void update_speed(char * filename, int speed) {
    char shortName[FILE_PATH_SZ];
    int i;
    for (i = 0; i < f.num_transfers; i++) {
    	getShortFileName(shortName, f.transfer_list[i]);
	if (strcmp(filename, shortName) == 0) {
	    f.speed[i] = speed;
	    break;
	}
    }
}

int get_index(char * filename) {
    char shortName[FILE_PATH_SZ];
    int i;
    for (i = 0; i < f.num_transfers; i++) {
    	getShortFileName(shortName, f.transfer_list[i]);
	if (strcmp(filename, shortName) == 0) {
	    return i;
	}
    }
    return -1;
}

int is_tranferring(char * filename) {
    char shortName[FILE_PATH_SZ];
    int i;
    for (i = 0; i < f.num_transfers; i++) {
    	getShortFileName(shortName, f.transfer_list[i]);
	if (strcmp(filename, shortName) == 0)
	    return 1;
    }
    return 0;
}

void setup_ncurses(void) {   
    initscr();
    cbreak();
    noecho();
    wtimeout(stdscr, PROGRESS_RR);
    keypad(stdscr, 1);
    curs_set(0);
    
    start_color();
    
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_CYAN);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(6, COLOR_BLUE, COLOR_BLACK);
    init_pair(7, COLOR_BLUE, COLOR_CYAN);
    
}

void print_usage(void) {
    printf("usage: spycomm [-h] [-c hostname:port]\n");
}

void * sendFile(void * arg) {
    sending_info * si = (sending_info *) arg;
    
    while (f.sent < f.num_transfers) {
	if (serverReady) {
	    struct timespec time_1;
	    struct timespec time_2;
	    
	    uint8_t packet[PACKET_SZ];
	    char shortName[FILE_PATH_SZ];
	    char file_data[BLOCK_SZ];
	    char full_path[FILE_PATH_SZ];
	    size_t nbytes;
	    (void)nbytes;
	    
	    int rem_size, total_size, percent_completed = 0;
	    float time_diff;
	    
	    strcpy(full_path, f.transfer_list[si->index]);
	    FILE * fp = fopen(full_path, "rb");
		    
	    /* Get file size */
	    if (fseek(fp, 0L, SEEK_END) == -1) {
		endwin();
		perror("fseek()");
		exit(EXIT_FAILURE);
	    }
	    rem_size = ftell(fp);
	    total_size = rem_size;
	    if (fseek(fp, 0L, SEEK_SET) == -1) {
		endwin();
		perror("fseek()");
		exit(EXIT_FAILURE);
	    }
	    
	    /* Get the short filename to send to the server */
	    getShortFileName(shortName, full_path);
	    
	    /* Send an initialization packet to verify otp */
	    memset(packet, 0, sizeof(uint8_t) * PACKET_SZ);
	    createInitPacket(packet, 0x0000, (rem_size / BLOCK_SZ));
	    sendPacket(si->sd, packet, PACKET_SZ);  
	    
	    /* Send a meta packet with filename */
	    memset(packet, 0, sizeof(uint8_t) * PACKET_SZ);
	    createMetaPacket(packet, shortName, strlen(shortName), 0x00000000);
	    sendPacket(si->sd, packet, PACKET_SZ);
	    
	    if (fp == NULL) {
		endwin();
		fprintf(stderr, "file open error\n");
		exit(EXIT_FAILURE);
	    }
	    
	    /* While the packet can be filled with data */
	    while (rem_size > BLOCK_SZ) {
		memset(file_data, 0, sizeof(char) * BLOCK_SZ);
		memset(packet, 0, sizeof(uint8_t) * (PACKET_SZ));
		
		nbytes = fread(file_data, sizeof(char), BLOCK_SZ, fp);
		if (nbytes != BLOCK_SZ) {
		    endwin();
		    fprintf(stderr, "fread() error\n");
		    exit(EXIT_FAILURE);
		}
		
		createDataPacket(packet, file_data, BLOCK_SZ);
		
		clock_gettime(CLOCK_REALTIME, &time_1);
		sendPacket(si->sd, packet, PACKET_SZ);
		clock_gettime(CLOCK_REALTIME, &time_2);
		time_diff = (float)(time_2.tv_nsec - time_1.tv_nsec) / 1.0e9;
		
		/* Converts Speed */
		float time_speed = ((((float)nbytes / time_diff) / 1024 / 1024));
		
		percent_completed = ((((float)total_size - (float)rem_size) / (float)total_size) * 100);
		update_progress(shortName, percent_completed);
		update_speed(shortName, time_speed);
			
		rem_size -= BLOCK_SZ;
	    }
	    
	    /* Creates termination with leftover content */
	    char rem_data[rem_size];
	    uint8_t t_packet[rem_size + 2];
	    memset(rem_data, 0, (sizeof(char) * rem_size));
	    memset(t_packet, 0, (sizeof(uint8_t) * (rem_size + 2)));
		    
	    nbytes = fread(rem_data, sizeof(char), rem_size, fp);
	    if (nbytes != rem_size) {
		endwin();
		fprintf(stderr, "fread() error\n");
		exit(EXIT_FAILURE);
	    }
	    
	    createTermPacket(t_packet, 0x00, rem_data, rem_size);
	    sendPacket(si->sd, t_packet, sizeof(t_packet));
	    
	    serverReady = false;
	    
	    update_speed(shortName, 0);
	    update_progress(shortName, 100);
	    
	    f.sent++;
	    si->index++;  
	}
    }
    free(arg);
    sending = 0;
    return NULL;
}

char ** processInput(char ** filenames, 
		     int num_files, 
		     int * tab, 
		     int * page, 
		     int * selected_file, 
		     int * t_page,
		     int * t_selected_file,
		     pthread_t send_file_thread, 
		     int sd) {
    
    int key = wgetch(stdscr);

    switch (key) {
	
    case KEY_UP:
	if (!(*tab)) {
	    if ((*selected_file) > 0) {
		if ((*selected_file) == (*page)*MAX_LINES)
		    (*page)--;
		(*selected_file)--;
	    }
	} else {
	    if ((*t_selected_file) > 0) {
		if ((*t_selected_file) == (*t_page)*MAX_LINES)
		    (*t_page)--;
		(*t_selected_file)--;
	    }
	}
	break;
	
    case KEY_DOWN:
	if (!(*tab)) {
	    if ((*selected_file) < num_files) {
		if ((*selected_file) == (*page)*MAX_LINES + MAX_LINES-1)
		    (*page)++;
		(*selected_file)++;
	    }
	} else {
	    if ((*t_selected_file) < f.num_transfers-1) {
		if ((*t_selected_file) == (*t_page)*MAX_LINES + MAX_LINES-1)
		    (*t_page)++;
		(*t_selected_file)++;
	    }
	}
	break;
	
    case '\t':
	/* switch to other window */
	if (!(*tab))
	    (*tab) = 1;
	else
	    (*tab) = 0;

	break;
	
    case '\n':
	if (!(*tab)) {
	    if (fileTest(filenames[(*selected_file)]) == 2) {
		changeDir(filenames[(*selected_file)]);
		filenames = getFileNames();
		(*selected_file) = 0;
		(*page) = 0;
		
	    } else if (!is_tranferring(filenames[(*selected_file)])) {
	    	char full_path[FILE_PATH_SZ];
	    	realpath(filenames[(*selected_file)], full_path);
		append_transfer_list(full_path);
		
		/* create thread to send file */
		sending_info * si = malloc(sizeof(sending_info));
		si->sd = sd;
		si->index = get_index(filenames[(*selected_file)]);
		
		if (!sending) {
		    sending = 1;
		    pthread_create(&send_file_thread, NULL, sendFile, si);
		}
	    } else {
		beep();
	    }
	}
	break;
	
    case 'q': ;
	uint8_t termPacket[4];
	createTermPacket(termPacket, 0xFF, NULL, 0);
	send(sd, termPacket, sizeof(uint8_t) * 4, 0);
	endwin();
	exit(0);	    
    }
    
    return filenames;
}
