# backend/Dockerfile - Multi-stage build for Cloud Storage Simulator
FROM ubuntu:22.04 AS builder
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    ca-certificates \
    libjsoncpp-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

# Copy all files from backend directory
COPY . .

# List files for debugging
RUN echo "Files in /build:" && ls -la

# Build the project
RUN if [ ! -f CMakeLists.txt ]; then echo "ERROR: CMakeLists.txt not found!"; ls -la; exit 1; fi && \
    mkdir -p build && \
    cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . --config Release

# Runtime stage
FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

# Install only minimal runtime dependencies
RUN apt-get update && apt-get install -y \
    libstdc++6 \
    libjsoncpp25 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy the built binary from builder stage
COPY --from=builder /build/build/cloud_server /app/cloud_server

# Create directories that the app might need
RUN mkdir -p /app/downloads /app/test_files && chmod +x /app/cloud_server

# Expose port (default 3001, can be overridden via PORT env var)
EXPOSE 3001

# Set default port environment variable
ENV PORT=3001

# Run the server
CMD ["./cloud_server"]
