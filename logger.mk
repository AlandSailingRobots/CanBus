

SRC	 			= CANBUS_Logger.cpp  $(CORE_SRC) 	



OBJECTS = $(addprefix $(BUILD_DIR)/, $(SRC:.cpp=.o))


all: $(LOGGER_EXEC) stats
# Link and build
$(LOGGER_EXEC): $(OBJECTS)
	rm -f $(OBJECT_FILE)
	@echo -n " " $(OBJECTS) >> $(OBJECT_FILE)
	@echo Linking object files
	$(CXX) $(LDFLAGS) @$(OBJECT_FILE) -Wl,-rpath=./ -o $@ $(LIBS)

# Compile CPP files into the build folder
$(BUILD_DIR)/%.o:$(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo Compiling CPP File: $@
	@$(CXX) -c $(CPPFLAGS) $(INC_DIR) -o ./$@ $< $(DEFINES) $(LIBS)

stats:$(LOGGER_EXEC)
	@echo Final executable size:
	$(SIZE) $(LOGGER_EXEC)
#  Create the directories needed
$(BUILD_DIR):
	@$(MKDIR_P) $(BUILD_DIR)

