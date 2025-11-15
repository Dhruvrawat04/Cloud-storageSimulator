#include "file_system.h"
#include <sstream>
#include <iomanip>

FileSystem::FileSystem(int total_blks, int blk_size) 
    : block_size(blk_size), total_blocks(total_blks), next_inode_number(1) {
    
    data_blocks.resize(total_blocks, false);
    
    // Create root directory
    createDirectory("/");
}

bool FileSystem::createDirectory(const std::string& path) {
    if (directories.find(path) != directories.end()) {
        std::cout << "Directory already exists: " << path << "\n";
        return false;
    }
    
    directories[path] = std::vector<DirectoryEntry>();
    std::cout << "Created directory: " << path << "\n";
    return true;
}

bool FileSystem::createFile(const std::string& path, int owner_id) {
    // Extract directory and filename
    size_t last_slash = path.find_last_of('/');
    std::string dir_path = (last_slash == 0) ? "/" : path.substr(0, last_slash);
    std::string filename = path.substr(last_slash + 1);
    
    if (directories.find(dir_path) == directories.end()) {
        std::cout << "Directory does not exist: " << dir_path << "\n";
        return false;
    }
    
    // Check if file already exists
    for (const auto& entry : directories[dir_path]) {
        if (entry.filename == filename) {
            std::cout << "File already exists: " << path << "\n";
            return false;
        }
    }
    
    // Create inode
    Inode inode(next_inode_number++, filename, owner_id);
    inodes[inode.inode_number] = inode;
    
    // Add to directory
    directories[dir_path].emplace_back(filename, inode.inode_number);
    
    std::cout << "Created file: " << path << " (inode: " << inode.inode_number << ")\n";
    return true;
}

bool FileSystem::writeFile(const std::string& path, const std::string& data) {
    int inode_num = findInode(path);
    if (inode_num == -1) {
        std::cout << "File not found: " << path << "\n";
        return false;
    }
    
    Inode& inode = inodes[inode_num];
    
    // Free existing blocks
    for (int block : inode.data_blocks) {
        freeBlock(block);
    }
    inode.data_blocks.clear();
    
    // Calculate blocks needed
    int blocks_needed = (data.size() + block_size - 1) / block_size;
    
    // Allocate blocks
    for (int i = 0; i < blocks_needed; i++) {
        int block = allocateBlock();
        if (block == -1) {
            std::cout << "Not enough space to write file: " << path << "\n";
            return false;
        }
        inode.data_blocks.push_back(block);
    }
    
    inode.size = data.size();
    inode.modification_time = std::time(nullptr);
    
    std::cout << "Written " << data.size() << " bytes to " << path 
              << " (" << blocks_needed << " blocks)\n";
    return true;
}

std::string FileSystem::readFile(const std::string& path) {
    int inode_num = findInode(path);
    if (inode_num == -1) {
        std::cout << "File not found: " << path << "\n";
        return "";
    }
    
    Inode& inode = inodes[inode_num];
    std::cout << "Reading file: " << path << " (" << inode.size << " bytes)\n";
    
    // Simulate file content
    std::stringstream content;
    content << "Content of file '" << inode.filename << "'\n";
    content << "Size: " << inode.size << " bytes\n";
    content << "Blocks: ";
    for (int block : inode.data_blocks) {
        content << block << " ";
    }
    content << "\n";
    
    return content.str();
}

bool FileSystem::deleteFile(const std::string& path) {
    size_t last_slash = path.find_last_of('/');
    std::string dir_path = (last_slash == 0) ? "/" : path.substr(0, last_slash);
    std::string filename = path.substr(last_slash + 1);
    
    if (directories.find(dir_path) == directories.end()) {
        std::cout << "Directory not found: " << dir_path << "\n";
        return false;
    }
    
    auto& dir_entries = directories[dir_path];
    for (auto it = dir_entries.begin(); it != dir_entries.end(); ++it) {
        if (it->filename == filename) {
            // Free data blocks
            Inode& inode = inodes[it->inode_number];
            for (int block : inode.data_blocks) {
                freeBlock(block);
            }
            
            // Remove inode
            inodes.erase(it->inode_number);
            
            // Remove directory entry
            dir_entries.erase(it);
            
            std::cout << "Deleted file: " << path << "\n";
            return true;
        }
    }
    
    std::cout << "File not found: " << path << "\n";
    return false;
}

std::vector<std::string> FileSystem::listDirectory(const std::string& path) {
    std::vector<std::string> result;
    
    if (directories.find(path) == directories.end()) {
        std::cout << "Directory not found: " << path << "\n";
        return result;
    }
    
    std::cout << "Contents of " << path << ":\n";
    for (const auto& entry : directories[path]) {
        std::string file_info = entry.filename;
        auto it = inodes.find(entry.inode_number);
        if (it != inodes.end()) {
            file_info += " (" + std::to_string(it->second.size) + " bytes)";
        }
        result.push_back(file_info);
        std::cout << "  " << file_info << "\n";
    }
    
    return result;
}

void FileSystem::displayFileSystemInfo() {
    std::cout << "\n=== FILE SYSTEM INFORMATION ===\n";
    std::cout << "Total Blocks: " << total_blocks << "\n";
    std::cout << "Block Size: " << block_size << " bytes\n";
    std::cout << "Total Capacity: " << (total_blocks * block_size) / 1024 << " KB\n";
    std::cout << "Used Blocks: " << std::count(data_blocks.begin(), data_blocks.end(), true) << "\n";
    std::cout << "Free Blocks: " << std::count(data_blocks.begin(), data_blocks.end(), false) << "\n";
    std::cout << "Disk Utilization: " << std::fixed << std::setprecision(2) 
              << getDiskUtilization() * 100 << "%\n";
    std::cout << "Total Files: " << inodes.size() << "\n";
    std::cout << "Total Directories: " << directories.size() << "\n";
}

double FileSystem::getDiskUtilization() {
    int used_blocks = std::count(data_blocks.begin(), data_blocks.end(), true);
    return static_cast<double>(used_blocks) / total_blocks;
}

int FileSystem::allocateBlock() {
    for (int i = 0; i < total_blocks; i++) {
        if (!data_blocks[i]) {
            data_blocks[i] = true;
            return i;
        }
    }
    return -1; // No free blocks
}

void FileSystem::freeBlock(int block_number) {
    if (block_number >= 0 && block_number < total_blocks) {
        data_blocks[block_number] = false;
    }
}

int FileSystem::findInode(const std::string& path) {
    size_t last_slash = path.find_last_of('/');
    std::string dir_path = (last_slash == 0) ? "/" : path.substr(0, last_slash);
    std::string filename = path.substr(last_slash + 1);
    
    if (directories.find(dir_path) == directories.end()) {
        return -1;
    }
    
    for (const auto& entry : directories[dir_path]) {
        if (entry.filename == filename) {
            return entry.inode_number;
        }
    }
    
    return -1;
}

void FileSystem::displayInodeInfo(int inode_number) {
    auto it = inodes.find(inode_number);
    if (it == inodes.end()) {
        std::cout << "Inode not found: " << inode_number << "\n";
        return;
    }
    
    const Inode& inode = it->second;
    std::cout << "\n=== INODE INFORMATION ===\n";
    std::cout << "Inode Number: " << inode.inode_number << "\n";
    std::cout << "Filename: " << inode.filename << "\n";
    std::cout << "Size: " << inode.size << " bytes\n";
    std::cout << "Owner: " << inode.owner_id << "\n";
    std::cout << "Permissions: " << inode.permissions << "\n";
    std::cout << "Created: " << std::ctime(&inode.creation_time);
    std::cout << "Modified: " << std::ctime(&inode.modification_time);
    std::cout << "Data Blocks: ";
    for (int block : inode.data_blocks) {
        std::cout << block << " ";
    }
    std::cout << "\n";
}

void FileSystem::checkConsistency() {
    std::cout << "\n=== FILE SYSTEM CONSISTENCY CHECK ===\n";
    
    int errors = 0;
    
    // Check directory entries point to valid inodes
    for (const auto& dir_pair : directories) {
        for (const auto& entry : dir_pair.second) {
            if (inodes.find(entry.inode_number) == inodes.end()) {
                std::cout << "ERROR: Directory " << dir_pair.first 
                          << " has invalid inode " << entry.inode_number << "\n";
                errors++;
            }
        }
    }
    
    // Check inodes have valid block allocations
    for (const auto& inode_pair : inodes) {
        for (int block : inode_pair.second.data_blocks) {
            if (block < 0 || block >= total_blocks || !data_blocks[block]) {
                std::cout << "ERROR: Inode " << inode_pair.first 
                          << " has invalid block " << block << "\n";
                errors++;
            }
        }
    }
    
    if (errors == 0) {
        std::cout << "File system is consistent - no errors found.\n";
    } else {
        std::cout << "Found " << errors << " consistency errors.\n";
    }
}