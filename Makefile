# DevHelper+Portal CLI - Makefile
# Compilation settings for cross-platform build

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c11 -O2

# Include paths
INCLUDES = -I.

# Libraries
# Note: Install libcurl development package before compiling
# macOS: brew install curl
# Linux: sudo apt-get install libcurl4-openssl-dev
# Windows: Download from https://curl.se/download.html
LIBS = -lcurl

# Source files
SOURCES = main.c \
          utils.c \
          project_setup.c \
          system_monitor.c \
          git_helper.c \
          weather.c \
          ucam_portal.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Target executable
TARGET = devhelper

# Platform detection
ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
    RM = del /Q
    RMDIR = rmdir /S /Q
else
    UNAME_S := $(shell uname -s)
    RM = rm -f
    RMDIR = rm -rf
    
    ifeq ($(UNAME_S),Darwin)
        # macOS specific flags
        CFLAGS += -arch x86_64
    endif
endif

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJECTS) $(LIBS)
	@echo "Build complete! Run with: ./$(TARGET)"

# Compile source files
%.o: %.c devhelper.h
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean build files
clean:
	@echo "Cleaning build files..."
	$(RM) $(OBJECTS) $(TARGET)
	$(RM) ucam_cookies.txt
	@echo "Clean complete!"

# Install dependencies (macOS)
install-deps-mac:
	@echo "Installing dependencies for macOS..."
	brew install curl
	@echo "Dependencies installed!"

# Install dependencies (Linux)
install-deps-linux:
	@echo "Installing dependencies for Linux..."
	sudo apt-get update
	sudo apt-get install -y libcurl4-openssl-dev build-essential
	@echo "Dependencies installed!"

# Run the program
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: clean $(TARGET)
	@echo "Debug build complete!"

# Help
help:
	@echo "DevHelper+Portal CLI - Makefile Commands"
	@echo ""
	@echo "Available targets:"
	@echo "  make              - Build the project"
	@echo "  make all          - Build the project (same as make)"
	@echo "  make clean        - Remove all build files"
	@echo "  make run          - Build and run the program"
	@echo "  make debug        - Build with debug symbols"
	@echo "  make install-deps-mac   - Install dependencies on macOS"
	@echo "  make install-deps-linux - Install dependencies on Linux"
	@echo "  make help         - Show this help message"
	@echo ""
	@echo "Before building, make sure libcurl is installed:"
	@echo "  macOS:   brew install curl"
	@echo "  Linux:   sudo apt-get install libcurl4-openssl-dev"
	@echo "  Windows: Download from https://curl.se/download.html"

.PHONY: all clean run debug help install-deps-mac install-deps-linux
