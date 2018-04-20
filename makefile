###############################################################################
#
# Master makefile of the Navigation System
#
#   Type 'make help' to display help text, with targets and external variables
#
# Usage: make [target] [ext variable 1] [ext variable 2]
#
#   Targets:
#		* ASPire: Build for ASPire
#		* Janet: Build for Janet
#
#   External Variables
#   	* USE_SIM: Indicates if the simulator is to be used, 0 for off, 1 for on.
#		* USE_LNM: 1: Local Navigation Module (voter system), 0: Line-follow (default)
#
#   Example
#   	Build the Janet navigation system with simulator interface and local navigation module
#   	'make Janet USE_SIM=1 USE_LNM=1'
#
# For the default code:   'make'
#
###############################################################################



###############################################################################
# Tools
###############################################################################

export CPPFLAGS             = -g -Wall -pedantic -Werror -std=gnu++14 -Wno-psabi
export LIBS                 = -lsqlite3 -lgps -lrt -lcurl -pthread -lwiringPi -lncurses


export CC               	= gcc
export CXX					= g++
export SIZE             	= size


export MKDIR_P				= mkdir -p




###############################################################################
# Folder Paths
###############################################################################

export SRC_DIR				= ./
export BUILD_DIR        	= build
export EXEC_DIR         	= ./

export INC_DIR         	 	= -I./ -I./Libs




###############################################################################
# Target Output
###############################################################################

export EXECUTABLE           = CANBus_logger.run


export OBJECT_FILE          = $(BUILD_DIR)/objects.tmp


###############################################################################
# Source Files
###############################################################################

SRC 						= Canbus.cpp mcp2515.cpp MsgParsing.cpp NMEA2k.cpp 


OBJECTS = $(addprefix $(BUILD_DIR)/, $(SRC:.cpp=.o))
###############################################################################
# Rules
###############################################################################

.PHONY: clean help

## Default, same as make ASPire
all: $(EXECUTABLE) stats

# Link and build
$(EXECUTABLE): $(OBJECTS)
	rm -f $(OBJECT_FILE)
	@echo -n " " $(OBJECTS) >> $(OBJECT_FILE)
	@echo Linking object files
	$(CXX) $(LDFLAGS) @$(OBJECT_FILE) -Wl,-rpath=./ -o $@ $(LIBS)

# Compile CPP files into the build folder
$(BUILD_DIR)/%.o:$(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo Compiling CPP File: $@
	@$(CXX) -c $(CPPFLAGS) $(INC_DIR) -o ./$@ $< $(DEFINES) $(LIBS)

stats:$(EXECUTABLE)
	@echo Final executable size:
	$(SIZE) $(EXECUTABLE)
#  Create the directories needed
$(BUILD_DIR):
	@$(MKDIR_P) $(BUILD_DIR)


## Remove object files and executables
clean:
	@echo Removing existing object files and executable
	-@rm -rd $(BUILD_DIR)
	-@rm $(EXECUTABLE)
	@echo DONE


## Displays this help text
help:
	@echo -e '\nUsage: make [target] [ext variable 1] [ext variable 2]'
	@echo -e '\nAvailable targets:'

#	$(info Available targets: )
	@awk '/^[a-zA-Z\-\_0-9]+:/ {                    	\
		nb = sub( /^## /, "", helpMsg );              	\
		if(nb == 0) {                                 	\
	  		helpMsg = $$0;                              \
	  		nb = sub( /^[^:]*:.* ## /, "", helpMsg );   \
		}                                             	\
		if (nb)                                       	\
			print "\t" $$1 helpMsg;                     \
	}                                               	\
	{ helpMsg = $$0 }'                              	\
	$(MAKEFILE_LIST) | column -ts:

	@echo -e '\nExternal Variables:'
	@echo -e '\tUSE_SIM = 1:Use with simulator	0: Without (default)'
	@echo -e '\tUSE_LNM = 1:Voter System	0: Line-follow (default)'
