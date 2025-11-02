#include "cloud.h"
#include <httplib.h>
#include <json/json.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <sstream>
#include <map>

using namespace httplib;
namespace fs = std::filesystem;

// Global variables for HTTP API
std::mutex api_mutex;
std::map<int, pthread_t> managed_threads;
int thread_id_counter = 1;

// CORS middleware
void setup_cors(Response &res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

// File operations endpoints
void setup_file_routes(Server &server) {
    // List files from downloads directory
    server.Get("/api/files", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        Json::Value files(Json::arrayValue);
        
        std::lock_guard<std::mutex> lock(api_mutex);
        
        // Scan downloads directory for real files
        if (fs::exists("./downloads")) {
            for (const auto& entry : fs::directory_iterator("./downloads")) {
                if (entry.is_regular_file()) {
                    Json::Value file;
                    file["id"] = entry.path().filename().string();
                    file["name"] = entry.path().filename().string();
                    file["size"] = static_cast<int>(fs::file_size(entry.path()));
                    auto ftime = fs::last_write_time(entry.path());
                    file["modified"] = std::to_string(ftime.time_since_epoch().count());
                    file["type"] = entry.path().extension().string();
                    files.append(file);
                }
            }
        }
        
        response["files"] = files;
        response["total"] = static_cast<int>(files.size());
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    // Upload file - uses real file operations
    server.Post("/api/files/upload", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        
        try {
            ensure_directories_exist();
            
            // Save uploaded content to test_files directory
            std::string timestamp = std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());
            std::string filename = "./test_files/upload_" + timestamp + ".txt";
            
            std::ofstream outfile(filename);
            if (outfile) {
                outfile << req.body;
                outfile.close();
                
                // Update cloudData
                pthread_mutex_lock(&rw_mutex);
                cloudData = req.body;
                pthread_mutex_unlock(&rw_mutex);
                
                log_event(0, "UPLOAD", "File saved to " + filename);
                
                response["success"] = true;
                response["message"] = "File uploaded successfully";
                response["filename"] = filename;
                response["size"] = static_cast<int>(req.body.size());
            } else {
                response["success"] = false;
                response["message"] = "Failed to save file";
            }
        } catch (const std::exception& e) {
            response["success"] = false;
            response["message"] = e.what();
        }
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    // Delete file - real file deletion
    server.Delete(R"(/api/files/(.+))", [](const Request &req, Response &res) {
        setup_cors(res);
        
        // Extract file_id from path manually
        std::string path = req.path;
        std::string prefix = "/api/files/";
        std::string file_id = path.substr(prefix.length());
        
        Json::Value response;
        std::string filepath = "./downloads/" + file_id;
        
        try {
            if (fs::exists(filepath)) {
                fs::remove(filepath);
                log_event(0, "DELETE", "File deleted: " + file_id);
                response["success"] = true;
                response["message"] = "File deleted successfully";
                response["fileId"] = file_id;
            } else {
                response["success"] = false;
                response["message"] = "File not found";
            }
        } catch (const std::exception& e) {
            response["success"] = false;
            response["message"] = e.what();
        }
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
}

// Cloud statistics endpoints - real statistics
void setup_stats_routes(Server &server) {
    server.Get("/api/stats", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        
        std::lock_guard<std::mutex> lock(api_mutex);
        
        // Count actual files
        int file_count = 0;
        size_t total_size = 0;
        if (fs::exists("./downloads")) {
            for (const auto& entry : fs::directory_iterator("./downloads")) {
                if (entry.is_regular_file()) {
                    file_count++;
                    total_size += fs::file_size(entry.path());
                }
            }
        }
        
        pthread_mutex_lock(&stats_mutex);
        response["totalFiles"] = file_count;
        response["totalSize"] = std::to_string(total_size / 1024) + " KB";
        response["cloudDataSize"] = static_cast<int>(cloudData.size());
        response["activeReaders"] = active_readers;
        response["activeWriters"] = active_writers;
        response["activeDeleters"] = active_deleters;
        response["completedReads"] = completed_reads;
        response["completedWrites"] = completed_writes;
        response["completedDeletes"] = completed_deletes;
        response["activeThreads"] = static_cast<int>(managed_threads.size());
        pthread_mutex_unlock(&stats_mutex);
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
}

// Log viewer endpoints - read from actual log files
void setup_log_routes(Server &server) {
    server.Get("/api/logs", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        Json::Value logs(Json::arrayValue);
        
        std::lock_guard<std::mutex> lock(api_mutex);
        
        // Read from simulation.log
        if (fs::exists("./logs/simulation.log")) {
            std::ifstream logfile("./logs/simulation.log");
            std::string line;
            int count = 0;
            std::vector<std::string> recent_logs;
            
            // Store all lines
            while (std::getline(logfile, line) && count < 100) {
                recent_logs.push_back(line);
                count++;
            }
            
            // Return most recent logs
            for (auto it = recent_logs.rbegin(); it != recent_logs.rend(); ++it) {
                Json::Value log;
                log["message"] = *it;
                log["timestamp"] = getCurrentTimestamp();
                logs.append(log);
            }
        }
        
        response["logs"] = logs;
        response["total"] = static_cast<int>(logs.size());
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
}

// Thread management endpoints - spawn real pthread threads
void setup_thread_routes(Server &server) {
    server.Get("/api/threads", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        Json::Value threads(Json::arrayValue);
        
        std::lock_guard<std::mutex> lock(api_mutex);
        
        pthread_mutex_lock(&stats_mutex);
        for (const auto& [id, thread] : managed_threads) {
            Json::Value thread_obj;
            thread_obj["id"] = id;
            thread_obj["status"] = "RUNNING";
            threads.append(thread_obj);
        }
        pthread_mutex_unlock(&stats_mutex);
        
        response["threads"] = threads;
        response["total"] = static_cast<int>(threads.size());
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    // Spawn thread endpoint
    server.Post("/api/threads/spawn", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Reader json_reader;
        Json::Value request_data;
        Json::Value response;
        
        if (json_reader.parse(req.body, request_data)) {
            std::string thread_type = request_data["type"].asString();
            
            std::lock_guard<std::mutex> lock(api_mutex);
            ensure_directories_exist();
            
            int* tid = new int(thread_id_counter++);
            pthread_t thread;
            
            if (thread_type == "READER") {
                pthread_create(&thread, nullptr, reader, tid);
                managed_threads[*tid] = thread;
                response["success"] = true;
                response["message"] = "Reader thread spawned";
                response["threadId"] = *tid;
            } else if (thread_type == "WRITER") {
                pthread_create(&thread, nullptr, writer, tid);
                managed_threads[*tid] = thread;
                response["success"] = true;
                response["message"] = "Writer thread spawned";
                response["threadId"] = *tid;
            } else if (thread_type == "DELETER") {
                pthread_create(&thread, nullptr, deleter, tid);
                managed_threads[*tid] = thread;
                response["success"] = true;
                response["message"] = "Deleter thread spawned";
                response["threadId"] = *tid;
            } else {
                delete tid;
                response["success"] = false;
                response["message"] = "Invalid thread type";
            }
        } else {
            response["success"] = false;
            response["message"] = "Invalid JSON";
        }
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    // Run stress test endpoint
    server.Post("/api/threads/stress-test", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Reader json_reader;
        Json::Value request_data;
        Json::Value response;
        
        if (json_reader.parse(req.body, request_data)) {
            int num_threads = request_data.get("count", 10).asInt();
            
            // Launch stress test in background
            std::thread([num_threads]() {
                run_stress_test(num_threads);
            }).detach();
            
            response["success"] = true;
            response["message"] = "Stress test started";
            response["threadCount"] = num_threads;
        } else {
            response["success"] = false;
            response["message"] = "Invalid JSON";
        }
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
}

int main() {
    Server server;
    
    // Initialize directories and logging
    ensure_directories_exist();
    log_event(0, "SYSTEM", "HTTP Server starting with advanced cloud storage features");
    std::cout << "=== Advanced Cloud Storage HTTP Server ===" << std::endl;
    std::cout << "Features: Pthread Threading | Microsecond Timing | Real File Operations" << std::endl;
    
    // Handle OPTIONS requests for CORS
    server.Options(".*", [](const Request &req, Response &res) {
        setup_cors(res);
        return;
    });
    
    // Setup routes
    setup_file_routes(server);
    setup_stats_routes(server);
    setup_log_routes(server);
    setup_thread_routes(server);
    
    // Health check endpoint
    server.Get("/api/health", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        response["status"] = "healthy";
        response["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    std::cout << "Cloud Storage Server starting on http://localhost:3001" << std::endl;
    server.listen("0.0.0.0", 3001);
    
    return 0;
}