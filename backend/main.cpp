#include "cloud.h"
#include "unified_os.h"
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
std::mutex process_mutex; // Add mutex for process scheduler thread safety
std::string last_scheduling_algorithm = "";
int last_scheduling_quantum = 2;
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
    
    // Clear/terminate all threads endpoint
    server.Delete("/api/threads", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        
        std::lock_guard<std::mutex> lock(api_mutex);
        
        int terminated_count = 0;
        pthread_mutex_lock(&stats_mutex);
        
        // Detach all managed threads (they will complete naturally)
        for (auto& [id, thread] : managed_threads) {
            pthread_detach(thread);
            terminated_count++;
        }
        managed_threads.clear();
        
        // Reset thread statistics
        active_readers = 0;
        active_writers = 0;
        active_deleters = 0;
        
        pthread_mutex_unlock(&stats_mutex);
        
        response["success"] = true;
        response["message"] = "All threads cleared";
        response["terminatedCount"] = terminated_count;
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
}

// OS Module endpoints
void setup_os_routes(Server &server) {
    // Process Scheduler endpoints
    server.Get("/api/os/processes", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        
        // Lock mutex for thread safety
        std::lock_guard<std::mutex> lock(process_mutex);
        
        // Return current state without resetting
        const auto& procs = process_scheduler.getProcesses();
        Json::Value processes(Json::arrayValue);
        
        for (const auto& proc : procs) {
            Json::Value p;
            p["pid"] = proc.pid;
            p["processName"] = proc.process_name;
            p["arrivalTime"] = proc.arrival_time;
            p["burstTime"] = proc.burst_time;
            p["priority"] = proc.priority;
            p["startTime"] = proc.start_time;
            p["completionTime"] = proc.completion_time;
            p["waitingTime"] = proc.waiting_time;
            p["turnaroundTime"] = proc.turnaround_time;
            processes.append(p);
        }
        
        // Serialize Gantt chart data
        Json::Value ganttChart(Json::arrayValue);
        const auto& gantt = process_scheduler.getGanttChart();
        for (const auto& entry : gantt) {
            Json::Value g;
            g["processId"] = entry.process_id;
            g["processName"] = entry.process_name;
            g["startTime"] = entry.start_time;
            g["endTime"] = entry.end_time;
            ganttChart.append(g);
        }
        
        response["averageWaitingTime"] = process_scheduler.getAverageWaitingTime();
        response["averageTurnaroundTime"] = process_scheduler.getAverageTurnaroundTime();
        response["processCount"] = static_cast<int>(procs.size());
        response["algorithm"] = process_scheduler.getCurrentAlgorithm();
        response["processes"] = processes;
        response["ganttChart"] = ganttChart;
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    server.Post("/api/os/processes/schedule", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        Json::Reader reader;
        Json::Value request_body;
        
        if (reader.parse(req.body, request_body)) {
            std::string algorithm = request_body.get("algorithm", "FCFS").asString();
            int quantum = request_body.get("quantum", 2).asInt();
            int processCount = request_body.get("processCount", 5).asInt();
            
            // CRITICAL FIX: Lock mutex to prevent race conditions
            std::lock_guard<std::mutex> lock(process_mutex);
            
            process_scheduler.resetScheduler();
            process_scheduler.generateRandomProcesses(processCount);
            process_scheduler.executeScheduler(algorithm, quantum);
            // Save the scheduling algorithm and quantum for later re-execution
            last_scheduling_algorithm = algorithm;
            last_scheduling_quantum = quantum;
            
            
            // Serialize detailed process data
            Json::Value processes(Json::arrayValue);
            const auto& procs = process_scheduler.getProcesses();
            for (const auto& proc : procs) {
                Json::Value p;
                p["pid"] = proc.pid;
                p["processName"] = proc.process_name;
                p["arrivalTime"] = proc.arrival_time;
                p["burstTime"] = proc.burst_time;
                p["priority"] = proc.priority;
                p["startTime"] = proc.start_time;
                p["completionTime"] = proc.completion_time;
                p["waitingTime"] = proc.waiting_time;
                p["turnaroundTime"] = proc.turnaround_time;
                processes.append(p);
            }
            
            // Serialize Gantt chart data
            Json::Value ganttChart(Json::arrayValue);
            const auto& gantt = process_scheduler.getGanttChart();
            for (const auto& entry : gantt) {
                Json::Value g;
                g["processId"] = entry.process_id;
                g["processName"] = entry.process_name;
                g["startTime"] = entry.start_time;
                g["endTime"] = entry.end_time;
                ganttChart.append(g);
            }
            
            response["success"] = true;
            response["algorithm"] = algorithm;
            response["processCount"] = processCount;
            response["averageWaitingTime"] = process_scheduler.getAverageWaitingTime();
            response["averageTurnaroundTime"] = process_scheduler.getAverageTurnaroundTime();
            response["processes"] = processes;
            response["ganttChart"] = ganttChart;
        } else {
            response["success"] = false;
            response["error"] = "Invalid request body";
        }
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    // Add manual process endpoint
    server.Post("/api/os/processes/add", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        Json::Reader reader;
        Json::Value request_body;
        
        if (reader.parse(req.body, request_body)) {
            std::lock_guard<std::mutex> lock(process_mutex);
            
            std::string processName = request_body.get("processName", "Custom Process").asString();
            int arrivalTime = request_body.get("arrivalTime", 0).asInt();
            int burstTime = request_body.get("burstTime", 1).asInt();
            int priority = request_body.get("priority", 1).asInt();
            
            // Create and add the process
            int pid = process_scheduler.getNextPid();
            Process newProcess(pid, processName, arrivalTime, burstTime, priority);
            process_scheduler.addProcess(newProcess);
            
            // Re-run the last scheduling algorithm if one was executed
            if (!last_scheduling_algorithm.empty()) {
                process_scheduler.resetProcessStates();
                process_scheduler.executeScheduler(last_scheduling_algorithm, last_scheduling_quantum);
            }
            
            response["success"] = true;
            response["message"] = "Process added successfully";
            response["process"]["pid"] = pid;
            response["process"]["processName"] = processName;
            response["process"]["arrivalTime"] = arrivalTime;
            response["process"]["burstTime"] = burstTime;
            response["process"]["priority"] = priority;
        } else {
            response["success"] = false;
            response["error"] = "Invalid request body";
        }
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    // Edit process endpoint
    server.Post(R"(/api/os/processes/edit/(\d+))", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        Json::Reader reader;
        Json::Value request_body;
        
        // Extract pid from path
        std::string path = req.path;
        std::string prefix = "/api/os/processes/edit/";
        int pid = std::stoi(path.substr(prefix.length()));
        
        if (reader.parse(req.body, request_body)) {
            std::lock_guard<std::mutex> lock(process_mutex);
            
            std::string processName = request_body.get("processName", "").asString();
            int arrivalTime = request_body.get("arrivalTime", 0).asInt();
            int burstTime = request_body.get("burstTime", 1).asInt();
            int priority = request_body.get("priority", 1).asInt();
            
            bool success = process_scheduler.editProcessAPI(pid, processName, arrivalTime, burstTime, priority);
            
            // Re-run the last scheduling algorithm if one was executed
            if (success && !last_scheduling_algorithm.empty()) {
                process_scheduler.resetProcessStates();
                process_scheduler.executeScheduler(last_scheduling_algorithm, last_scheduling_quantum);
            }
            
            if (success) {
                Process* proc = process_scheduler.findProcess(pid);
                response["success"] = true;
                response["message"] = "Process updated successfully";
                response["process"]["pid"] = proc->pid;
                response["process"]["processName"] = proc->process_name;
                response["process"]["arrivalTime"] = proc->arrival_time;
                response["process"]["burstTime"] = proc->burst_time;
                response["process"]["priority"] = proc->priority;
            } else {
                response["success"] = false;
                response["error"] = "Process not found or invalid parameters";
            }
        } else {
            response["success"] = false;
            response["error"] = "Invalid request body";
        }
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    // Delete process endpoint
    server.Delete(R"(/api/os/processes/(\d+))", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        
        // Extract pid from path
        std::string path = req.path;
        std::string prefix = "/api/os/processes/";
        int pid = std::stoi(path.substr(prefix.length()));
        
        std::lock_guard<std::mutex> lock(process_mutex);
        
        Process* proc = process_scheduler.findProcess(pid);
        if (proc) {
            std::string processName = proc->process_name;
            process_scheduler.deleteProcess(pid);
            
            // Re-run the last scheduling algorithm if one was executed
            if (!last_scheduling_algorithm.empty()) {
                process_scheduler.resetProcessStates();
                process_scheduler.executeScheduler(last_scheduling_algorithm, last_scheduling_quantum);
            }
            
            response["success"] = true;
            response["message"] = "Process deleted successfully";
            response["deletedProcess"]["pid"] = pid;
            response["deletedProcess"]["processName"] = processName;
        } else {
            response["success"] = false;
            response["error"] = "Process not found";
        }
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    // File System endpoints
    server.Get("/api/os/filesystem", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        
        response["totalBlocks"] = 1024;
        response["blockSize"] = 4096;
        response["utilization"] = file_system.getDiskUtilization();
        response["status"] = "operational";
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    server.Post("/api/os/filesystem/create", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        Json::Reader reader;
        Json::Value request_body;
        
        if (reader.parse(req.body, request_body)) {
            std::string path = request_body.get("path", "/test.txt").asString();
            std::string data = request_body.get("data", "").asString();
            
            bool created = file_system.createFile(path, 0);
            bool written = false;
            if (created && !data.empty()) {
                written = file_system.writeFile(path, data);
            }
            
            response["success"] = created;
            response["written"] = written;
            response["utilization"] = file_system.getDiskUtilization();
        } else {
            response["success"] = false;
            response["error"] = "Invalid request body";
        }
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    // IPC endpoints
    server.Get("/api/os/ipc", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        
        response["messageQueues"] = ipc_manager.getMessageQueueCount();
        response["sharedMemorySegments"] = ipc_manager.getSharedMemoryCount();
        response["status"] = "operational";
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    server.Post("/api/os/ipc/message", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        Json::Reader reader;
        Json::Value request_body;
        
        if (reader.parse(req.body, request_body)) {
            int queueId = request_body.get("queueId", 1).asInt();
            int sender = request_body.get("sender", 100).asInt();
            int receiver = request_body.get("receiver", 200).asInt();
            std::string content = request_body.get("content", "Test message").asString();
            
            // Ensure queue exists
            ipc_manager.createMessageQueue(queueId, 10);
            bool sent = ipc_manager.sendMessage(queueId, sender, receiver, content);
            
            response["success"] = sent;
        } else {
            response["success"] = false;
            response["error"] = "Invalid request body";
        }
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    // View IPC messages endpoint
    server.Get("/api/os/ipc/messages", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        
        response["totalMessages"] = ipc_manager.getTotalMessages();
        response["queues"] = ipc_manager.getMessageQueueCount();
        response["sharedMemorySegments"] = ipc_manager.getSharedMemoryCount();
        response["status"] = "operational";
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    // Deadlock Detection endpoints
    server.Get("/api/os/deadlock", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        
        bool hasDeadlock = deadlock_detector.detectDeadlock();
        bool safeState = deadlock_detector.isSafeState();
        
        response["hasDeadlock"] = hasDeadlock;
        response["safeState"] = safeState;
        response["status"] = "operational";
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    server.Post("/api/os/deadlock/simulate", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        
        deadlock_detector.simulateDeadlockScenario();
        bool hasDeadlock = deadlock_detector.detectDeadlock();
        
        response["success"] = true;
        response["deadlockCreated"] = hasDeadlock;
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    server.Get("/api/os/deadlock/visualize", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        
        // Get wait-for graph
        const auto& waitForGraph = deadlock_detector.getWaitForGraph();
        const auto& processes = deadlock_detector.getProcesses();
        const auto& resources = deadlock_detector.getResources();
        
        // Build wait-for graph JSON
        Json::Value graphData(Json::arrayValue);
        for (const auto& entry : waitForGraph) {
            int processId = entry.first;
            const auto& waitingFor = entry.second;
            Json::Value edge;
            edge["processId"] = processId;
            
            // Find process name
            auto procIt = std::find_if(processes.begin(), processes.end(),
                [processId](const DLProcess& p) { return p.process_id == processId; });
            edge["processName"] = (procIt != processes.end()) ? procIt->process_name : "P" + std::to_string(processId);
            
            // Add waiting for list
            Json::Value waitingList(Json::arrayValue);
            for (int waitId : waitingFor) {
                auto waitProcIt = std::find_if(processes.begin(), processes.end(),
                    [waitId](const DLProcess& p) { return p.process_id == waitId; });
                Json::Value waitInfo;
                waitInfo["processId"] = waitId;
                waitInfo["processName"] = (waitProcIt != processes.end()) ? waitProcIt->process_name : "P" + std::to_string(waitId);
                waitingList.append(waitInfo);
            }
            edge["waitingFor"] = waitingList;
            graphData.append(edge);
        }
        
        // Build processes info
        Json::Value processesData(Json::arrayValue);
        for (const auto& proc : processes) {
            Json::Value procInfo;
            procInfo["id"] = proc.process_id;
            procInfo["name"] = proc.process_name;
            
            // Allocated resources
            Json::Value allocatedRes(Json::arrayValue);
            for (const auto& alloc : proc.allocated) {
                int resId = alloc.first;
                int amount = alloc.second;
                if (amount > 0) {
                    auto resIt = std::find_if(resources.begin(), resources.end(),
                        [resId](const Resource& r) { return r.resource_id == resId; });
                    Json::Value resInfo;
                    resInfo["id"] = resId;
                    resInfo["name"] = (resIt != resources.end()) ? resIt->resource_name : "R" + std::to_string(resId);
                    resInfo["amount"] = amount;
                    allocatedRes.append(resInfo);
                }
            }
            procInfo["allocated"] = allocatedRes;
            
            // Needed resources
            Json::Value neededRes(Json::arrayValue);
            for (const auto& need : proc.needed) {
                int resId = need.first;
                int amount = need.second;
                if (amount > 0) {
                    auto resIt = std::find_if(resources.begin(), resources.end(),
                        [resId](const Resource& r) { return r.resource_id == resId; });
                    Json::Value resInfo;
                    resInfo["id"] = resId;
                    resInfo["name"] = (resIt != resources.end()) ? resIt->resource_name : "R" + std::to_string(resId);
                    resInfo["amount"] = amount;
                    neededRes.append(resInfo);
                }
            }
            procInfo["needed"] = neededRes;
            processesData.append(procInfo);
        }
        
        response["waitForGraph"] = graphData;
        
        // Build RAG (Resource Allocation Graph) data
        auto ragEdges = deadlock_detector.getResourceAllocationGraph();
        Json::Value ragEdgesData(Json::arrayValue);
        for (const auto& edge : ragEdges) {
            Json::Value edgeJson;
            edgeJson["type"] = edge.type;
            edgeJson["from"]["id"] = edge.from_id;
            edgeJson["from"]["type"] = edge.from_type;
            edgeJson["from"]["name"] = edge.from_name;
            edgeJson["to"]["id"] = edge.to_id;
            edgeJson["to"]["type"] = edge.to_type;
            edgeJson["to"]["name"] = edge.to_name;
            edgeJson["units"] = edge.units;
            ragEdgesData.append(edgeJson);
        }
        
        // Build resources info
        Json::Value resourcesData(Json::arrayValue);
        for (const auto& res : resources) {
            Json::Value resInfo;
            resInfo["id"] = res.resource_id;
            resInfo["name"] = res.resource_name;
            resInfo["total"] = res.total_units;
            resInfo["available"] = res.available_units;
            resourcesData.append(resInfo);
        }
        
        response["ragEdges"] = ragEdgesData;
        response["resources"] = resourcesData;
        response["processes"] = processesData;
        response["hasDeadlock"] = deadlock_detector.detectDeadlock();
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    server.Post("/api/os/deadlock/recover", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        
        auto deadlockedBefore = deadlock_detector.findDeadlockedProcesses();
        deadlock_detector.recoverFromDeadlock();
        auto deadlockedAfter = deadlock_detector.findDeadlockedProcesses();
        
        response["success"] = true;
        response["processesTerminated"] = static_cast<int>(deadlockedBefore.size() - deadlockedAfter.size());
        response["stillDeadlocked"] = deadlock_detector.detectDeadlock();
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    // Shared Memory endpoints
    server.Post("/api/os/ipc/shared-memory", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        Json::Reader reader;
        Json::Value request_body;
        
        if (reader.parse(req.body, request_body)) {
            std::string name = request_body.get("name", "").asString();
            int size = request_body.get("size", 1024).asInt();
            std::string initialData = request_body.get("data", "").asString();
            
            if (name.empty()) {
                response["success"] = false;
                response["error"] = "Memory segment name is required";
            } else {
                void* segment = ipc_manager.createSharedMemory(name, size);
                if (segment && !initialData.empty()) {
                    ipc_manager.writeToSharedMemory(name, initialData);
                }
                response["success"] = (segment != nullptr);
                response["name"] = name;
                response["size"] = size;
            }
        } else {
            response["success"] = false;
            response["error"] = "Invalid request body";
        }
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    server.Get("/api/os/ipc/shared-memory", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        
        if (!req.has_param("name")) {
            response["success"] = false;
            response["error"] = "Memory segment name is required";
        } else {
            std::string name = req.get_param_value("name");
            if (name.empty()) {
                response["success"] = false;
                response["error"] = "Memory segment name cannot be empty";
            } else {
                std::string data = ipc_manager.readFromSharedMemory(name);
                
                if (data.find("❌") != std::string::npos) {
                    response["success"] = false;
                    response["error"] = "Shared memory segment not found";
                } else {
                    response["success"] = true;
                    response["name"] = name;
                    response["data"] = data;
                }
            }
        }
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    server.Post("/api/os/ipc/shared-memory/write", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        Json::Reader reader;
        Json::Value request_body;
        
        if (reader.parse(req.body, request_body)) {
            std::string name = request_body.get("name", "").asString();
            std::string data = request_body.get("data", "").asString();
            
            if (name.empty()) {
                response["success"] = false;
                response["error"] = "Memory segment name is required";
            } else {
                ipc_manager.writeToSharedMemory(name, data);
                response["success"] = true;
                response["name"] = name;
            }
        } else {
            response["success"] = false;
            response["error"] = "Invalid request body";
        }
        
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, response);
        res.set_content(json_string, "application/json");
    });
    
    // Comprehensive OS simulation
    server.Post("/api/os/simulate", [](const Request &req, Response &res) {
        setup_cors(res);
        Json::Value response;
        Json::Reader reader;
        Json::Value request_body;
        
        if (reader.parse(req.body, request_body)) {
            std::string module = request_body.get("module", "all").asString();
            
            if (module == "processes" || module == "all") {
                run_process_scheduler_demo();
            }
            if (module == "filesystem" || module == "all") {
                run_file_system_demo();
            }
            if (module == "ipc" || module == "all") {
                run_ipc_demo();
            }
            if (module == "deadlock" || module == "all") {
                run_deadlock_detection_demo();
            }
            
            response["success"] = true;
            response["module"] = module;
        } else {
            response["success"] = false;
            response["error"] = "Invalid request body";
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
    setup_os_routes(server);
    
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