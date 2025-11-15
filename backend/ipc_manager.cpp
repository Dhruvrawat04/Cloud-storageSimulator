#include "ipc_manager.h"
#include <stdexcept>
#include <cstring>
#include <iostream>

using namespace std;

bool MessageQueue::sendMessage(const Message& msg) {
    unique_lock<mutex> lock(mtx);
    
    if (static_cast<int>(messages.size()) >= max_size) {
        cout << "Message queue is full. Cannot send message.\n";
        return false;
    }
    
    messages.push(msg);
    cv.notify_one();
    
    cout << "Message sent from " << msg.sender_id << " to " << msg.receiver_id 
              << " (ID: " << msg.message_id << ")\n";
    return true;
}

Message MessageQueue::receiveMessage(int receiver_id) {
    unique_lock<mutex> lock(mtx);
    
    // Wait for message with matching receiver_id
    cv.wait(lock, [this, receiver_id]() {
        // Inspect queue without modifying it
        queue<Message> temp = messages;
        while (!temp.empty()) {
            if (temp.front().receiver_id == receiver_id) {
                return true;
            }
            temp.pop();
        }
        return false;
    });
    
    // Find and extract the matching message
    queue<Message> temp_queue;
    Message result; // Now uses the default constructor
    bool found = false;
    
    while (!messages.empty()) {
        Message msg = messages.front();
        messages.pop();
        
        if (!found && msg.receiver_id == receiver_id) {
            result = msg;
            found = true;
        } else {
            temp_queue.push(msg);
        }
    }
    
    // Restore remaining messages
    messages = move(temp_queue);
    
    if (found) {
        cout << "Message received by " << receiver_id << " from " 
                  << result.sender_id << ": " << result.content << "\n";
        return result;
    }
    
    throw runtime_error("Message not found after condition variable wakeup");
}

bool MessageQueue::isEmpty() const {
    return messages.empty();
}

int MessageQueue::getMessageCount() const {
    return messages.size();
}

void* SharedMemory::createSegment(const string& name, size_t size) {
    lock_guard<mutex> lock(mtx);
    
    if (memory_segments.find(name) != memory_segments.end()) {
        cout << "Shared memory segment already exists: " << name << "\n";
        return memory_segments[name];
    }
    
    void* segment = malloc(size);
    if (segment) {
        memset(segment, 0, size);
        memory_segments[name] = segment;
        segment_sizes[name] = size;
        cout << "Created shared memory segment: " << name << " (" << size << " bytes)\n";
    } else {
        cout << "Failed to create shared memory segment: " << name << "\n";
    }
    
    return segment;
}

void* SharedMemory::getSegment(const string& name) {
    lock_guard<mutex> lock(mtx);
    auto it = memory_segments.find(name);
    return (it != memory_segments.end()) ? it->second : nullptr;
}

bool SharedMemory::deleteSegment(const string& name) {
    lock_guard<mutex> lock(mtx);
    
    auto it = memory_segments.find(name);
    if (it != memory_segments.end()) {
        free(it->second);
        memory_segments.erase(it);
        segment_sizes.erase(name);
        cout << "Deleted shared memory segment: " << name << "\n";
        return true;
    }
    
    cout << "Shared memory segment not found: " << name << "\n";
    return false;
}

size_t SharedMemory::getSegmentSize(const string& name) {
    lock_guard<mutex> lock(mtx);
    auto it = segment_sizes.find(name);
    return (it != segment_sizes.end()) ? it->second : 0;
}

void SharedMemory::displaySegments() {
    lock_guard<mutex> lock(mtx);
    
    cout << "\n=== SHARED MEMORY SEGMENTS ===\n";
    for (const auto& segment : memory_segments) {
        cout << "Name: " << segment.first 
                  << ", Size: " << segment_sizes.at(segment.first) << " bytes\n";
    }
}

void IPCManager::createMessageQueue(int queue_id, int max_size) {
    // Use try_emplace for C++17 compatibility
    message_queues.try_emplace(queue_id, max_size);
    cout << "Created message queue with ID: " << queue_id << "\n";
}

bool IPCManager::sendMessage(int queue_id, int sender, int receiver, const string& content) {
    auto it = message_queues.find(queue_id);
    if (it == message_queues.end()) {
        cout << "Message queue not found: " << queue_id << "\n";
        return false;
    }
    
    Message msg(sender, receiver, content, next_message_id++);
    return it->second.sendMessage(msg);
}

Message IPCManager::receiveMessage(int queue_id, int receiver) {
    auto it = message_queues.find(queue_id);
    if (it == message_queues.end()) {
        throw runtime_error("Message queue not found: " + to_string(queue_id));
    }
    
    return it->second.receiveMessage(receiver);
}

void* IPCManager::createSharedMemory(const string& name, size_t size) {
    return shared_memory.createSegment(name, size);
}

void* IPCManager::accessSharedMemory(const string& name) {
    return shared_memory.getSegment(name);
}

void IPCManager::createNamedMutex(const string& name) {
    // Use try_emplace for mutex
    named_mutexes.try_emplace(name);
    cout << "Created named mutex: " << name << "\n";
}

void IPCManager::lockNamedMutex(const string& name) {
    auto it = named_mutexes.find(name);
    if (it != named_mutexes.end()) {
        it->second.lock();
        cout << "Locked mutex: " << name << "\n";
    } else {
        cout << "Mutex not found: " << name << "\n";
    }
}

void IPCManager::unlockNamedMutex(const string& name) {
    auto it = named_mutexes.find(name);
    if (it != named_mutexes.end()) {
        it->second.unlock();
        cout << "Unlocked mutex: " << name << "\n";
    } else {
        cout << "Mutex not found: " << name << "\n";
    }
}

void IPCManager::displayIPCStatus() {
    cout << "\n=== IPC SYSTEM STATUS ===\n";
    cout << "Message Queues: " << message_queues.size() << "\n";
    cout << "Total Messages: " << getTotalMessages() << "\n";
    cout << "Named Mutexes: " << named_mutexes.size() << "\n";
    
    shared_memory.displaySegments();
}

int IPCManager::getTotalMessages() {
    int total = 0;
    for (const auto& queue : message_queues) {
        total += queue.second.getMessageCount();
    }
    return total;
}

size_t IPCManager::getTotalSharedMemory() {
    size_t total = 0;
    auto segments = shared_memory.getSegments();
    auto sizes = shared_memory.getSegmentSizes();
    
    for (const auto& segment : segments) {
        auto it = sizes.find(segment.first);
        if (it != sizes.end()) {
            total += it->second;
        }
    }
    return total;
}
#include <limits> // Add this include at the top

// Add these implementations to ipc_manager.cpp

void IPCManager::interactiveSendMessage() {
    std::cout << "\n=== SEND MESSAGE ===\n";
    
    int queue_id, sender_id, receiver_id;
    std::string content;
    
    // Get queue ID
    std::cout << "Enter Message Queue ID: ";
    if (!(std::cin >> queue_id)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "❌ Invalid queue ID!\n";
        return;
    }
    
    // Check if queue exists
    if (message_queues.find(queue_id) == message_queues.end()) {
        std::cout << "❌ Message queue " << queue_id << " does not exist!\n";
        return;
    }
    
    // Get sender ID
    std::cout << "Enter Sender ID: ";
    if (!(std::cin >> sender_id)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "❌ Invalid sender ID!\n";
        return;
    }
    
    // Get receiver ID
    std::cout << "Enter Receiver ID: ";
    if (!(std::cin >> receiver_id)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "❌ Invalid receiver ID!\n";
        return;
    }
    
    // Get message content
    std::cout << "Enter Message Content: ";
    std::cin.ignore(); // Clear newline
    std::getline(std::cin, content);
    
    if (content.empty()) {
        std::cout << "❌ Message content cannot be empty!\n";
        return;
    }
    
    // Send the message
    if (sendMessage(queue_id, sender_id, receiver_id, content)) {
        std::cout << "✅ Message sent successfully!\n";
    } else {
        std::cout << "❌ Failed to send message!\n";
    }
}

void IPCManager::viewAllMessages() {
    std::cout << "\n=== ALL MESSAGES IN SYSTEM ===\n";
    
    if (message_queues.empty()) {
        std::cout << "No message queues exist.\n";
        return;
    }
    
    for (const auto& queue_pair : message_queues) {
        std::cout << "\n--- Message Queue " << queue_pair.first << " ---\n";
        std::cout << "Message Count: " << queue_pair.second.getMessageCount() << "\n";
        
        // Note: In a real system, you'd have a way to peek at messages without removing them
        // For this simulation, we'll just show the count
        if (queue_pair.second.getMessageCount() > 0) {
            std::cout << "📫 Contains " << queue_pair.second.getMessageCount() << " unread messages\n";
        } else {
            std::cout << "📭 No messages in this queue\n";
        }
    }
}

void IPCManager::clearMessageQueue(int queue_id) {
    auto it = message_queues.find(queue_id);
    if (it == message_queues.end()) {
        std::cout << "❌ Message queue " << queue_id << " not found!\n";
        return;
    }
    
    // Create a new empty queue to replace the old one
    int max_size = 100; // Default size
    message_queues[queue_id] = MessageQueue(max_size);
    
    std::cout << "✅ Cleared all messages from queue " << queue_id << "\n";
}

void IPCManager::interactiveCreateSharedMemory() {
    std::cout << "\n=== CREATE SHARED MEMORY ===\n";
    
    std::string name;
    size_t size;
    
    // Get segment name
    std::cout << "Enter Shared Memory Name: ";
    std::cin.ignore();
    std::getline(std::cin, name);
    
    if (name.empty()) {
        std::cout << "❌ Shared memory name cannot be empty!\n";
        return;
    }
    
    // Get size
    std::cout << "Enter Size (in bytes): ";
    if (!(std::cin >> size) || size == 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "❌ Invalid size!\n";
        return;
    }
    
    // Create shared memory
    void* segment = createSharedMemory(name, size);
    if (segment) {
        std::cout << "✅ Shared memory '" << name << "' created successfully!\n";
        
        // Option to write initial data
        std::cout << "Do you want to write initial data? (y/n): ";
        char choice;
        std::cin >> choice;
        
        if (choice == 'y' || choice == 'Y') {
            std::string initial_data;
            std::cout << "Enter initial data: ";
            std::cin.ignore();
            std::getline(std::cin, initial_data);
            
            writeToSharedMemory(name, initial_data);
        }
    }
}

void IPCManager::viewSharedMemoryContent(const std::string& name) {
    void* segment = accessSharedMemory(name);
    if (!segment) {
        std::cout << "❌ Shared memory '" << name << "' not found!\n";
        return;
    }
    
    // Get segment size from shared_memory
    size_t size = shared_memory.getSegmentSize(name);
    
    std::cout << "\n=== SHARED MEMORY CONTENT ===\n";
    std::cout << "Name: " << name << "\n";
    std::cout << "Size: " << size << " bytes\n";
    std::cout << "Content: \"";
    
    // Print the content as string (assuming it's null-terminated)
    char* data = static_cast<char*>(segment);
    for (size_t i = 0; i < size && data[i] != '\0'; i++) {
        std::cout << data[i];
    }
    std::cout << "\"\n";
    
    // Also show raw bytes
    std::cout << "Raw bytes (first 100): ";
    for (size_t i = 0; i < std::min(size, (size_t)100); i++) {
        if (isprint(data[i])) {
            std::cout << data[i];
        } else {
            std::cout << ".";
        }
    }
    std::cout << "\n";
}

void IPCManager::writeToSharedMemory(const std::string& name, const std::string& data) {
    void* segment = accessSharedMemory(name);
    if (!segment) {
        std::cout << "❌ Shared memory '" << name << "' not found!\n";
        return;
    }
    
    size_t size = shared_memory.getSegmentSize(name);
    if (data.size() >= size) {
        std::cout << "⚠️ Warning: Data truncated to fit in " << size << " bytes\n";
    }
    
    // Copy data to shared memory
    char* seg_data = static_cast<char*>(segment);
    strncpy(seg_data, data.c_str(), size - 1);
    seg_data[size - 1] = '\0'; // Ensure null termination
    
    std::cout << "✅ Written to shared memory '" << name << "': \"" << data << "\"\n";
}

std::string IPCManager::readFromSharedMemory(const std::string& name) {
    void* segment = accessSharedMemory(name);
    if (!segment) {
        return "❌ Shared memory segment not found!";
    }
    
    char* data = static_cast<char*>(segment);
    return std::string(data);
}

void IPCManager::displayAllMessageQueues() {
    std::cout << "\n=== ALL MESSAGE QUEUES ===\n";
    
    if (message_queues.empty()) {
        std::cout << "No message queues exist.\n";
        return;
    }
    
    for (const auto& queue_pair : message_queues) {
        std::cout << "Queue ID: " << queue_pair.first 
                  << " | Messages: " << queue_pair.second.getMessageCount() << "\n";
    }
}

void IPCManager::displayMessageQueueDetails(int queue_id) {
    auto it = message_queues.find(queue_id);
    if (it == message_queues.end()) {
        std::cout << "❌ Message queue " << queue_id << " not found!\n";
        return;
    }
    
    std::cout << "\n=== MESSAGE QUEUE " << queue_id << " DETAILS ===\n";
    std::cout << "Message Count: " << it->second.getMessageCount() << "\n";
    std::cout << "Status: " << (it->second.isEmpty() ? "Empty" : "Has messages") << "\n";
}

int IPCManager::getMessageQueueCount() {
    return message_queues.size();
}

int IPCManager::getSharedMemoryCount() {
    return shared_memory.getSegments().size();
}
