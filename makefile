# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -O0 -g -MMD -MP

# Directories to search for source files
SRC_DIRS = ./util
BUILD_DIR = ./build

# Output executable
TARGET = $(BUILD_DIR)/ld

# Find all .cpp files in the specified directories
SRCS = $(shell find $(SRC_DIRS) -name '*.cpp')
SRCS += ./main.cpp

# Generate object file names in the build directory, preserving directory structure
OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

DEPS = $(OBJS:.o=.d)

# Default target
all: $(TARGET)

# Compile each .cpp file to .o in the build directory
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@) # Create the directory if it does not exist
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

-include $(DEPS)

# Clean up
clean:
	rm -f $(TARGET) $(OBJS)
	rm -rf $(BUILD_DIR)

.PHONY: all clean