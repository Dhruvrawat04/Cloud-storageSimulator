# Cloud Storage C++ HTTP Server

This is a C++ HTTP server that provides REST API endpoints for your cloud storage application.

## Prerequisites

- C++17 compatible compiler (g++ 7+ or clang++ 5+)
- CMake 3.12+
- libjsoncpp-dev
- wget (for downloading httplib.h)

## Installation

### Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libjsoncpp-dev wget
```

### CentOS/RHEL:
```bash
sudo yum install gcc-c++ cmake jsoncpp-devel wget
```

### macOS:
```bash
brew install cmake jsoncpp wget
```

## Building

1. Make the build script executable:
```bash
chmod +x build-wsl.sh
```

2. Run the build script:
```bash
./build.sh
```

Or build manually:
```bash
# Download httplib.h
wget https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make
```

## Running

After building, run the server:
```bash
cd build
./cloud_server
```

The server will start on `http://localhost:8080`

## API Endpoints

### Files
- `GET /api/files` - List all files
- `POST /api/files/upload` - Upload a file
- `DELETE /api/files/{id}` - Delete a file by ID

### Statistics
- `GET /api/stats` - Get cloud storage statistics

### Logs
- `GET /api/logs` - Get system logs

### Threads
- `GET /api/threads` - List active threads
- `POST /api/threads` - Create a new thread

### Health
- `GET /api/health` - Health check endpoint

## Frontend Integration

Update your frontend to point to `http://localhost:8080` for API calls.

## CORS

The server includes CORS headers to allow frontend connections from any origin.

## Notes

- This is a demo server with mock data
- File upload functionality saves files to memory (implement persistent storage as needed)
- Thread management is simulated for demonstration
- Logs are stored in memory (implement persistent logging as needed)