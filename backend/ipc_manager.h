#ifndef IPC_MANAGER_H
#define IPC_MANAGER_H

#include <queue>
#include <map>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <iostream>
#include <ctime>
#include <cstring>
#include <string>

struct Message {
    int sender_id;
    int receiver_id;
    std::string content;
    int message_id;
    std::time_t timestamp;
    
    // ADD DEFAULT CONSTRUCTOR HERE
    Message() 
        : sender_id(-1), receiver_id(-1), content(""), message_id(-1), 
          timestamp(std::time(nullptr)) {}
    
    Message(int sender, int receiver, const std::string& msg, int id)
        : sender_id(sender), receiver_id(receiver), content(msg), 
          message_id(id), timestamp(std::time(nullptr)) {}
};

class MessageQueue {
private:
    std::queue<Message> messages;
    std::mutex mtx;
    std::condition_variable cv;
    int max_size;

public:
    MessageQueue(int max_sz = 100) : max_size(max_sz) {}
    
    // ADD MOVE CONSTRUCTOR AND ASSIGNMENT
    MessageQueue(MessageQueue&& other) noexcept 
        : messages(std::move(other.messages))
        , max_size(other.max_size) {
        // Note: mutex and condition_variable are not movable
    }
    
    MessageQueue& operator=(MessageQueue&& other) noexcept {
        if (this != &other) {
            std::lock_guard<std::mutex> lock(mtx);
            std::lock_guard<std::mutex> other_lock(other.mtx);
            messages = std::move(other.messages);
            max_size = other.max_size;
        }
        return *this;
    }
    
    // DELETE COPY OPERATIONS
    MessageQueue(const MessageQueue&) = delete;
    MessageQueue& operator=(const MessageQueue&) = delete;
    
    bool sendMessage(const Message& msg);
    Message receiveMessage(int receiver_id);
    bool isEmpty() const;
    int getMessageCount() const;
};

class SharedMemory {
private:
    std::map<std::string, void*> memory_segments;
    std::map<std::string, size_t> segment_sizes;
    std::mutex mtx;

public:
    SharedMemory() = default;
    
    void* createSegment(const std::string& name, size_t size);
    void* getSegment(const std::string& name);
    bool deleteSegment(const std::string& name);
    size_t getSegmentSize(const std::string& name);
    void displaySegments();
    
    // Public accessors
    const std::map<std::string, void*>& getSegments() const { return memory_segments; }
    const std::map<std::string, size_t>& getSegmentSizes() const { return segment_sizes; }
};

class IPCManager {
private:
    std::map<int, MessageQueue> message_queues;
    SharedMemory shared_memory;
    std::map<std::string, std::condition_variable> condition_variables;
    std::map<std::string, std::mutex> named_mutexes;
    int next_message_id;

public:
    IPCManager() : next_message_id(1) {}
    
    // Message passing
    void createMessageQueue(int queue_id, int max_size = 100);
    bool sendMessage(int queue_id, int sender, int receiver, const std::string& content);
    Message receiveMessage(int queue_id, int receiver);
    
    // Shared memory
    void* createSharedMemory(const std::string& name, size_t size);
    void* accessSharedMemory(const std::string& name);
    
    // Synchronization primitives
    void createNamedMutex(const std::string& name);
    void lockNamedMutex(const std::string& name);
    void unlockNamedMutex(const std::string& name);
    
    // Monitoring
    void displayIPCStatus();
    int getTotalMessages();
    size_t getTotalSharedMemory();
    int getMessageQueueCount();
    int getSharedMemoryCount();
    void interactiveSendMessage();
    void viewAllMessages();
    void clearMessageQueue(int queue_id);
    
    // NEW: Shared memory interaction functions  
    void interactiveCreateSharedMemory();
    void viewSharedMemoryContent(const std::string& name);
    void writeToSharedMemory(const std::string& name, const std::string& data);
    std::string readFromSharedMemory(const std::string& name);
    
    // NEW: Utility functions
    void displayAllMessageQueues();
    void displayMessageQueueDetails(int queue_id);
};

#endif
