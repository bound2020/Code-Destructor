# This is the makefile used to build the dlib C++ library's regression test suite
# on Debian Linux using the gcc compiler.

# this is the name of the output executable
TARGET = kdd

# these are the compile time flags passed to gcc
CFLAGS = -ggdb  -DDEBUG -DDLIB_NO_GUI_SUPPORT  -I /export/kdd/dlib-17.45/  -Wall

# These are the link time flags passed to gcc
LFLAGS = -lpthread   -lnsl 

# The name of the compiler.  If you only have one version of
# gcc installed then you probably want to change this to just g++ 
CC = nice g++

####################################################
####################################################
#  Here we list all the cpp files we want to compile

SRC = main.cpp
SRC += rep.cpp
SRC += naive_bay.cpp
SRC += /export/kdd/dlib-17.45/dlib/all/source.cpp

####################################################

TMP = $(SRC:.cpp=.o)
OBJ = $(TMP:.c=.o)

$(TARGET): $(OBJ) 
	@echo Linking $@
	@$(CC) $(OBJ) $(LFLAGS) -o $@
	@echo Build Complete

.cpp.o: $<
	@echo Compiling $<
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	@rm -f $(OBJ) $(TARGET)
	@echo All object files and binaries removed

dep: 
	@echo Running makedepend
	@makedepend -- $(CFLAGS) -- $(SRC) 2> /dev/null 
	@echo Completed makedepend

###############################################################################
##########  Stuff from makedepend                                         #####
##########  type make dep at the command line to rebuild the dependencies #####
##########  Also, DON'T edit the contents of this file beyond this line.  #####
###############################################################################

