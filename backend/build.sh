#!/bin/bash

echo "Building Cloud Storage C++ Server..."

cd "$(dirname "$0")"

# Create necessary directories
mkdir -p downloads test_files

# Create build directory
rm -rf build
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
echo "Building..."
make -j$(nproc)

if [ -f "cloud_server" ]; then
    echo "Build successful!"
    exit 0
else
    echo "Build failed - cloud_server executable not found"
    exit 1
fi