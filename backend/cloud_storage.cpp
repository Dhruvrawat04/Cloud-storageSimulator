#include "cloud.h"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <iterator>
#include <filesystem>
#include <vector>
#include <random>
#include <ctime>
#include <sstream>
#include <chrono>

// Existing global definitions
std::string cloudData = "InitialFile";
pthread_mutex_t rw_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_readcount = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
int read_count = 0;

// NEW: Timing-related global definitions
pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;
int total_operations = 0;
int active_readers = 0;
int active_writers = 0;
int active_deleters = 0;
int completed_reads = 0;
int completed_writes = 0;
int completed_deletes = 0;
double total_read_time = 0.0;
double total_write_time = 0.0;
double total_delete_time = 0.0;
std::chrono::steady_clock::time_point system_start_time;

// Global statistics definitions
std::map<std::string, OperationStats> global_stats;
std::vector<OperationTiming> detailed_timings;

// ===== TIMING UTILITY FUNCTIONS =====

std::chrono::high_resolution_clock::time_point get_current_time() {
    return std::chrono::high_resolution_clock::now();
}

long long get_microseconds_since(const std::chrono::high_resolution_clock::time_point& start) {
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

double get_elapsed_time_ms(std::chrono::steady_clock::time_point start) {
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    return duration.count() / 1000.0; // Convert to milliseconds
}

std::string format_duration(double ms) {
    if (ms < 1000) {
        return std::to_string((int)ms) + "ms";
    } else if (ms < 60000) {
        return std::to_string(ms / 1000.0).substr(0, 5) + "s";
    } else {
        int minutes = (int)(ms / 60000);
        double seconds = (ms - minutes * 60000) / 1000.0;
        return std::to_string(minutes) + "m " + std::to_string(seconds).substr(0, 4) + "s";
    }
}

std::string getCurrentTimestamp() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string getCurrentTimestampMicro() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(
        now.time_since_epoch()) % 1000000;
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(6) << ms.count();
    return oss.str();
}

// ===== LOGGING FUNCTIONS =====

// Keep existing log_event function for backward compatibility
void log_event(int thread_id, const std::string& action, const std::string& status) {
    pthread_mutex_lock(&log_mutex);
    
    std::string timestamp = getCurrentTimestamp();
    std::string thread_type;
    
    if (action == "READ") thread_type = "READER";
    else if (action == "WRITE") thread_type = "WRITER";
    else if (action == "DELETE") thread_type = "DELETER";
    else thread_type = "MAIN";
    
    std::string log_entry = "[" + timestamp + "] [" + thread_type + "#" + std::to_string(thread_id) + "] " + action + " " + status;
    
    // Log to console
    std::cout << log_entry << std::endl;
    
    // Log to file
    std::ofstream log_file("./logs/simulation.log", std::ios::app);
    if (log_file) {
        log_file << log_entry << std::endl;
        log_file.close();
    }
    
    pthread_mutex_unlock(&log_mutex);
}

void log_real_time_status(const std::string& message) {
    pthread_mutex_lock(&log_mutex);
    std::string timestamp = getCurrentTimestampMicro();
    std::string log_entry = "[" + timestamp + "] [REAL-TIME] " + message;
    
    std::cout << log_entry << std::endl;
    
    std::ofstream log_file("./logs/realtime_status.log", std::ios::app);
    if (log_file) {
        log_file << log_entry << std::endl;
        log_file.close();
    }
    pthread_mutex_unlock(&log_mutex);
}

void log_timing_event(int thread_id, const std::string& action, const OperationTiming& timing) {
    pthread_mutex_lock(&log_mutex);
    
    std::string timestamp = getCurrentTimestampMicro();
    std::string thread_type;
    
    if (action == "READ") thread_type = "READER";
    else if (action == "WRITE") thread_type = "WRITER";
    else if (action == "DELETE") thread_type = "DELETER";
    else thread_type = "MAIN";
    
    std::string log_entry = "[" + timestamp + "] [" + thread_type + "#" + 
                           std::to_string(thread_id) + "] " + action + 
                           " - Wait: " + std::to_string(timing.wait_time_us) + "μs, " +
                           "Operation: " + std::to_string(timing.operation_time_us) + "μs, " +
                           "Total: " + std::to_string(timing.total_time_us) + "μs";
    
    std::ofstream perf_file("./logs/performance.log", std::ios::app);
    if (perf_file) {
        perf_file << log_entry << std::endl;
        perf_file.close();
    }
    pthread_mutex_unlock(&log_mutex);
}

// Overloaded version for simple logging
void log_timing_event(int thread_id, const std::string& action, const std::string& status, double duration_ms) {
    pthread_mutex_lock(&log_mutex);
    
    std::string timestamp = getCurrentTimestamp();
    std::string thread_type;
    
    if (action == "READ") thread_type = "READER";
    else if (action == "WRITE") thread_type = "WRITER";
    else if (action == "DELETE") thread_type = "DELETER";
    else thread_type = "MAIN";
    
    std::string log_entry = "[" + timestamp + "] [" + thread_type + "#" + std::to_string(thread_id) + "] " + action + " " + status;
    
    if (duration_ms >= 0) {
        log_entry += " (took: " + format_duration(duration_ms) + ")";
    }
    
    // Log to console
    std::cout << log_entry << std::endl;
    
    // Log to file
    std::ofstream log_file("./logs/simulation.log", std::ios::app);
    if (log_file) {
        log_file << log_entry << std::endl;
        log_file.close();
    }
    
    pthread_mutex_unlock(&log_mutex);
}

// ===== TIMING SYSTEM FUNCTIONS =====

// NEW: Initialize timing system
void initialize_timing_system() {
    system_start_time = std::chrono::steady_clock::now();
    
    pthread_mutex_lock(&stats_mutex);
    total_operations = 0;
    active_readers = active_writers = active_deleters = 0;
    completed_reads = completed_writes = completed_deletes = 0;
    total_read_time = total_write_time = total_delete_time = 0.0;
    pthread_mutex_unlock(&stats_mutex);
    
    std::cout << "\n🕐 TIMING SYSTEM INITIALIZED\n";
    std::cout << "System start time: " << getCurrentTimestamp() << "\n";
    log_timing_event(0, "SYSTEM", "TIMING_INITIALIZED", 0);
}

// NEW: Cleanup timing system
void cleanup_timing_system() {
    auto system_end = std::chrono::steady_clock::now();
    double total_system_time = get_elapsed_time_ms(system_start_time);
    
    std::cout << "\n🕐 TIMING SYSTEM CLEANUP\n";
    std::cout << "Total system runtime: " << format_duration(total_system_time) << "\n";
    log_timing_event(0, "SYSTEM", "TIMING_CLEANUP", total_system_time);
}

// NEW: Update operation statistics
void update_operation_stats(const std::string& operation, double duration, bool started) {
    pthread_mutex_lock(&stats_mutex);
    
    if (operation == "READ") {
        if (started) {
            active_readers++;
        } else {
            active_readers--;
            completed_reads++;
            total_read_time += duration;
        }
    } else if (operation == "WRITE") {
        if (started) {
            active_writers++;
        } else {
            active_writers--;
            completed_writes++;
            total_write_time += duration;
        }
    } else if (operation == "DELETE") {
        if (started) {
            active_deleters++;
        } else {
            active_deleters--;
            completed_deletes++;
            total_delete_time += duration;
        }
    }
    
    if (!started) {
        total_operations++;
    }
    
    pthread_mutex_unlock(&stats_mutex);
}

// ===== STATISTICS FUNCTIONS =====

void update_statistics(const std::string& operation, const OperationTiming& timing) {
    pthread_mutex_lock(&stats_mutex);
    global_stats[operation].add_timing(timing);
    detailed_timings.push_back(timing);
    pthread_mutex_unlock(&stats_mutex);
}

void print_performance_report() {
    pthread_mutex_lock(&stats_mutex);
    
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "📊 PERFORMANCE ANALYSIS REPORT\n";
    std::cout << std::string(60, '=') << "\n";
    
    for (const auto& [operation, stats] : global_stats) {
        if (stats.count > 0) {
            std::cout << "\n" << operation << " OPERATIONS:\n";
            std::cout << "  Count: " << stats.count << "\n";
            std::cout << "  Average Wait Time: " << stats.get_avg_wait_time() << "μs\n";
            std::cout << "  Average Operation Time: " << stats.get_avg_operation_time() << "μs\n";
            std::cout << "  Average Total Time: " << stats.get_avg_total_time() << "μs\n";
            std::cout << "  Min Time: " << stats.min_time_us << "μs\n";
            std::cout << "  Max Time: " << stats.max_time_us << "μs\n";
        }
    }
    
    std::cout << std::string(60, '=') << "\n";
    pthread_mutex_unlock(&stats_mutex);
}

void reset_statistics() {
    pthread_mutex_lock(&stats_mutex);
    global_stats.clear();
    detailed_timings.clear();
    pthread_mutex_unlock(&stats_mutex);
}

// ===== UTILITY FUNCTIONS =====

void ensure_directories_exist() {
    std::filesystem::create_directories("./test_files/");
    std::filesystem::create_directories("./downloads/");
    std::filesystem::create_directories("./logs/");
}

void show_directory_structure() {
    std::cout << "\n=== DIRECTORY STRUCTURE ===\n";
    for (const auto& entry : std::filesystem::recursive_directory_iterator(".")) {
        if (entry.is_directory()) {
            std::cout << "[DIR]  " << entry.path().string() << "\n";
        } else {
            auto size = std::filesystem::file_size(entry);
            std::cout << "[FILE] " << entry.path().string() 
                      << " (" << size << " bytes)\n";
        }
    }
}

std::string getRandomTestFile() {
    std::vector<std::string> test_files;

    // Create test_files directory if it doesn't exist
    if (!std::filesystem::exists("./test_files/")) {
        std::filesystem::create_directory("./test_files/");
        
        // Create sample files if directory is empty
        std::ofstream customer("./test_files/customer_data.txt");
        customer << "Customer Database\n";
        customer << "=================\n";
        customer << "ID: 001, Name: John Doe, Email: john@example.com\n";
        customer << "ID: 002, Name: Jane Smith, Email: jane@example.com\n";
        customer << "ID: 003, Name: Bob Johnson, Email: bob@example.com\n";
        customer.close();
        
        std::ofstream product("./test_files/product_list.txt");
        product << "Product Inventory\n";
        product << "=================\n";
        product << "SKU: ABC123, Product: Laptop, Price: $999.99, Stock: 50\n";
        product << "SKU: DEF456, Product: Mouse, Price: $29.99, Stock: 200\n";
        product << "SKU: GHI789, Product: Keyboard, Price: $79.99, Stock: 150\n";
        product.close();

        std::ofstream logfile("./test_files/logfile.txt");
        logfile << "System Log File\n";
        logfile << "===============\n";
        logfile << "2024-01-15 09:30:15 - System started\n";
        logfile << "2024-01-15 09:35:22 - User login: admin\n";
        logfile << "2024-01-15 10:15:33 - Database backup completed\n";
        logfile << "2024-01-15 11:42:18 - Warning: Low disk space\n";
        logfile.close();
    }

    // Scan for files in test_files directory
    for (const auto& entry : std::filesystem::directory_iterator("./test_files/")) {
        if (entry.is_regular_file()) {
            test_files.push_back(entry.path().string());
        }
    }

    if (test_files.empty()) {
        return "./test_files/default.txt"; // fallback
    }

    // Return random file
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, test_files.size() - 1);
    
    return test_files[dis(gen)];
}

// ===== FILE OPERATION FUNCTIONS =====


// Enhanced uploadFile with better error handling and synchronization
void uploadFile(const std::string& filename) {
    auto start_time = std::chrono::steady_clock::now();
    
    // Input validation
    if (filename.empty()) {
        std::cout << "Error: Empty filename provided for upload\n";
        log_timing_event(0, "UPLOAD", "ERROR (empty filename)", 0);
        return;
    }
    
    // Check if file exists
    if (!std::filesystem::exists(filename)) {
        std::cout << "Error: File '" << filename << "' does not exist\n";
        log_timing_event(0, "UPLOAD", "ERROR (file not found: '" + filename + "')", 0);
        return;
    }
    
    // Check file size limits (10MB limit)
    auto file_size = std::filesystem::file_size(filename);
    if (file_size > 10 * 1024 * 1024) { // 10MB
        std::cout << "Error: File '" << filename << "' too large (" << file_size << " bytes)\n";
        log_timing_event(0, "UPLOAD", "ERROR (file too large: " + std::to_string(file_size) + " bytes)", 0);
        return;
    }

    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (!in) {
        std::cout << "Error: cannot open '" << filename << "' to upload\n";
        log_timing_event(0, "UPLOAD", "ERROR (cannot open '" + filename + "')", 0);
        return;
    }

    std::string content((std::istreambuf_iterator<char>(in)),
                       std::istreambuf_iterator<char>());
    in.close();

    // CRITICAL: Proper synchronization for cloudData access
    pthread_mutex_lock(&rw_mutex);
    cloudData = content;
    pthread_mutex_unlock(&rw_mutex);
    
    double duration = get_elapsed_time_ms(start_time);
    
    std::cout << "[UPLOAD] Uploaded '" << filename << "' to cloud (size: " << cloudData.size() << " bytes)\n";
    log_timing_event(0, "UPLOAD", "SUCCESS \"" + filename + "\" (size: " + std::to_string(cloudData.size()) + " bytes)", duration);
}

// Enhanced downloadFile with better synchronization
void downloadFile(const std::string& filename) {
    auto start_time = std::chrono::steady_clock::now();
    
    // Input validation
    if (filename.empty()) {
        std::cout << "Error: Empty filename provided for download\n";
        log_timing_event(0, "DOWNLOAD", "ERROR (empty filename)", 0);
        return;
    }
    
    // CRITICAL: Proper synchronization for cloudData access
    std::string content;
    pthread_mutex_lock(&rw_mutex);
    content = cloudData; // Copy while holding the lock
    pthread_mutex_unlock(&rw_mutex);
    
    std::ofstream out(filename, std::ios::out | std::ios::binary);
    if (!out) {
        std::cout << "Error: cannot open '" << filename << "' to write download\n";
        log_timing_event(0, "DOWNLOAD", "ERROR (cannot open '" + filename + "')", 0);
        return;
    }

    out << content;
    out.close();

    // Verify file was written
    if (!std::filesystem::exists(filename)) {
        std::cout << "Error: Download file creation failed for '" << filename << "'\n";
        log_timing_event(0, "DOWNLOAD", "ERROR (file creation failed: '" + filename + "')", 0);
        return;
    }

    double duration = get_elapsed_time_ms(start_time);
    
    std::cout << "[DOWNLOAD] Saved cloud content to '" << filename << "' (size: " << content.size() << " bytes)\n";
    log_timing_event(0, "DOWNLOAD", "SUCCESS \"" + filename + "\" (size: " + std::to_string(content.size()) + " bytes)", duration);
}

// Stress test function - spawns multiple threads to test the system
void run_stress_test(int num_threads) {
    std::cout << "\n=== Starting Stress Test with " << num_threads << " threads ===\n" << std::endl;
    log_event(0, "STRESS_TEST", "Starting with " + std::to_string(num_threads) + " threads");
    
    std::vector<pthread_t> threads;
    std::vector<int*> thread_ids;
    
    // Create a mix of readers, writers, and deleters
    for (int i = 0; i < num_threads; i++) {
        pthread_t thread;
        int* tid = new int(i + 1);
        thread_ids.push_back(tid);
        
        // Distribute thread types: 50% readers, 30% writers, 20% deleters
        int type = i % 10;
        if (type < 5) {
            // Reader
            pthread_create(&thread, nullptr, reader, tid);
        } else if (type < 8) {
            // Writer
            pthread_create(&thread, nullptr, writer, tid);
        } else {
            // Deleter
            pthread_create(&thread, nullptr, deleter, tid);
        }
        
        threads.push_back(thread);
    }
    
    // Wait for all threads to complete
    for (pthread_t thread : threads) {
        pthread_join(thread, nullptr);
    }
    
    // Cleanup
    for (int* tid : thread_ids) {
        delete tid;
    }
    
    std::cout << "\n=== Stress Test Completed ===\n" << std::endl;
    log_event(0, "STRESS_TEST", "Completed successfully");
    print_performance_report();
}