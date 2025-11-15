# Cloud Storage Simulator

A full-stack cloud storage system simulator built with React, TypeScript, C++, and CMake. This project demonstrates core OS and cloud computing concepts including file operations, process scheduling, IPC management, and deadlock detection.

## 🌐 Live Deployment

- **Frontend**: https://cloud-storagesimulator.vercel.app/
- **Backend API**: https://cloud-storagesimulator.onrender.com

## 📋 Overview

Cloud Storage Simulator is an educational platform that simulates real-world cloud storage operations with integrated OS concepts. It provides a comprehensive web interface to interact with a high-performance C++ backend server that handles file operations, system processes, and resource management.

## ✨ Features

### Cloud Storage Operations
- **File Management**: Upload, download, delete, and browse files in cloud storage
- **File Statistics**: View detailed file information including size, creation date, and modification time
- **Batch Operations**: Handle multiple file operations efficiently
- **Storage Monitoring**: Real-time tracking of storage usage and file counts

### OS Concepts Simulation
- **Process Scheduling**: Visualize and simulate CPU scheduling algorithms (FCFS, SJF, Round Robin)
- **Inter-Process Communication (IPC)**: Manage message queues and shared memory
- **Deadlock Detection**: Identify and resolve circular wait conditions in resource allocation
- **Thread Management**: Monitor and control thread operations and synchronization

### Performance Monitoring
- **Real-time Metrics**: Display CPU usage, memory consumption, and I/O operations
- **Performance Graphs**: Visual representation of system performance over time
- **Connection Status**: Monitor backend connectivity and server health
- **Activity Logging**: View detailed logs of all system operations

## 🏗️ Architecture

### Frontend
- **Framework**: React 18 with TypeScript
- **Build Tool**: Vite
- **Styling**: Tailwind CSS + shadcn/ui components
- **State Management**: React Hooks
- **HTTP Client**: Custom API service layer with environment-based configuration

**Frontend Stack:**
```
src/
├── components/          # React UI components
│   ├── CloudHeader.tsx
│   ├── CloudStats.tsx
│   ├── FileOperations.tsx
│   ├── ThreadManager.tsx
│   ├── DeadlockGraphs.tsx
│   ├── PerformanceMetrics.tsx
│   └── ui/              # shadcn/ui components
├── pages/               # Page components
│   ├── Dashboard.tsx
│   ├── Files.tsx
│   ├── Monitoring.tsx
│   ├── OSSimulator.tsx
│   └── Performance.tsx
├── services/            # API client
│   └── cloudApi.ts
├── hooks/               # Custom React hooks
└── lib/                 # Utilities
```

### Backend
- **Language**: C++ 17
- **Build System**: CMake
- **HTTP Server**: cpp-httplib
- **JSON Handling**: jsoncpp
- **Threading**: std::thread with POSIX threads

**Backend Features:**
- RESTful API endpoints for all cloud storage operations
- Multi-threaded request handling
- CORS support for cross-origin requests
- Process scheduling simulation algorithms
- Deadlock detection using graph-based algorithms
- IPC queue management

**Key Components:**
```
backend/
├── main.cpp                    # HTTP server entry point
├── cloud_storage.cpp/h        # Cloud storage operations
├── process_scheduler.cpp/h    # Process scheduling algorithms
├── deadlock_detector.cpp/h    # Deadlock detection logic
├── ipc_manager.cpp/h          # IPC queue management
├── file_system.cpp/h          # Virtual file system
├── cloud_integration.cpp      # Cloud integration features
└── CMakeLists.txt             # Build configuration
```

## 🚀 Getting Started

### Prerequisites
- **Frontend**: Node.js 16+ and npm
- **Backend**: C++ 17 compiler (g++, clang), CMake 3.10+, pthread library
- **Docker**: For containerized backend deployment

### Installation & Development

#### Frontend Development

```bash
# Install dependencies
npm install

# Start development server
npm run dev

# Build for production
npm run build

# Preview production build
npm run preview
```

#### Backend Development

```bash
cd backend

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
cmake --build .

# Run the server
./cloud_server

# Server runs on http://localhost:3001
```

#### Environment Variables (Frontend)

Create a `.env` file in the root directory:
```env
VITE_API_URL=http://localhost:3001
```

For production (Vercel):
- Set `VITE_API_URL` environment variable to your backend URL

### Docker Setup

#### Building the Docker Image

```bash
# From repository root
docker build -t cloud-storage-simulator -f backend/Dockerfile .

# Run container
docker run -p 3001:3001 cloud-storage-simulator
```

#### Docker Build Details
- **Base Image**: Ubuntu 22.04
- **Multi-stage Build**: Separate builder and runtime stages for optimized image size
- **Runtime Dependencies**: libstdc++6, libjsoncpp25, ca-certificates
- **Exposed Port**: 3001 (configurable via PORT environment variable)

## 📡 API Endpoints

### File Operations
- `GET /api/files` - List all files
- `POST /api/files/upload` - Upload a file
- `GET /api/files/<filename>` - Download a file
- `DELETE /api/files/<filename>` - Delete a file
- `GET /api/files/stats` - Get storage statistics

### Process Scheduling
- `POST /api/process/schedule` - Schedule processes
- `GET /api/process/status` - Get process status
- `POST /api/process/execute` - Execute scheduled processes

### IPC Management
- `POST /api/ipc/queue/create` - Create message queue
- `POST /api/ipc/queue/send` - Send message
- `GET /api/ipc/queue/receive` - Receive message
- `DELETE /api/ipc/queue/<id>` - Delete queue

### Deadlock Detection
- `POST /api/deadlock/detect` - Analyze for deadlocks
- `GET /api/deadlock/graph` - Get resource allocation graph
- `POST /api/deadlock/resolve` - Attempt to resolve detected deadlocks

### OS Simulation
- `GET /api/os/status` - Get system status
- `POST /api/os/process/create` - Create new process
- `POST /api/os/thread/create` - Create new thread

## 🛠️ Technology Stack

### Frontend
- React 18
- TypeScript
- Vite
- Tailwind CSS
- shadcn/ui
- React Router

### Backend
- C++ 17
- CMake
- cpp-httplib
- jsoncpp
- POSIX Threads (pthreads)

### Deployment
- **Frontend**: Vercel (Free Tier)
- **Backend**: Render.com (Free Tier)
- **Containerization**: Docker

## 📊 Performance Characteristics

- **Frontend Load Time**: < 2s (with Vercel CDN)
- **Backend Cold Start**: ~30-60s on free tier (first request after inactivity)
- **File Upload Speed**: Limited by network bandwidth and Render.com free tier (512MB RAM)
- **Concurrent Connections**: Limited by container memory (free tier: 512MB)

## 🔧 Deployment

### Frontend (Vercel)

1. Connect your GitHub repository to Vercel
2. Set environment variable:
   - `VITE_API_URL`: Your backend deployment URL
3. Deploy on push to main branch

### Backend (Render.com)

1. Connect GitHub repository to Render
2. Create new Web Service
3. Use provided `render.yaml` configuration
4. Select "Free" plan
5. Backend deploys automatically on push

**Deployment YAML Configuration:**
```yaml
services:
  - type: web
    name: cloud-storage-simulator
    runtime: docker
    dockerfilePath: ./backend/Dockerfile
    region: oregon
    plan: free
    envVars:
      - key: PORT
        value: 3001
```

## 📝 Usage Examples

### Upload a File
```typescript
const formData = new FormData();
formData.append('file', fileObject);
const response = await fetch('https://cloud-storagesimulator.onrender.com/api/files/upload', {
  method: 'POST',
  body: formData
});
```

### List Files
```typescript
const response = await fetch('https://cloud-storagesimulator.onrender.com/api/files');
const data = await response.json();
console.log(data.files);
```

### Schedule Process
```typescript
const response = await fetch('https://cloud-storagesimulator.onrender.com/api/process/schedule', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({
    processId: 1,
    burstTime: 5,
    arrivalTime: 0
  })
});
```

## 🐛 Troubleshooting

### Backend Not Connected
1. Check browser console (F12) for API Base URL and VITE_API_URL values
2. Verify environment variable is set in Vercel: `VITE_API_URL=https://cloud-storagesimulator.onrender.com`
3. Trigger Vercel redeploy after environment variable changes
4. Wait 2-3 minutes for deployment to complete
5. Check backend is running: Visit https://cloud-storagesimulator.onrender.com/api/files

### Docker Build Issues
1. Ensure build context includes `backend/` directory
2. Verify CMakeLists.txt exists in backend directory
3. Check all dependencies are installed in Docker builder stage
4. Run `docker build --no-cache` to rebuild from scratch

### Performance Issues
- Free tier services may sleep after 15 minutes of inactivity
- First request after sleep may take 30-60 seconds (cold start)
- Upgrade to paid plans for consistent performance

## 🤝 Contributing

Contributions are welcome! Please follow these guidelines:
1. Create a feature branch
2. Make your changes with clear commit messages
3. Test thoroughly on local environment
4. Push to GitHub and create a pull request

## 📄 License

This project is open source and available under the MIT License.

## 📧 Support

For issues or questions:
1. Check existing GitHub issues
2. Review the troubleshooting section above
3. Create a new GitHub issue with detailed information

## 🔐 Security Notes

- CORS is enabled for cross-origin requests
- Backend validates all file operations
- Input sanitization on file names and paths
- Thread-safe operations using mutexes and locks

## 📚 Educational Value

This project is designed for learning:
- Cloud storage system architecture
- OS concepts (scheduling, IPC, deadlock detection)
- Full-stack web application development
- C++ backend development
- Docker containerization
- Cloud deployment practices

## 🎯 Future Enhancements

- [ ] User authentication and authorization
- [ ] Database integration for persistent storage
- [ ] Advanced search functionality
- [ ] File versioning and recovery
- [ ] Compression algorithms
- [ ] Network protocol simulation
- [ ] Advanced monitoring dashboards

---

**Last Updated**: November 15, 2025

**Deployment Status**:
- Frontend: ✅ Live on Vercel
- Backend: ✅ Live on Render.com
