/*
# ------------------------------------------- #
# Group Number:       1
# Names:              Clayton Caron
#                     Cody Moorhouse
#		              David James
#                     Marshall Anzinger
#                     Shaun Yonkers
# Group Project:      spycomm.h
# CMPT 361:           Class Section AS40
# Instructor's Name:  Dr. Nicholas Boers
# ------------------------------------------- #
*/

#ifndef _spycomm_h_
#define _spycomm_h_

#include "globals.h"


/* 
Overview: Retrieve filenames in current directory and fill the Files struct.

Input:
    Void

Output:
    Void
 
*/
void get_file_list(void);

/* 
Overview: Update a given files progress with its current completion percentage

Input:
    - The filename of the file currently being sent in the thread 
    - The current percent complete

Output:
    Void

Process:
     Takes in the filename and goes through the transfer list and updates
     the percentage complete in the transfers struct
 
*/
void update_progress(char * filename, int percent_complete);


/*
Overview: Update a given files transfer speed

Input:
    - The filename of the file currently being sent in the thread 
    - The current transfer speed
    
Output:
    Void

Process:
     Takes in the filename and goes through the transfer list and updates
     the transfer speed in the transfers struct

*/
void update_speed(char * filename, int speed);

/*
Overview: Set up the initial ncurses options and screen

Input:
    Void
    
Output:
    Void

*/
void setup_ncurses(void);

/*
Overview: Print usage message for -h command

Input:
    Void
    
Output:
    Void

*/
void print_usage(void);

/*
Overview: Thread function used to send file to the connected server

Input:
    Takes in the sending_info struct with all the info about the file wanted
    to be sent

Output:
    Void, closes thread at the end

Process:
    When a new file is being sent to the server this function will be called with the sending info
    struct. The function will then send in order, an 'I' packet, a 'M' packet, chunks of 512 bytes 
    of the file and lastly, a 'T' packet indicating the end. Client UI info is also updated as the 
    file upload progresses.

*/
void * sendFile(void * ta_sending);

/*
Overview: Control between user keyboard input and incoming packets

Input: 
    - Filenames
    - Number of filenames
    - Flag determining whether or not tab has been pressed 
      (1 for transfer list highlighting, 0 for file explorer highlighting)
    - Page number for managing the file explorer
    - Currently selected file in file explorer
    - Page number for managing the transfer list
    - Currently selected file in  transfer list
    - Thread to send a file
    - Socket descriptor
    
Output:
    Returns filenames of the current page

Process:
    Gets and handles user input, updating variables needed to track selections.
        -Enter:      Tries to send the selected file, adding it to transfers.
                     If the file has already been chosen, an audible warning is given.
        -q:          Quits the program
        -Up Arrow:   Move the selection up one file
        -Down Arrow: Move the selection down one file
        -Tab:        Move the selection between the transfer list and file explorer

*/
char ** processInput(char ** Filenames, 
		     int num_files, 
		     int * tab,
		     int * page, 
		     int * selected_file, 
		     int * t_page, 
		     int * t_selected_file, 
		     pthread_t send_file_thread, 
		     int sd);


/*
Overview: Return whether the selected file is in the process of being transferred

Input:
    Selected file
    
Output:
    True or false

*/
int is_tranferring(char * filename);


/*
Overview: Get the index of the selected filename within the Transfers struct

Input:
    Selected file
    
Output:
    True or false

*/
int get_index(char * filename);


/*
Overview: Called when a packet is received from the server. 
          Typically an error has occured when a packet is received.

Input:
    Socket descriptor
    
Output:
    Void

*/
void processServerPacket(int sd);


/* Structures */

/* struct used for file management using ncurses */
struct Transfers {
    char ** transfer_list;
    int num_transfers;
    int * percent_complete;
    int * speed;
    int sent;
};

/* struct used to pass informtion into thread that is used in sendFile */
typedef struct _sending_info {
    int sd;
    int index;
} sending_info;

#endif /* _spycomm_h_ */
