
#include "cloud.h"
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <filesystem>
#include <random>
#include <thread>
#include <chrono>
#include <cerrno> 
// Enhanced Reader with microsecond-precision timing
void* reader(void* arg) {
    int id = *((int*)arg);
    OperationTiming timing;
    timing.start_time = get_current_time();
    update_operation_stats("READ", 0, true);
    log_event(id, "READ", "STARTED");
    log_real_time_status("Reader #" + std::to_string(id) + " attempting to acquire read lock");

    pthread_mutex_lock(&mutex_readcount);
    read_count++;
    if (read_count == 1) { // first reader blocks writers
        pthread_mutex_lock(&rw_mutex);
        log_real_time_status("Reader #" + std::to_string(id) + " acquired exclusive access (first reader)");

    }

    pthread_mutex_unlock(&mutex_readcount);
    timing.lock_acquired_time = get_current_time();

    log_real_time_status("Reader #" + std::to_string(id) + " lock acquired after " + 

                        std::to_string(timing.wait_time_us = get_microseconds_since(timing.start_time)) + "μs");


    // Ensure downloads directory exists
    ensure_directories_exist();
    // Read cloudData and save to file with enhanced error handling

    std::string content = cloudData;

    std::string download_filename = "./downloads/download_reader_" + std::to_string(id) + 

                                   "_" + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(

                                   std::chrono::system_clock::now().time_since_epoch()).count()) + ".txt";

    

    std::cout << "[Reader " << id << "] reading (size " << content.size() << "): ";

    if (content.size() > 80) {

        std::cout << content.substr(0, 80) << "...";

    } else {

        std::cout << content;

    }

    std::cout << " [Wait: " << timing.wait_time_us << "μs]" << std::endl;

    

    // Simulate realistic read processing time

    std::this_thread::sleep_for(std::chrono::milliseconds(100 + (id % 50))); // Variable delay 100-150ms
  timing.operation_complete_time = get_current_time();

  // Write to download file with enhanced error handling
   std::ofstream out(download_filename);
    if (out) {
        // Write metadata header
        out << "=== CLOUD DOWNLOAD METADATA ===\n";
        out << "Downloaded by: Reader #" << id << "\n";
        out << "Download time: " << getCurrentTimestampMicro() << "\n";
        out << "Content size: " << content.size() << " bytes\n";
        out << "Processing time: " << get_microseconds_since(timing.lock_acquired_time) << " microseconds\n";
        out << "================================\n\n";
        out << content;
        out.close();

        

        // Verify file was written correctly

        if (std::filesystem::exists(download_filename)) {

            auto file_size = std::filesystem::file_size(download_filename);

            log_event(id, "READ", "SUCCESS (saved to " + download_filename + 

                     ", file size: " + std::to_string(file_size) + " bytes)");

        } else {

            log_event(id, "READ", "WARNING (file created but verification failed)");

        }

    } else {

        log_event(id, "READ", "ERROR (failed to create " + download_filename + ")");

    }

    

    timing.end_time = get_current_time();

    timing.calculate_durations();

    

    // Release read lock

    pthread_mutex_lock(&mutex_readcount);

    read_count--;

    if (read_count == 0) { // last reader unblocks writers

        pthread_mutex_unlock(&rw_mutex);

        log_real_time_status("Reader #" + std::to_string(id) + " released exclusive access (last reader)");

    }

    pthread_mutex_unlock(&mutex_readcount);

    

    // Log detailed timing information

    log_timing_event(id, "READ", timing);

    update_statistics("READ", timing);

    update_operation_stats("READ", timing.total_time_us, false);

    

    log_event(id, "READ", "COMPLETED (total time: " + std::to_string(timing.total_time_us) + "μs)");

    return nullptr;

}



// Enhanced Writer with microsecond-precision timing and real file operations

void* writer(void* arg) {

    int id = *((int*)arg);

    OperationTiming timing;

    timing.start_time = get_current_time();

    update_operation_stats("WRITE", 0, true);

    

    log_event(id, "WRITE", "STARTED");

    log_real_time_status("Writer #" + std::to_string(id) + " attempting to acquire write lock");

    

    pthread_mutex_lock(&rw_mutex);

    timing.lock_acquired_time = get_current_time();

    timing.wait_time_us = get_microseconds_since(timing.start_time);

    

    log_real_time_status("Writer #" + std::to_string(id) + " acquired exclusive access after " + 

                        std::to_string(timing.wait_time_us) + "μs");

    

    size_t prev_size = cloudData.size();

    std::string test_file = getRandomTestFile();

    

    std::cout << "[Writer " << id << "] uploading from '" << test_file 

              << "'... (prev size: " << prev_size << ") [Wait: " << timing.wait_time_us << "μs]\n";

    

    // Read from test file with enhanced error handling

    std::ifstream in(test_file);

    if (in) {

        std::string content((std::istreambuf_iterator<char>(in)),

                           std::istreambuf_iterator<char>());

        in.close();

        

        // Add writer metadata to content

        std::ostringstream enhanced_content;

        enhanced_content << "=== UPLOAD METADATA ===\n";

        enhanced_content << "Uploaded by: Writer #" << id << "\n";

        enhanced_content << "Upload time: " << getCurrentTimestampMicro() << "\n";

        enhanced_content << "Source file: " << test_file << "\n";

        enhanced_content << "Original size: " << content.size() << " bytes\n";

        enhanced_content << "=======================\n\n";

        enhanced_content << content;

        

        cloudData = enhanced_content.str();

        

        // Simulate realistic upload processing time based on content size

        auto upload_delay = std::chrono::milliseconds(200 + (content.size() / 100)); // 200ms + 1ms per 100 bytes

        std::this_thread::sleep_for(upload_delay);

        

        timing.operation_complete_time = get_current_time();

        

        size_t new_size = cloudData.size();

        std::cout << "[Writer " << id << "] finished uploading (new size: " << new_size

                  << ") preview: "

                  << (cloudData.size() > 60 ? cloudData.substr(0, 60) + "..." : cloudData)

                  << " [Operation: " << get_microseconds_since(timing.lock_acquired_time) << "μs]\n";

        

        log_event(id, "WRITE", "SUCCESS \"" + test_file + "\" (size: " + 

                 std::to_string(new_size) + " bytes)");

        

        // Log file operation details

        log_real_time_status("Writer #" + std::to_string(id) + " processed " + 

                           std::to_string(content.size()) + " bytes from " + test_file);

    } else {

        // Enhanced fallback content

        std::ostringstream fallback_content;

        fallback_content << "=== FALLBACK CONTENT ===\n";

        fallback_content << "Generated by: Writer #" << id << "\n";

        fallback_content << "Generation time: " << getCurrentTimestampMicro() << "\n";

        fallback_content << "Reason: Could not read " << test_file << "\n";

        fallback_content << "========================\n\n";

        fallback_content << "Default content generated due to file access error.\n";

        fallback_content << "Thread ID: " << id << "\n";

        fallback_content << "Timestamp: " << std::time(nullptr) << "\n";

        

        cloudData = fallback_content.str();

        timing.operation_complete_time = get_current_time();

        

        log_event(id, "WRITE", "FALLBACK (using default content, size: " + 

                 std::to_string(cloudData.size()) + " bytes)");

    }

    

    timing.end_time = get_current_time();

    timing.calculate_durations();

    

    pthread_mutex_unlock(&rw_mutex);

    log_real_time_status("Writer #" + std::to_string(id) + " released exclusive access");

    

    // Log detailed timing information

    log_timing_event(id, "WRITE", timing);

    update_statistics("WRITE", timing);

    update_operation_stats("WRITE", timing.total_time_us, false);

    

    log_event(id, "WRITE", "COMPLETED (total time: " + std::to_string(timing.total_time_us) + "μs)");

    return nullptr;

}



// Enhanced Deleter with microsecond-precision timing and backup functionality

void* deleter(void* arg) {

    int id = *((int*)arg);

    OperationTiming timing;

    timing.start_time = get_current_time();

    update_operation_stats("DELETE", 0, true);

    

    log_event(id, "DELETE", "STARTED");

    log_real_time_status("Deleter #" + std::to_string(id) + " attempting to acquire delete lock");

    

    pthread_mutex_lock(&rw_mutex);

    timing.lock_acquired_time = get_current_time();

    timing.wait_time_us = get_microseconds_since(timing.start_time);

    

    log_real_time_status("Deleter #" + std::to_string(id) + " acquired exclusive access after " + 

                        std::to_string(timing.wait_time_us) + "μs");

    

    size_t prev_size = cloudData.size();

    std::cout << "[Deleter " << id << "] clearing cloud data... (prev size: " << prev_size 

              << ") [Wait: " << timing.wait_time_us << "μs]\n";

    

    // Create backup before deletion

    if (!cloudData.empty()) {

        ensure_directories_exist();

        std::string backup_filename = "./downloads/backup_before_delete_" + std::to_string(id) + 

                                     "_" + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(

                                     std::chrono::system_clock::now().time_since_epoch()).count()) + ".txt";

        

        std::ofstream backup(backup_filename);

        if (backup) {

            backup << "=== DELETION BACKUP METADATA ===\n";

            backup << "Deleted by: Deleter #" << id << "\n";

            backup << "Deletion time: " << getCurrentTimestampMicro() << "\n";

            backup << "Original size: " << prev_size << " bytes\n";

            backup << "================================\n\n";

            backup << cloudData;

            backup.close();

            

            log_real_time_status("Deleter #" + std::to_string(id) + " created backup: " + backup_filename);

        }

    }

    

    cloudData.clear();

    

    // Simulate realistic deletion processing time

    std::this_thread::sleep_for(std::chrono::milliseconds(50 + (id % 25))); // 50-75ms variable delay

    

    timing.operation_complete_time = get_current_time();

    

    std::cout << "[Deleter " << id << "] finished clearing (new size: " << cloudData.size() 

              << ") [Operation: " << get_microseconds_since(timing.lock_acquired_time) << "μs]\n";

    

    timing.end_time = get_current_time();

    timing.calculate_durations();

    

    pthread_mutex_unlock(&rw_mutex);

    log_real_time_status("Deleter #" + std::to_string(id) + " released exclusive access");

    

    log_event(id, "DELETE", "SUCCESS (cleared " + std::to_string(prev_size) + " bytes)");

    

    // Log detailed timing information

    log_timing_event(id, "DELETE", timing);

    update_statistics("DELETE", timing);

    update_operation_stats("DELETE", timing.total_time_us, false);

    

    log_event(id, "DELETE", "COMPLETED (total time: " + std::to_string(timing.total_time_us) + "μs)");

    return nullptr;

}