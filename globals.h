/*
------------------------------------------- #
# Group Number:       1
# Names:              Clayton Caron
#                     Cody Moorhouse
#		      David James
#                     Marshall Anzinger
#                     Shaun Yonkers
# Group Project:      globals.h
# CMPT 361:           Class Section AS40
# Instructor's Name:  Dr. Nicholas Boers
# ------------------------------------------- #
*/

#ifndef _globals_h_
#define _globals_h_

/* Macros */
#define DEFAULT_HOST            "localhost"
#define DEFAULT_PORT            "22222"

#define HOST_BF_SZ              256
#define PORT_BF_SZ              6

#define BACKLOG                 10              /* Maximum connections to allow on server */

#define BLOCK_SZ                512             /* In bytes */
#define PACKET_SZ               (BLOCK_SZ + 1)  /* In bytes */
#define FILE_PATH_SZ            256             /* In bytes */
#define HASH_SZ                 32

#define PROGRESS_RR             200000          /* Sets refresh rate for progress bar (in microseconds) */
#define MAX_LINES               24
#define BAR_LEN                 18              /* Set progress bar length */
#define T_LEN                   22              /* Set x coord of transfer output */

#define LOG_PATH                "./spycommd.log" /* Sets the path for log files */
#define OTP_TABLE_FILE          "md5-to-OTP.txt"
#define UPLOAD_DIR              "./Uploads/\0"
#define DELIM                   "-"
#define OTP_DIR                 "./otp"

/* Test Macros */
#define TEST_BUFSZ              80
#define TEST_TO_SERVER_MSG      "Testing: 1 - 2 -3"
#define TEST_TO_CLIENT_MSG      "ROGER THAT"

#define TEST_MAX_BLOCK_SZ       10

#endif /* _globals_h_*/
