#include "process_scheduler.h"
#include <limits>
#include <iostream>
#include <iomanip>
#include <string>

void ProcessScheduler::addProcess(const Process& process) {
    processes.push_back(process);
}

void ProcessScheduler::FCFS() {
    std::cout << "Executing FCFS Scheduling...\n";
    clearGanttChart();
    
    // Sort processes by arrival time
    std::sort(processes.begin(), processes.end(), 
              [](const Process& a, const Process& b) {
                  return a.arrival_time < b.arrival_time;
              });
    
    current_time = 0;
    
    for (auto& process : processes) {
        if (current_time < process.arrival_time) {
            current_time = process.arrival_time;
        }
        
        int start_time = current_time;
        process.start_time = start_time;
        process.state = RUNNING;
        
        // Record Gantt chart entry with process name
        gantt_chart.emplace_back(process.pid, process.process_name, start_time, start_time + process.burst_time);
        
        std::cout << "Time " << current_time << ": " << process.process_name << " (P" << process.pid << ") starts execution\n";
        current_time += process.burst_time;
        
        process.completion_time = current_time;
        process.turnaround_time = process.completion_time - process.arrival_time;
        process.waiting_time = process.turnaround_time - process.burst_time;
        process.state = TERMINATED;
        
        std::cout << "Time " << current_time << ": " << process.process_name << " (P" << process.pid << ") completes\n";
    }
}

void ProcessScheduler::SJF() {
    std::cout << "Executing SJF Scheduling...\n";
    clearGanttChart();
    
    std::vector<Process*> ready_list;
    current_time = 0;
    int completed = 0;
    int n = processes.size();
    
    while (completed < n) {
        // Add arrived processes to ready list
        for (auto& process : processes) {
            if (process.arrival_time <= current_time && process.state == NEW) {
                process.state = READY;
                ready_list.push_back(&process);
            }
        }
        
        if (ready_list.empty()) {
            current_time++;
            continue;
        }
        
        // Sort by burst time (SJF)
        std::sort(ready_list.begin(), ready_list.end(),
                  [](const Process* a, const Process* b) {
                      return a->burst_time < b->burst_time;
                  });
        
        Process* current = ready_list.front();
        ready_list.erase(ready_list.begin());
        
        current->start_time = current_time;
        current->state = RUNNING;
        
        // Record Gantt chart entry with process name
        gantt_chart.emplace_back(current->pid, current->process_name, current_time, current_time + current->burst_time);
        
        std::cout << "Time " << current_time << ": " << current->process_name << " (P" << current->pid << ") starts execution\n";
        current_time += current->burst_time;
        
        current->completion_time = current_time;
        current->turnaround_time = current->completion_time - current->arrival_time;
        current->waiting_time = current->turnaround_time - current->burst_time;
        current->state = TERMINATED;
        completed++;
        
        std::cout << "Time " << current_time << ": " << current->process_name << " (P" << current->pid << ") completes\n";
    }
}

void ProcessScheduler::RoundRobin(int time_quantum) {
    std::cout << "Executing Round Robin Scheduling (Quantum=" << time_quantum << ")...\n";
    clearGanttChart();
    
    std::queue<Process*> ready_queue;
    current_time = 0;
    int completed = 0;
    int n = processes.size();
    
    // Initialize processes
    for (auto& process : processes) {
        process.remaining_time = process.burst_time;
        process.start_time = -1;
    }
    
    while (completed < n) {
        // Add arrived processes to ready queue
        for (auto& process : processes) {
            if (process.arrival_time <= current_time && process.state == NEW) {
                process.state = READY;
                ready_queue.push(&process);
            }
        }
        
        if (ready_queue.empty()) {
            current_time++;
            continue;
        }
        
        Process* current = ready_queue.front();
        ready_queue.pop();
        
        if (current->start_time == -1) {
            current->start_time = current_time;
        }
        
        current->state = RUNNING;
        
        int execution_time = std::min(time_quantum, current->remaining_time);
        int start_execution = current_time;
        
        // Record Gantt chart entry for this execution slice with process name
        gantt_chart.emplace_back(current->pid, current->process_name, start_execution, start_execution + execution_time);
        
        std::cout << "Time " << current_time << ": " << current->process_name << " (P" << current->pid 
                  << ") executes for " << execution_time << " units\n";
        
        current_time += execution_time;
        current->remaining_time -= execution_time;
        
        if (current->remaining_time == 0) {
            current->completion_time = current_time;
            current->turnaround_time = current->completion_time - current->arrival_time;
            current->waiting_time = current->turnaround_time - current->burst_time;
            current->state = TERMINATED;
            completed++;
            std::cout << "Time " << current_time << ": " << current->process_name << " (P" << current->pid << ") completes\n";
        } else {
            current->state = READY;
            ready_queue.push(current);
        }
    }
}

void ProcessScheduler::PriorityScheduling() {
    std::cout << "Executing Priority Scheduling...\n";
    clearGanttChart();
    
    std::vector<Process*> ready_list;
    current_time = 0;
    int completed = 0;
    int n = processes.size();
    
    while (completed < n) {
        // Add arrived processes to ready list
        for (auto& process : processes) {
            if (process.arrival_time <= current_time && process.state == NEW) {
                process.state = READY;
                ready_list.push_back(&process);
            }
        }
        
        if (ready_list.empty()) {
            current_time++;
            continue;
        }
        
        // Sort by priority (lower number = higher priority)
        std::sort(ready_list.begin(), ready_list.end(),
                  [](const Process* a, const Process* b) {
                      return a->priority < b->priority;
                  });
        
        Process* current = ready_list.front();
        ready_list.erase(ready_list.begin());
        
        current->start_time = current_time;
        current->state = RUNNING;
        
        // Record Gantt chart entry with process name
        gantt_chart.emplace_back(current->pid, current->process_name, current_time, current_time + current->burst_time);
        
        std::cout << "Time " << current_time << ": " << current->process_name << " (P" << current->pid << ") starts execution\n";
        current_time += current->burst_time;
        
        current->completion_time = current_time;
        current->turnaround_time = current->completion_time - current->arrival_time;
        current->waiting_time = current->turnaround_time - current->burst_time;
        current->state = TERMINATED;
        completed++;
        
        std::cout << "Time " << current_time << ": " << current->process_name << " (P" << current->pid << ") completes\n";
    }
}

void ProcessScheduler::executeScheduler(const std::string& algorithm, int quantum) {
    current_algorithm = algorithm;
    resetProcessStates();
    if (algorithm == "FCFS") {
        FCFS();
    } else if (algorithm == "SJF") {
        SJF();
    } else if (algorithm == "RR") {
        RoundRobin(quantum);
    } else if (algorithm == "PRIORITY") {
        PriorityScheduling();
    }
}

void ProcessScheduler::displayResults() {
    std::cout << "\n=== SCHEDULING RESULTS ===\n";
    std::cout << std::setw(5) << "PID" << std::setw(15) << "Process Name" 
              << std::setw(8) << "Arrival" << std::setw(8) << "Burst" << std::setw(8) << "Priority"
              << std::setw(8) << "Start" << std::setw(12) << "Completion"
              << std::setw(10) << "Waiting" << std::setw(14) << "Turnaround\n";
    
    for (const auto& process : processes) {
        std::cout << std::setw(5) << process.pid 
                  << std::setw(15) << process.process_name
                  << std::setw(8) << process.arrival_time
                  << std::setw(8) << process.burst_time
                  << std::setw(8) << process.priority
                  << std::setw(8) << process.start_time
                  << std::setw(12) << process.completion_time
                  << std::setw(10) << process.waiting_time
                  << std::setw(14) << process.turnaround_time << "\n";
    }
    
    std::cout << "\nAverage Waiting Time: " << getAverageWaitingTime() << "\n";
    std::cout << "Average Turnaround Time: " << getAverageTurnaroundTime() << "\n";
}

// ... (keep all other existing functions the same until addManualProcess)

void ProcessScheduler::addManualProcess() {
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "➕ ADD MANUAL PROCESS\n";
    std::cout << std::string(50, '=') << "\n";
    
    // AUTO-ASSIGN PID
    int pid = getNextPid();
    std::cout << "🆔 Auto-assigned PID: " << pid << "\n";
    
    int arrival, burst, priority;
    std::string process_name;
    
    // Get process name
    std::cout << "Enter Process Name: ";
    std::cin.ignore(); // Clear newline
    std::getline(std::cin, process_name);
    
    // If no name provided, use default
    if (process_name.empty()) {
        process_name = "Process " + std::to_string(pid);
        std::cout << "Using default name: " << process_name << "\n";
    }
    
    // Get arrival time
    while (true) {
        std::cout << "Enter Arrival Time: ";
        if (std::cin >> arrival && arrival >= 0) {
            break;
        } else {
            std::cout << "❌ Invalid input! Please enter a non-negative integer.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    
    // Get burst time
    while (true) {
        std::cout << "Enter Burst Time: ";
        if (std::cin >> burst && burst > 0) {
            break;
        } else {
            std::cout << "❌ Invalid input! Please enter a positive integer.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    
    // Get priority
    while (true) {
        std::cout << "Enter Priority (lower number = higher priority): ";
        if (std::cin >> priority && priority >= 0) {
            break;
        } else {
            std::cout << "❌ Invalid input! Please enter a non-negative integer.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    
    // Create and validate process
    Process new_process(pid, process_name, arrival, burst, priority);
    if (validateProcess(new_process)) {
        processes.push_back(new_process);
        std::cout << "✅ " << process_name << " (P" << pid << ") added successfully!\n";
        
        // Show current process list
        displayCurrentProcesses();
    } else {
        std::cout << "❌ Failed to add process! Validation failed.\n";
    }
}

void ProcessScheduler::displayCurrentProcesses() {
    if (processes.empty()) {
        std::cout << "📭 No processes in the queue.\n";
        return;
    }
    
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "📋 CURRENT PROCESS QUEUE (" << processes.size() << " processes)\n";
    std::cout << std::string(60, '=') << "\n";
    
    std::cout << std::setw(5) << "PID" << std::setw(20) << "Process Name" 
              << std::setw(10) << "Arrival" << std::setw(10) << "Burst" << std::setw(10) << "Priority"
              << std::setw(15) << "Status" << "\n";
    std::cout << std::string(70, '-') << "\n";
    
    for (const auto& process : processes) {
        std::string state_str;
        switch (process.state) {
            case NEW: state_str = "New"; break;
            case READY: state_str = "Ready"; break;
            case RUNNING: state_str = "Running"; break;
            case WAITING: state_str = "Waiting"; break;
            case TERMINATED: state_str = "Terminated"; break;
        }
        
        std::cout << std::setw(5) << process.pid 
                  << std::setw(20) << process.process_name
                  << std::setw(10) << process.arrival_time
                  << std::setw(10) << process.burst_time
                  << std::setw(10) << process.priority
                  << std::setw(15) << state_str << "\n";
    }
    
    // Calculate totals
    int total_burst = 0;
    int max_arrival = 0;
    for (const auto& process : processes) {
        total_burst += process.burst_time;
        max_arrival = std::max(max_arrival, process.arrival_time);
    }
    
    std::cout << "\n📊 Queue Summary:\n";
    std::cout << "  • Total CPU Time Needed: " << total_burst << " units\n";
    std::cout << "  • Last Arrival Time: " << max_arrival << " units\n";
    std::cout << "  • Estimated Completion: ~" << (max_arrival + total_burst) << " units\n";
}

void ProcessScheduler::editProcess(int process_id) {
    auto it = std::find_if(processes.begin(), processes.end(),
                          [process_id](const Process& p) { return p.pid == process_id; });
    
    if (it == processes.end()) {
        std::cout << "❌ Process P" << process_id << " not found!\n";
        return;
    }
    
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "✏️  EDITING " << it->process_name << " (P" << process_id << ")\n";
    std::cout << std::string(50, '=') << "\n";
    
    std::cout << "Current values:\n";
    std::cout << "  Process Name: " << it->process_name << "\n";
    std::cout << "  Arrival Time: " << it->arrival_time << "\n";
    std::cout << "  Burst Time: " << it->burst_time << "\n";
    std::cout << "  Priority: " << it->priority << "\n";
    
    // Get new values
    std::string new_name;
    int new_arrival, new_burst, new_priority;
    
    std::cout << "\nEnter new Process Name (" << it->process_name << "): ";
    std::cin.ignore();
    std::getline(std::cin, new_name);
    if (!new_name.empty()) {
        it->process_name = new_name;
    }
    
    std::cout << "Enter new Arrival Time (" << it->arrival_time << "): ";
    if (std::cin >> new_arrival && new_arrival >= 0) {
        it->arrival_time = new_arrival;
    }
    
    std::cout << "Enter new Burst Time (" << it->burst_time << "): ";
    if (std::cin >> new_burst && new_burst > 0) {
        it->burst_time = new_burst;
        it->remaining_time = new_burst;
    }
    
    std::cout << "Enter new Priority (" << it->priority << "): ";
    if (std::cin >> new_priority && new_priority >= 0) {
        it->priority = new_priority;
    }
    
    std::cout << "✅ " << it->process_name << " (P" << process_id << ") updated successfully!\n";
}

void ProcessScheduler::generateRandomProcesses(int count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> arrival(0, 10);
    std::uniform_int_distribution<> burst(1, 10);
    std::uniform_int_distribution<> priority(1, 5);
    
    // List of sample process names for random generation
    std::vector<std::string> sample_names = {
        "Web Browser", "Text Editor", "Media Player", "File Manager", 
        "System Monitor", "Calculator", "Terminal", "Image Viewer",
        "Music Player", "Video Editor", "Compiler", "Database Server",
        "Web Server", "Game Engine", "AI Processor", "Backup Tool"
    };
    
    std::uniform_int_distribution<> name_dist(0, sample_names.size() - 1);
    
    for (int i = 0; i < count; i++) {
        int pid = getNextPid();
        std::string name = sample_names[name_dist(gen)] + " " + std::to_string(pid);
        processes.emplace_back(pid, name, arrival(gen), burst(gen), priority(gen));
    }
}

void ProcessScheduler::displayGanttChart() {
    if (gantt_chart.empty()) {
        std::cout << "No Gantt chart data available.\n";
        return;
    }
    
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "📊 GANTT CHART VISUALIZATION\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    // Display timeline
    std::cout << "Process Execution Timeline:\n";
    std::cout << "Time: ";
    for (const auto& entry : gantt_chart) {
        std::cout << std::setw(4) << entry.start_time << " ";
    }
    std::cout << gantt_chart.back().end_time << "\n";
    
    // Display process bars with names
    std::cout << "        ";
    for (const auto& entry : gantt_chart) {
        // Shorten long names for display
        std::string display_name = entry.process_name;
        if (display_name.length() > 6) {
            display_name = display_name.substr(0, 6) + "..";
        }
        std::cout << "+" << std::string(6, '-') << "+ ";
    }
    std::cout << "\n";
    
    std::cout << "Process:";
    for (const auto& entry : gantt_chart) {
        std::string display_name = entry.process_name;
        if (display_name.length() > 6) {
            display_name = display_name.substr(0, 6) + "..";
        }
      std::cout << "|" << std::setw(6) << display_name << "| ";
    }
    std::cout << "\n";
    
    std::cout << "        ";
    for (const auto& entry : gantt_chart) {
        std::cout << "+" << std::string(6, '-') << "+ ";
    }
    std::cout << "\n\n";
    
    // Detailed execution information
    std::cout << "Detailed Execution:\n";
    std::cout << std::setw(20) << "Process Name" << std::setw(12) << "Start" 
              << std::setw(12) << "End" << std::setw(16) << "Duration\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (const auto& entry : gantt_chart) {
        std::cout << std::setw(20) << entry.process_name
                  << std::setw(12) << entry.start_time
                  << std::setw(12) << entry.end_time
                  << std::setw(16) << (entry.end_time - entry.start_time) << "\n";
    }
}


void ProcessScheduler::displayProcessTimeline() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "⏰ PROCESS TIMELINE WITH WAITING/BURST TIME BARS\n";
    std::cout << std::string(80, '=') << "\n\n";
    
    // Find maximum time for scaling
    int max_time = 0;
    for (const auto& process : processes) {
        max_time = std::max(max_time, process.completion_time);
    }
    
    // Display each process timeline
    for (const auto& process : processes) {
        std::cout << "P" << process.pid << " | ";
        
        // Waiting period (before start)
        for (int t = 0; t < process.start_time; t++) {
            std::cout << "░";
        }
        
        // Execution period
        for (int t = process.start_time; t < process.completion_time; t++) {
            std::cout << "█";
        }
        
        // Idle period after completion (if any)
        for (int t = process.completion_time; t < max_time; t++) {
            std::cout << " ";
        }
        
        std::cout << " | Arrival: " << std::setw(2) << process.arrival_time
                  << ", Start: " << std::setw(2) << process.start_time
                  << ", Burst: " << std::setw(2) << process.burst_time
                  << ", Wait: " << std::setw(2) << process.waiting_time
                  << "\n";
    }
    
    // Time scale
    std::cout << "\nTime:   ";
    for (int t = 0; t <= max_time; t++) {
        std::cout << std::setw(2) << t << " ";
    }
    std::cout << "\n";
    
    std::cout << "\nLegend: █ = Executing, ░ = Waiting\n";
}

void ProcessScheduler::displayVisualResults() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "📈 COMPREHENSIVE SCHEDULING ANALYSIS\n";
    std::cout << std::string(80, '=') << "\n";
    
    // Display numerical results
    displayResults();
    
    // Display visualizations
    displayGanttChart();
    displayProcessTimeline();
    
    // Performance summary
    std::cout << "\n" << std::string(40, '-') << "\n";
    std::cout << "📊 PERFORMANCE SUMMARY\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << "Average Waiting Time: " << std::fixed << std::setprecision(2) 
              << getAverageWaitingTime() << " units\n";
    std::cout << "Average Turnaround Time: " << getAverageTurnaroundTime() << " units\n";
    
    if (!gantt_chart.empty()) {
        std::cout << "CPU Utilization: " << std::setprecision(1) 
                  << (static_cast<double>(gantt_chart.back().end_time) / gantt_chart.back().end_time * 100) << "%\n";
        std::cout << "Throughput: " << processes.size() << " processes in " 
                  << gantt_chart.back().end_time << " units = " 
                  << std::setprecision(3) << (static_cast<double>(processes.size()) / gantt_chart.back().end_time)
                  << " processes/unit\n";
    }
}

void ProcessScheduler::clearGanttChart() {
    gantt_chart.clear();
}


// Add these missing function implementations to process_scheduler.cpp

double ProcessScheduler::getAverageWaitingTime() {
    if (processes.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& process : processes) {
        total += process.waiting_time;
    }
    return total / processes.size();
}

double ProcessScheduler::getAverageTurnaroundTime() {
    if (processes.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& process : processes) {
        total += process.turnaround_time;
    }
    return total / processes.size();
}

bool ProcessScheduler::validateProcess(const Process& process) {
    // Basic validation rules
    if (process.pid < 0) {
        std::cout << "❌ Invalid PID: " << process.pid << "\n";
        return false;
    }
    
    if (process.arrival_time < 0) {
        std::cout << "❌ Invalid arrival time: " << process.arrival_time << "\n";
        return false;
    }
    
    if (process.burst_time <= 0) {
        std::cout << "❌ Invalid burst time: " << process.burst_time << "\n";
        return false;
    }
    
    if (process.priority < 0) {
        std::cout << "❌ Invalid priority: " << process.priority << "\n";
        return false;
    }
    
    // Check for duplicate PID
    for (const auto& existing : processes) {
        if (existing.pid == process.pid) {
            std::cout << "❌ Duplicate PID: " << process.pid << "\n";
            return false;
        }
    }
    
    return true;
}

void ProcessScheduler::generateRandomProcessesInteractive() {
    int count;
    std::cout << "Enter number of random processes to generate: ";
    
    if (!(std::cin >> count)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "❌ Invalid input! Please enter a number.\n";
        return;
    }
    
    if (count <= 0 || count > 100) {
        std::cout << "❌ Please enter a number between 1-100.\n";
        return;
    }
    
    generateRandomProcesses(count);
    std::cout << "✅ Generated " << count << " random processes!\n";
}

void ProcessScheduler::deleteProcess(int process_id) {
    auto it = std::find_if(processes.begin(), processes.end(),
                          [process_id](const Process& p) { return p.pid == process_id; });
    
    if (it == processes.end()) {
        std::cout << "❌ Process P" << process_id << " not found!\n";
        return;
    }
    
    std::string process_name = it->process_name;
    processes.erase(it);
    std::cout << "✅ Deleted " << process_name << " (P" << process_id << ")\n";
}

void ProcessScheduler::clearAllProcesses() {
    if (processes.empty()) {
        std::cout << "📭 Process queue is already empty.\n";
        return;
    }
    
    int count = processes.size();
    processes.clear();
    clearGanttChart();
    std::cout << "✅ Cleared all " << count << " processes from the queue.\n";
}

void ProcessScheduler::resetScheduler() {
    processes.clear();
    clearGanttChart();
    current_time = 0;
    current_process = nullptr;
    
    // Clear the ready queue
    while (!ready_queue.empty()) {
        ready_queue.pop();
    }
}

void ProcessScheduler::resetProcessStates() {
    // Reset all processes to NEW state for fresh scheduling
    for (auto& process : processes) {
        process.state = NEW;
        process.remaining_time = process.burst_time;
        process.start_time = -1;
        process.completion_time = -1;
        process.waiting_time = 0;
        process.turnaround_time = 0;
    }
    
    clearGanttChart();
    current_time = 0;
    current_process = nullptr;
    
    // Clear the ready queue
    while (!ready_queue.empty()) {
        ready_queue.pop();
    }
}

// API helper methods
Process* ProcessScheduler::findProcess(int pid) {
    auto it = std::find_if(processes.begin(), processes.end(),
                          [pid](const Process& p) { return p.pid == pid; });
    return (it != processes.end()) ? &(*it) : nullptr;
}

bool ProcessScheduler::editProcessAPI(int pid, const std::string& name, int arrival, int burst, int priority) {
    Process* proc = findProcess(pid);
    if (!proc) {
        return false;
    }
    
    // Validate inputs
    if (arrival < 0 || burst <= 0 || priority < 0) {
        return false;
    }
    
    // Update process fields
    if (!name.empty()) {
        proc->process_name = name;
    }
    proc->arrival_time = arrival;
    proc->burst_time = burst;
    proc->priority = priority;
    proc->remaining_time = burst;
    proc->state = NEW;
    proc->start_time = -1;
    proc->completion_time = -1;
    proc->waiting_time = 0;
    proc->turnaround_time = 0;
    
    // Clear stale scheduling artifacts
    clearGanttChart();
    current_time = 0;
    current_process = nullptr;
    current_algorithm = "";
    
    return true;
}
