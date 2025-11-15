#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <vector>
#include <map>
#include <string>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <filesystem>

struct Inode {
    int inode_number;
    std::string filename;
    int size;
    time_t creation_time;
    time_t modification_time;
    std::string permissions;
    int owner_id;
    std::vector<int> data_blocks;
    
    // Add default constructor
    Inode() : inode_number(-1), filename(""), size(0), creation_time(0), 
              modification_time(0), permissions(""), owner_id(0) {}
    
    Inode(int num, const std::string& name, int owner = 0)
        : inode_number(num), filename(name), size(0), creation_time(std::time(nullptr)),
          modification_time(std::time(nullptr)), permissions("rw-r--r--"), owner_id(owner) {}
};
struct DirectoryEntry {
    std::string filename;
    int inode_number;
    
    DirectoryEntry(const std::string& name, int inode) 
        : filename(name), inode_number(inode) {}
};

class FileSystem {
private:
    std::map<int, Inode> inodes;
    std::map<std::string, std::vector<DirectoryEntry>> directories;
    std::vector<bool> data_blocks;
    int block_size;
    int total_blocks;
    int next_inode_number;

public:
    FileSystem(int total_blocks = 1024, int blk_size = 4096);
    
    // File operations
    bool createFile(const std::string& path, int owner_id = 0);
    bool deleteFile(const std::string& path);
    bool writeFile(const std::string& path, const std::string& data);
    std::string readFile(const std::string& path);
    bool copyFile(const std::string& src, const std::string& dest);
    bool moveFile(const std::string& src, const std::string& dest);
    
    // Directory operations
    bool createDirectory(const std::string& path);
    bool deleteDirectory(const std::string& path);
    std::vector<std::string> listDirectory(const std::string& path);
    
    // File system operations
    void displayFileSystemInfo();
    void displayInodeInfo(int inode_number);
    double getDiskUtilization();
    void checkConsistency();
    
    // Utility functions
    int allocateBlock();
    void freeBlock(int block_number);
    int findInode(const std::string& path);
    std::string getAbsolutePath(const std::string& path);
};

#endif
