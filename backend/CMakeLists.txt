cmake_minimum_required(VERSION 3.12)
project(CloudStorageServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Find jsoncpp
pkg_check_modules(JSONCPP jsoncpp)
if(NOT JSONCPP_FOUND)
    message(FATAL_ERROR "jsoncpp not found. Install with: sudo apt-get install libjsoncpp-dev")
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${JSONCPP_INCLUDE_DIRS})

# Add executable with all source files
add_executable(cloud_server 
    main.cpp
    cloud_storage.cpp
    cloud_rw.cpp
)

# Link libraries
target_link_libraries(cloud_server ${JSONCPP_LIBRARIES})

# Compiler flags
target_compile_options(cloud_server PRIVATE ${JSONCPP_CFLAGS_OTHER})

# Find threads
find_package(Threads REQUIRED)
target_link_libraries(cloud_server Threads::Threads)

# Add httplib header
target_include_directories(cloud_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})