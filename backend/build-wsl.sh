#!/bin/bash

echo "Building Cloud Storage Server in WSL home directory..."

# Create a temporary build directory in WSL home
BUILD_DIR="$HOME/cloud-server-build"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Copy source files to WSL directory
cp -r /mnt/c/Users/capdh/desktop/cloud_storage/backend/* "$BUILD_DIR/"
cd "$BUILD_DIR"

# Check if httplib.h exists or is just a placeholder
if [ ! -f "httplib.h" ] || grep -q "placeholder" "httplib.h"; then
    echo "Downloading real httplib.h..."
    rm -f httplib.h
    wget -O httplib.h https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h
    if [ $? -ne 0 ]; then
        echo "Failed to download httplib.h. Please check your internet connection."
        exit 1
    fi
    echo "httplib.h downloaded successfully"
fi

# Install dependencies
echo "Installing dependencies..."
sudo apt-get update
sudo apt-get install -y build-essential cmake libjsoncpp-dev pkg-config

# Clean any existing build artifacts
rm -rf build
rm -f CMakeCache.txt
mkdir -p build
cd build

# Configure and build
cmake ..
make

if [ -f "cloud_server" ]; then
    echo "Build successful! Starting server..."
    echo "Server will run on http://localhost:3001"
    echo "Press Ctrl+C to stop the server"
    ./cloud_server
else
    echo "Build failed - cloud_server executable not found"
fi