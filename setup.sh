#!/bin/bash
# Quick Setup Script for DevHelper+Portal CLI
# This script helps install dependencies and build the project

set -e  # Exit on error

echo "======================================"
echo "DevHelper+Portal CLI - Setup Script"
echo "======================================"
echo ""

# Detect OS
OS="$(uname -s)"
echo "Detected OS: $OS"
echo ""

# Install dependencies based on OS
case "$OS" in
    Linux*)
        echo "Installing dependencies for Linux..."
        echo "This requires sudo privileges."
        echo ""
        
        # Check if apt-get is available
        if command -v apt-get &> /dev/null; then
            sudo apt-get update
            sudo apt-get install -y libcurl4-openssl-dev build-essential
            echo "✓ Dependencies installed!"
        elif command -v yum &> /dev/null; then
            sudo yum install -y libcurl-devel gcc make
            echo "✓ Dependencies installed!"
        else
            echo "⚠ Could not detect package manager. Please install libcurl-dev manually."
        fi
        ;;
        
    Darwin*)
        echo "Installing dependencies for macOS..."
        
        # Check if Homebrew is installed
        if command -v brew &> /dev/null; then
            brew install curl
            echo "✓ Dependencies installed!"
        else
            echo "⚠ Homebrew not found. Please install it from https://brew.sh/"
            echo "  Then run: brew install curl"
            exit 1
        fi
        ;;
        
    MINGW*|MSYS*|CYGWIN*)
        echo "Windows detected."
        echo "Please install libcurl manually from: https://curl.se/download.html"
        echo "Or use MinGW/MSYS2 package manager."
        ;;
        
    *)
        echo "Unknown OS. Please install libcurl development package manually."
        ;;
esac

echo ""
echo "======================================"
echo "Building the project..."
echo "======================================"
echo ""

# Build the project
if command -v make &> /dev/null; then
    make clean
    make
    
    echo ""
    echo "======================================"
    echo "✓ Build complete!"
    echo "======================================"
    echo ""
    echo "Run the program with:"
    echo "  ./devhelper"
    echo ""
    echo "Or use:"
    echo "  make run"
    echo ""
else
    echo "⚠ Make not found. Compiling manually..."
    gcc -Wall -Wextra -std=c11 -O2 -o devhelper main.c utils.c project_setup.c system_monitor.c git_helper.c weather.c ucam_portal.c -lcurl
    
    if [ $? -eq 0 ]; then
        echo "✓ Build complete!"
        echo "Run the program with: ./devhelper"
    else
        echo "✗ Build failed. Please check error messages above."
        exit 1
    fi
fi

echo ""
echo "======================================"
echo "Setup Notes"
echo "======================================"
echo ""
echo "1. Weather Checker:"
echo "   - Get a free API key from: https://openweathermap.org/api"
echo "   - Set it in the app menu or edit weather.c"
echo ""
echo "2. UCAM Portal:"
echo "   - Use your UIU credentials to login"
echo "   - Some features may need customization based on portal structure"
echo ""
echo "3. For help:"
echo "   - Read README.md"
echo "   - Run: make help"
echo ""
echo "Happy coding! 🚀"
echo ""
