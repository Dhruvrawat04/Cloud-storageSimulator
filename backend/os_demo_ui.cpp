#include "cloud.h"
#include "process_scheduler.h"
#include "file_system.h"
#include "ipc_manager.h"
#include "deadlock_detector.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>

// Define global instances (REMOVED MemoryManager)
ProcessScheduler process_scheduler;
FileSystem file_system(256, 512);
IPCManager ipc_manager;
DeadlockDetector deadlock_detector;

// Add this function declaration
void run_automatic_ipc_demo();

// ===== PROCESS MANAGEMENT CONSOLE =====

void run_process_management_demo() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "⚙️  PROCESS MANAGEMENT CONSOLE\n";
    std::cout << std::string(60, '=') << "\n";
    
    int choice;
    bool management_mode = true;
    
    while (management_mode) {
        std::cout << "\n" << std::string(50, '-') << "\n";
        std::cout << "PROCESS MANAGEMENT MENU\n";
        std::cout << std::string(50, '-') << "\n";
        process_scheduler.displayCurrentProcesses();
        std::cout << std::string(50, '-') << "\n";
        
        std::cout << "1. ➕ Add New Process (Manual Entry)\n";
        std::cout << "2. 🎲 Generate Random Processes\n";
        std::cout << "3. ✏️  Edit Existing Process\n";
        std::cout << "4. 🗑️  Delete Process\n";
        std::cout << "5. 🧹 Clear All Processes\n";
        std::cout << "6. 🚀 Run Scheduling Algorithms\n";
        std::cout << "7. 📊 Show Detailed Process Info\n";
        std::cout << "8. ↩️  Return to Main Menu\n";
        std::cout << "Enter your choice (1-8): ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "❌ Invalid input! Please enter a number 1-8.\n";
            continue;
        }
        
        switch (choice) {
            case 1:
                process_scheduler.addManualProcess();
                break;
                
            case 2:
                process_scheduler.generateRandomProcessesInteractive();
                break;
                
            case 3: {
                int pid;
                std::cout << "Enter Process ID to edit: ";
                if (std::cin >> pid) {
                    process_scheduler.editProcess(pid);
                } else {
                    std::cout << "❌ Invalid Process ID!\n";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                break;
            }
                
            case 4: {
                int pid;
                std::cout << "Enter Process ID to delete: ";
                if (std::cin >> pid) {
                    process_scheduler.deleteProcess(pid);
                } else {
                    std::cout << "❌ Invalid Process ID!\n";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                break;
            }
                
            case 5:
                process_scheduler.clearAllProcesses();
                break;
                
            case 6:
                if (process_scheduler.getProcesses().empty()) {
                    std::cout << "❌ No processes to schedule! Please add processes first.\n";
                } else {
                    // Run all scheduling algorithms on current processes
                    std::cout << "\n" << std::string(50, '=') << "\n";
                    std::cout << "🚀 RUNNING SCHEDULING ALGORITHMS\n";
                    std::cout << std::string(50, '=') << "\n";
                    
                    // FCFS
                    std::cout << "\n--- FCFS (First-Come, First-Served) ---\n";
                    ProcessScheduler temp_scheduler;
                    for (const auto& proc : process_scheduler.getProcesses()) {
                        temp_scheduler.addProcess(proc);
                    }
                    temp_scheduler.executeScheduler("FCFS");
                    temp_scheduler.displayVisualResults();
                    
                    // SJF
                    std::cout << "\n--- SJF (Shortest Job First) ---\n";
                    temp_scheduler.resetScheduler();
                    for (const auto& proc : process_scheduler.getProcesses()) {
                        temp_scheduler.addProcess(proc);
                    }
                    temp_scheduler.executeScheduler("SJF");
                    temp_scheduler.displayVisualResults();
                    
                    // Round Robin
                    std::cout << "\n--- Round Robin (Time Quantum = 3) ---\n";
                    temp_scheduler.resetScheduler();
                    for (const auto& proc : process_scheduler.getProcesses()) {
                        temp_scheduler.addProcess(proc);
                    }
                    temp_scheduler.executeScheduler("RR", 3);
                    temp_scheduler.displayVisualResults();
                    
                    // Priority
                    std::cout << "\n--- Priority Scheduling ---\n";
                    temp_scheduler.resetScheduler();
                    for (const auto& proc : process_scheduler.getProcesses()) {
                        temp_scheduler.addProcess(proc);
                    }
                    temp_scheduler.executeScheduler("PRIORITY");
                    temp_scheduler.displayVisualResults();
                }
                break;
                
            case 7:
                process_scheduler.displayCurrentProcesses();
                break;
                
            case 8:
                management_mode = false;
                std::cout << "↩️  Returning to main menu...\n";
                break;
                
            default:
                std::cout << "❌ Invalid choice! Please enter 1-8.\n";
        }
        
        if (management_mode && choice != 8) {
            std::cout << "\nPress Enter to continue...";
            std::cin.ignore();
            std::cin.get();
        }
    }
}

// ===== EXISTING DEMO FUNCTIONS =====

void run_process_scheduler_demo() {
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "PROCESS SCHEDULING DEMONSTRATION WITH GANTT CHARTS\n";
    std::cout << std::string(50, '=') << "\n";
    
    // Create sample processes with names
    std::vector<Process> sample_processes = {
        Process(1, "Web Browser", 0, 6, 3),
        Process(2, "Text Editor", 2, 4, 1),
        Process(3, "Media Player", 4, 8, 4),
        Process(4, "File Manager", 6, 5, 2),
        Process(5, "Terminal", 8, 3, 1)
    };
    
    // FCFS Demo
    std::cout << "\n--- FCFS (First-Come, First-Served) ---\n";
    for (const auto& proc : sample_processes) {
        process_scheduler.addProcess(proc);
    }
    process_scheduler.executeScheduler("FCFS");
    process_scheduler.displayVisualResults();
    process_scheduler.resetScheduler();
    
    // SJF Demo  
    std::cout << "\n--- SJF (Shortest Job First) ---\n";
    for (const auto& proc : sample_processes) {
        process_scheduler.addProcess(proc);
    }
    process_scheduler.executeScheduler("SJF");
    process_scheduler.displayVisualResults();
    process_scheduler.resetScheduler();
    
    // Round Robin Demo
    std::cout << "\n--- Round Robin (Time Quantum = 3) ---\n";
    for (const auto& proc : sample_processes) {
        process_scheduler.addProcess(proc);
    }
    process_scheduler.executeScheduler("RR", 3);
    process_scheduler.displayVisualResults();
    process_scheduler.resetScheduler();
    
    // Priority Scheduling Demo
    std::cout << "\n--- Priority Scheduling ---\n";
    for (const auto& proc : sample_processes) {
        process_scheduler.addProcess(proc);
    }
    process_scheduler.executeScheduler("PRIORITY");
    process_scheduler.displayVisualResults();
    process_scheduler.resetScheduler();
}

void run_file_system_demo() {
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "FILE SYSTEM DEMONSTRATION\n";
    std::cout << std::string(50, '=') << "\n";
    
    // Create directory structure
    std::cout << "Creating directory structure...\n";
    file_system.createDirectory("/home");
    file_system.createDirectory("/home/user");
    file_system.createDirectory("/home/system");
    file_system.createDirectory("/tmp");
    file_system.createDirectory("/var");
    file_system.createDirectory("/var/log");
    
    // Create and write files
    std::cout << "\n--- Creating and Writing Files ---\n";
    file_system.createFile("/home/user/document.txt");
    file_system.writeFile("/home/user/document.txt", "This is a sample document with important data.");
    
    file_system.createFile("/home/user/notes.md");
    file_system.writeFile("/home/user/notes.md", "# Project Notes\n- Task 1: Complete OS project\n- Task 2: Add more features");
    
    file_system.createFile("/tmp/temp_data.bin");
    file_system.writeFile("/tmp/temp_data.bin", "Temporary binary data stored here for processing.");
    
    file_system.createFile("/var/log/system.log");
    file_system.writeFile("/var/log/system.log", "2024-01-15 10:30:45 - System started\n2024-01-15 10:31:12 - User login");
    
    // List directories
    std::cout << "\n--- Directory Listing ---\n";
    file_system.listDirectory("/");
    std::cout << "\n";
    file_system.listDirectory("/home");
    std::cout << "\n";
    file_system.listDirectory("/home/user");
    
    // Read file content
    std::cout << "\n--- Reading Files ---\n";
    std::string content = file_system.readFile("/home/user/document.txt");
    std::cout << "File content: " << content << "\n";
    
    // Display file system information
    std::cout << "\n--- File System Information ---\n";
    file_system.displayFileSystemInfo();
    
    // Display inode information
    std::cout << "\n--- Inode Information ---\n";
    file_system.displayInodeInfo(1); // Assuming first file has inode 1
    
    // Consistency check
    std::cout << "\n--- File System Consistency Check ---\n";
    file_system.checkConsistency();
    
    // Cleanup demo
    std::cout << "\n--- Cleanup ---\n";
    file_system.deleteFile("/tmp/temp_data.bin");
    file_system.listDirectory("/tmp");
}

void run_ipc_demo() {
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "INTER-PROCESS COMMUNICATION DEMONSTRATION\n";
    std::cout << std::string(50, '=') << "\n";
    
    int choice;
    bool ipc_mode = true;
    
    while (ipc_mode) {
        std::cout << "\n" << std::string(50, '-') << "\n";
        std::cout << "IPC INTERACTIVE MENU\n";
        std::cout << std::string(50, '-') << "\n";
        ipc_manager.displayIPCStatus();
        std::cout << std::string(50, '-') << "\n";
        
        std::cout << "1. 📨 Send Custom Message\n";
        std::cout << "2. 📥 View All Messages\n";
        std::cout << "3. 🗑️ Clear Message Queue\n";
        std::cout << "4. 💾 Create Shared Memory\n";
        std::cout << "5. 👀 View Shared Memory Content\n";
        std::cout << "6. ✏️ Write to Shared Memory\n";
        std::cout << "7. 🔒 Create Named Mutex\n";
        std::cout << "8. 🆕 Create Message Queue\n";
        std::cout << "9. 🚀 Run Automatic Demo\n";
        std::cout << "10. ↩️ Return to Main Menu\n";
        std::cout << "Enter your choice (1-10): ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "❌ Invalid input! Please enter a number 1-10.\n";
            continue;
        }
        
        switch (choice) {
            case 1:
                ipc_manager.interactiveSendMessage();
                break;
                
            case 2:
                ipc_manager.viewAllMessages();
                break;
                
            case 3: {
                int queue_id;
                std::cout << "Enter Queue ID to clear: ";
                if (std::cin >> queue_id) {
                    ipc_manager.clearMessageQueue(queue_id);
                } else {
                    std::cout << "❌ Invalid Queue ID!\n";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
                break;
            }
                
            case 4:
                ipc_manager.interactiveCreateSharedMemory();
                break;
                
            case 5: {
                std::string name;
                std::cout << "Enter Shared Memory Name: ";
                std::cin.ignore();
                std::getline(std::cin, name);
                ipc_manager.viewSharedMemoryContent(name);
                break;
            }
                
            case 6: {
                std::string name, data;
                std::cout << "Enter Shared Memory Name: ";
                std::cin.ignore();
                std::getline(std::cin, name);
                std::cout << "Enter Data to Write: ";
                std::getline(std::cin, data);
                ipc_manager.writeToSharedMemory(name, data);
                break;
            }
                
            case 7: {
                std::string mutex_name;
                std::cout << "Enter Mutex Name: ";
                std::cin.ignore();
                std::getline(std::cin, mutex_name);
                ipc_manager.createNamedMutex(mutex_name);
                break;
            }
                
            case 8: {
                int queue_id, max_size;
                std::cout << "Enter Queue ID: ";
                if (!(std::cin >> queue_id)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "❌ Invalid Queue ID!\n";
                    break;
                }
                std::cout << "Enter Maximum Size: ";
                if (!(std::cin >> max_size) || max_size <= 0) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "❌ Invalid size! Using default 100.\n";
                    max_size = 100;
                }
                ipc_manager.createMessageQueue(queue_id, max_size);
                break;
            }
                
            case 9:
                run_automatic_ipc_demo();
                break;
                
            case 10:
                ipc_mode = false;
                std::cout << "↩️ Returning to main menu...\n";
                break;
                
            default:
                std::cout << "❌ Invalid choice! Please enter 1-10.\n";
        }
        
        if (ipc_mode && choice != 10) {
            std::cout << "\nPress Enter to continue...";
            std::cin.ignore();
            std::cin.get();
        }
    }
}

// Keep the original automatic demo as a separate function
void run_automatic_ipc_demo() {
    std::cout << "\n--- Running Automatic IPC Demo ---\n";
    
    // Create message queues
    std::cout << "Creating message queues...\n";
    ipc_manager.createMessageQueue(1, 10);
    ipc_manager.createMessageQueue(2, 5);
    
    // Send messages
    std::cout << "\n--- Sending Messages ---\n";
    ipc_manager.sendMessage(1, 100, 200, "Hello from Process 100!");
    ipc_manager.sendMessage(1, 101, 200, "System update available");
    ipc_manager.sendMessage(1, 102, 300, "Data processing complete");
    ipc_manager.sendMessage(2, 400, 500, "High priority alert!");
    
    // Shared memory demonstration
    std::cout << "\n--- Shared Memory ---\n";
    void* shared_buffer = ipc_manager.createSharedMemory("buffer1", 1024);
    if (shared_buffer) {
        ipc_manager.writeToSharedMemory("buffer1", "Initial shared memory data");
        ipc_manager.viewSharedMemoryContent("buffer1");
    }
    
    // Named mutex demonstration
    std::cout << "\n--- Named Mutex ---\n";
    ipc_manager.createNamedMutex("resource_lock");
    ipc_manager.lockNamedMutex("resource_lock");
    std::cout << "Critical section protected by mutex\n";
    ipc_manager.unlockNamedMutex("resource_lock");
    
    // Display IPC status
    std::cout << "\n--- IPC System Status ---\n";
    ipc_manager.displayIPCStatus();
}

    
 void run_deadlock_detection_demo() {
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "DEADLOCK DETECTION DEMONSTRATION\n";
    std::cout << std::string(50, '=') << "\n";
    
    // Display initial state
    std::cout << "Initial System State:\n";
    deadlock_detector.displaySystemState();
    
    // Simulate normal resource allocation
    std::cout << "\n--- Normal Resource Allocation ---\n";
    deadlock_detector.requestResource(1, 1, 2); // Process 1 requests 2 printers
    deadlock_detector.requestResource(2, 2, 1); // Process 2 requests 1 scanner
    deadlock_detector.requestResource(3, 3, 3); // Process 3 requests 3 modems
    
    deadlock_detector.displaySystemState();
    deadlock_detector.displayResourceAllocation(); // This should work now
    
    // Simulate deadlock scenario
    std::cout << "\n--- Deadlock Scenario Simulation ---\n";
    deadlock_detector.simulateDeadlockScenario();
    
    // Recovery from deadlock
    std::cout << "\n--- Deadlock Recovery ---\n";
    deadlock_detector.recoverFromDeadlock();
    
    // Final state
    std::cout << "\n--- Final System State ---\n";
    deadlock_detector.displaySystemState();
    
    // Show wait-for graph visualization
    std::cout << "\n--- Wait-For Graph Visualization ---\n";
    deadlock_detector.visualizeWaitForGraph();
}

void run_comprehensive_os_demo() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "COMPREHENSIVE OPERATING SYSTEM DEMONSTRATION\n";
    std::cout << std::string(60, '=') << "\n";
    
    run_process_scheduler_demo();
    run_file_system_demo();
    run_ipc_demo();
    run_deadlock_detection_demo();
    
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "ALL OS DEMONSTRATIONS COMPLETED SUCCESSFULLY!\n";
    std::cout << std::string(60, '=') << "\n";
}
