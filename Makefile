# ------------------------------------------- #
# Group Number:       1
# Names:              Clayton Caron
#                     Cody Moorhouse
#		      David James
#                     Marshall Anzinger
#                     Shaun Yonkers
# Group Project:      Makefile
# CMPT 361:           Class Section AS40
# Instructor's Name:  Dr. Nicholas Boers
# ------------------------------------------- #

#########
# Flags #
#########
CC=gcc
CFLAGS=-D_POSIX_C_SOURCE=200809L -D_BSD_SOURCE -Wall -pedantic -std=c99
LDLIBS=-lncurses -pthread -lm

###############
# Directories #
###############
OTP_DIR="otp"
UPLOAD_DIR="Uploads"

##############
# Executable #
##############
.PHONY: all clean

all: spycomm spycommd otpdir uploadsdir

tests: testsocket testfilemanager

spycomm: spycomm.o packet.o socket.o filemanager.o usefuloutput.o

spycommd: spycommd.o packet.o socket.o filemanager.o usefuloutput.o

testfilemanager: testfilemanager.o filemanager.o

testsocket: testsocket.o socket.o packet.o usefuloutput.o filemanager.o

################
# OBJECT FILES #
################
filemanager.o: filemanager.c filemanager.h globals.h

packet.o: packet.c packet.h filemanager.c socket.c globals.h usefuloutput.h

socket.o: socket.c socket.h packet.h usefuloutput.h globals.h

spycomm.o: spycomm.c spycomm.h socket.h globals.h

spycommd.o: spycommd.c spycommd.h packet.h globals.h

usefuloutput.o: usefuloutput.c usefuloutput.h globals.h

################
# Runtime Reqs #
################
otpdir:
	if [ ! -d "$(OTP_DIR)" ]; then mkdir -p $(OTP_DIR); fi

uploadsdir:
	if [ ! -d "$(UPLOADS_DIR)" ]; then mkdir -p $(UPLOAD_DIR); fi

#########
# TESTS #
#########
testfilemanager.o: testfilemanager.c filemanager.h globals.h

testsocket.o: socket.h testsocket.c globals.h packet.h usefuloutput.h filemanager.h

#########
# Clean #
#########
clean:
	$(RM) *.o spycommd.log spycomm spycommd testfilemanager testpacket testsocket
