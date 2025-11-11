# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

# Boost include path (adjust if Boost is in a non-standard location)
BOOST_INCLUDE = /usr/include/boost
# Uncomment and modify if Boost is not in system path:
# BOOST_INCLUDE = -I/path/to/boost/include

# Linker flags for quadmath library (required by Boost float128)
LDFLAGS = -lquadmath

# Directories
BUILD_DIR = build
BIN_DIR = bin

# Target executable
TARGET = $(BIN_DIR)/main

# Source files
SRCS = main.cpp utils.cpp

# Object files (placed in build directory)
OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

# Default target
all: $(TARGET)

# Create directories if they don't exist
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

# Link object files to create executable
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile source files to object files in build directory
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(BOOST_INCLUDE) -c $< -o $@

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean generated files and directories
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all run clean
