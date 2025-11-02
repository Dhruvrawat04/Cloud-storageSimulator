#!/bin/bash

echo "Building Cloud Storage Server..."

# Check if httplib.h exists
if [ ! -f "httplib.h" ]; then
    echo "Downloading httplib.h..."
    wget https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h
fi

# Install dependencies on Ubuntu/Debian
echo "Installing dependencies..."
sudo apt-get update
sudo apt-get install -y build-essential cmake libjsoncpp-dev pkg-config

# Clean and create build directory
rm -rf build
mkdir -p build
cd build

# Configure and build
cmake ..
make

echo "Build complete! Run with: ./cloud_server"