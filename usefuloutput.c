/*
  # ------------------------------------------- #
  # Group Number:	      1
  # Names:	      Clayton Caron
  #		      Cody Moorhouse
  #		      David James
  #		      Marshall Anzinger
  #		      Shaun Yonkers
  # Group Project:      usefuloutput.c
  # CMPT 361:	      Class Section AS40
  # Instructor's Name:  Dr. Nicholas Boers
  # ------------------------------------------- #
*/

/* Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include <time.h>

/* Custom Libraries */
#include "usefuloutput.h"
#include "globals.h"
#include "filemanager.h"

/* Static Variables */
static FILE * logfp;    /* log file pointer */

/* Server functions */
void time_stamp(char * stamp) {
    time_t cur_time = time(NULL);
    struct tm *tm = localtime(&cur_time);
    
    strftime(stamp, 29, "%a %b %e %T %Z %Y", tm);
}

void log_start(char * filename) {
    char stamp[29];
    time_stamp(stamp);
    logfp = fopen(LOG_PATH, "a");
    fprintf(logfp, "[%s] File transfer started: %s\n", stamp, filename);
    fflush(logfp);
    fclose(logfp);
}

void log_fail(char * filename) {
    char stamp[29];
    time_stamp(stamp);
    logfp = fopen(LOG_PATH, "a");
    fprintf(logfp, "[%s] File transfer failed: %s\n", stamp, filename);
    fflush(logfp);
    fclose(logfp);
}

void log_success(char * filename) {
    char stamp[29];
    time_stamp(stamp);
    logfp = fopen(LOG_PATH, "a");
    fprintf(logfp, "[%s] File transfer successful: %s\n", stamp, filename);
    fflush(logfp);
    fclose(logfp);
}

/* Client functions */

void print_transfer_list(char ** file_list,
			 int num_files,
			 int * percent_complete,
			 int * speed,
			 int selected_file,
			 int page,
			 int max_lines,
			 int tab) {

    char shortName[FILE_PATH_SZ];

    attron(COLOR_PAIR(1));

    int i;
    for(i = 0; i < max_lines; i++) {
	/* print separator line */
	mvwaddch(stdscr, i, T_LEN, '|');
    }

    int lines = max_lines;
    if ((page * max_lines + max_lines) > num_files)
	lines = num_files % max_lines;

    for (i = 0; i < lines; i++) {

	/* print progress bar */
	attron(COLOR_PAIR(1));
	mvwaddch(stdscr, i, T_LEN + 21, '{');
	mvwaddch(stdscr, i, BAR_LEN + T_LEN + 22, '}');
	int j;
	for (j = 0; j < BAR_LEN; j++) {
	    mvwaddch(stdscr, i, j + T_LEN + 22, '-');
	}
	if(percent_complete[max_lines * page + i] != 100) {
	    attron(COLOR_PAIR(2));
	    for (j = 0; j < percent_complete[max_lines * page + i] / (100.0 / BAR_LEN); j++) {
		mvwaddch(stdscr, i, j + T_LEN + 22, '-');
	    }
	}

	/* print percent complete */
	if (percent_complete[max_lines * page + i] == 0)
	    attron(COLOR_PAIR(4));
	else if (percent_complete[max_lines * page + i] == 100)
	    attron(COLOR_PAIR(3));
	else
	    attron(COLOR_PAIR(5));
	mvwprintw(stdscr, i, T_LEN + 2, "[%i%%]", percent_complete[max_lines * page + i]);

	/* print transfer speed */
	attron(COLOR_PAIR(1));
	mvwprintw(stdscr, i, T_LEN + 10, "[%iKB/s]", speed[max_lines * page + i]);

	/* print filenames */
	if (max_lines * page + i == selected_file && tab)
	    attron(COLOR_PAIR(2));
	else
	    attron(COLOR_PAIR(1));
	getShortFileName(shortName, file_list[max_lines * page + i]);
	mvwprintw(stdscr, i, BAR_LEN + T_LEN + 25, "%s", shortName);
    }
}

int print_dir_contents(char ** filenames, 
		       int selected_file, 
		       int page, 
		       int max_lines, 
		       int tab) {
    int i;
    for (i = 0; i < max_lines; i++) {
		if(filenames[max_lines * page + i] == NULL)
		    break;
		/* print filenames */
		if (max_lines * page + i == selected_file 
		    && fileTest(filenames[max_lines * page + i]) == 2
		    && !tab)
		    attron(COLOR_PAIR(7));
		else if (fileTest(filenames[max_lines * page + i]) == 2)
		    attron(COLOR_PAIR(6));
		else if (max_lines * page + i == selected_file && !tab)
		    attron(COLOR_PAIR(2));
		else
		    attron(COLOR_PAIR(1));
		mvwprintw(stdscr, i, 0, "%s", filenames[max_lines * page + i]);
    }
	
    i = 0;
    while(filenames[i] != NULL) {
	i++;
    }
    return i - 1;
}
