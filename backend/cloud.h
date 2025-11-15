#ifndef CLOUD_H
#define CLOUD_H

#include <climits>
#include <string>
#include <pthread.h>
#include <chrono>
#include <vector>
#include <map>

// Timing structure for microsecond precision
struct OperationTiming {
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point lock_acquired_time;
    std::chrono::high_resolution_clock::time_point operation_complete_time;
    std::chrono::high_resolution_clock::time_point end_time;
    
    // Calculated durations in microseconds
    long long wait_time_us = 0;
    long long operation_time_us = 0;
    long long total_time_us = 0;
    
    void calculate_durations() {
        wait_time_us = std::chrono::duration_cast<std::chrono::microseconds>(
            lock_acquired_time - start_time).count();
        operation_time_us = std::chrono::duration_cast<std::chrono::microseconds>(
            operation_complete_time - lock_acquired_time).count();
        total_time_us = std::chrono::duration_cast<std::chrono::microseconds>(
            end_time - start_time).count();
    }
};

// Statistics structure
struct OperationStats {
    int count = 0;
    long long total_wait_time_us = 0;
    long long total_operation_time_us = 0;
    long long total_time_us = 0;
    long long min_time_us = LLONG_MAX;
    long long max_time_us = 0;
    
    void add_timing(const OperationTiming& timing) {
        count++;
        total_wait_time_us += timing.wait_time_us;
        total_operation_time_us += timing.operation_time_us;
        total_time_us += timing.total_time_us;
        
        if (timing.total_time_us < min_time_us) min_time_us = timing.total_time_us;
        if (timing.total_time_us > max_time_us) max_time_us = timing.total_time_us;
    }
    
    double get_avg_wait_time() const { return count > 0 ? (double)total_wait_time_us / count : 0.0; }
    double get_avg_operation_time() const { return count > 0 ? (double)total_operation_time_us / count : 0.0; }
    double get_avg_total_time() const { return count > 0 ? (double)total_time_us / count : 0.0; }
};

// Global variables
extern std::string cloudData;
extern pthread_mutex_t rw_mutex;
extern pthread_mutex_t mutex_readcount;
extern pthread_mutex_t log_mutex;
extern pthread_mutex_t stats_mutex;
extern int read_count;

// Global statistics
extern std::map<std::string, OperationStats> global_stats;
extern std::vector<OperationTiming> detailed_timings;

// Runtime counters (defined in cloud_storage.cpp)
extern int total_operations;
extern int active_readers;
extern int active_writers;
extern int active_deleters;
extern int completed_reads;
extern int completed_writes;
extern int completed_deletes;

// Thread function prototypes
void* reader(void* arg);
void* writer(void* arg);
void* deleter(void* arg);

// File operation functions
void uploadFile(const std::string& filename);
void downloadFile(const std::string& filename);

// Logging functions
void log_event(int thread_id, const std::string& action, const std::string& status);
void log_timing_event(int thread_id, const std::string& action, const OperationTiming& timing);
void log_real_time_status(const std::string& message);

// Statistics functions
void update_statistics(const std::string& operation, const OperationTiming& timing);
void print_performance_report();
void reset_statistics();

// Statistics/timing control (implemented in cloud_storage.cpp)
void update_operation_stats(const std::string& operation, double duration, bool started);
void initialize_timing_system();
void cleanup_timing_system();

// Utility functions
std::string getRandomTestFile();
std::string getCurrentTimestamp();
std::string getCurrentTimestampMicro();
void ensure_directories_exist();
void show_directory_structure();

// Stress test function
void run_stress_test(int num_threads);

// Advanced timing utilities
std::chrono::high_resolution_clock::time_point get_current_time();
long long get_microseconds_since(const std::chrono::high_resolution_clock::time_point& start);


void run_cloud_simulator();
void run_stress_test(int num_threads);
void print_performance_report();

// Cloud integration functions
void run_cloud_storage_demo();
void run_cloud_stress_test();
void show_cloud_performance_report();
#endif // CLOUD_H
