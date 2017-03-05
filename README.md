# ----------------------------------------------------------- #
# Group Number:        	1
# Names:               	Clayton Caron
#                       Cody Moorhouse
#			David James
#			Marshall Anzinger
#			Shaun Yonkers
# Date:                 December 5th, 2015
# CMPT 361:           	Class Section AS40
# Instructors Name:   	Dr. Nicholas Boers
# ----------------------------------------------------------- #

# ------------------------------------------------------------------------- #
# 			* IMPORTANT *					    #
# ------------------------------------------------------------------------- #

(1) We were unable to complete the encryption portion of the applications due
to time constraints and problematic code.

# ------------------------------------------------------------------------- #
#			INTRODUCTION       			 	    #
# ------------------------------------------------------------------------- #

Spycommd and spycomm are intended to provide a secure encrypted channel for
secret file transfers. Spycommd acts as headquarters (HQ -- a listening server)
and will be in a listening state waiting for fellow spies to connect. Spycomm
is the program a spy would use to send files to HQ.

# ------------------------------------------------------------------------- #
#			  USAGE				             	    #
# ------------------------------------------------------------------------- #

#----------------#
#  Installation	 #
#----------------#

In the main directory, type ‘make’ in the command line to compile the
executables, as well as create any directories necessary for execution.

# --------------- #
#    spycommd     #
# --------------- #

Spycommd can be executed from the command line via $> ./spycommd or run as a
daemon on a Linux server via the Upstart service. (Please consult your friendly
neighbourhood IT department for information)
You can specify a port with [-p port]. (E.G. ./spycommd -p 1234)

Information concerning recieved files is written in spycommd.log

# ------------- #
#    spycomm    #
# ------------- #

Spycomm can be executed from the command line via $> ./spycomm
You can specify a host and port with [-c hostname:port].
(E.G. ./spycomm -c localhost:1234)

Spycomm will open up a graphic user interface.

# ------------------- #
# Exiting/Closing     #
# ------------------- #

spycommd can be closed safely by pressing Control+C together.

spycomm can be closed by either pressing the ‘Q’ key on the keyboard or by
pressing Control+C

# ---------------------------------- #
#     Interacting with spycomm       #
# ---------------------------------- #

Once you are in the graphic user interface, you can browse through files
via the following:

-------------------------------------------------------------------------------
|	ENTER KEY		| Select a file or change a directory	      |
|       TAB KEY			| Change between directory area and file      |
|   				| progress area.   	   	    	      |
|	ARROW KEYS		| Move up and down in an area 		      |
|	‘Q’ Key			| Quits the application			      |
-------------------------------------------------------------------------------
