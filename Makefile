# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -Werror -std=c++20 -Iheaders -DDEBUG

# Linker flags
LDFLAGS = -static-libstdc++ -static-libgcc -lssl -lcrypto

# Directories
SRC_DIR = src
HDR_DIR = headers
OBJ_DIR = obj

# Executable name
TARGET = Grigori

# Find all .cpp files in src/
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Create matching object file names in obj/
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Default target
all: $(TARGET)

# Link object files into final executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile .cpp files into .o files inside obj/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Rebuild everything
rebuild: clean all

.PHONY: all clean rebuild
