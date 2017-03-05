/*
# ------------------------------------------- #
# Group Number:       1
# Names:              Clayton Caron
#                     Cody Moorhouse
#		     		  David James
#                     Marshall Anzinger
#                     Shaun Yonkers
# Group Project:      usefuloutput.h
# CMPT 361:           Class Section AS40
# Instructor's Name:  Dr. Nicholas Boers
# ------------------------------------------- #
*/

#ifndef _usefuloutput_h_
#define _usefuloutput_h_

/* Get current date/time */
void time_stamp(char * stamp);

/* Record file transfer information in a log file */
void log_start(char * filename);
void log_fail(char * filename);
void log_success(char * filename);

/* Print files with their completion percentages and progress bar */
void print_transfer_list(char **file_list,
						 int num_files,
	                     int *percent_complete,
	                     int *speed, 
	                     int selected_file,
	                     int page,
	                     int max_lines,
	                     int tab);

/* Print files in current directory, return the number of files */
int print_dir_contents(char **filenames, 
					   int selected_file, 
					   int page, 
					   int max_lines,
					   int tab);

#endif /* _usefuloutput_h_ */
