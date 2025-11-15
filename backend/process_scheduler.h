#ifndef PROCESS_SCHEDULER_H
#define PROCESS_SCHEDULER_H

#include <vector>
#include <queue>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <random>
#include <string>
#include <limits>

enum ProcessState {
    NEW, READY, RUNNING, WAITING, TERMINATED
};

struct Process {
    int pid;
    std::string process_name;  // ADDED: Process name field
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_time;
    int start_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
    ProcessState state;
    
    // Updated constructor with process name
    Process(int id, const std::string& name, int arrival, int burst, int pri = 0) 
        : pid(id), process_name(name), arrival_time(arrival), burst_time(burst), priority(pri),
          remaining_time(burst), start_time(-1), completion_time(-1),
          waiting_time(0), turnaround_time(0), state(NEW) {}
    
    // Backward compatible constructor
    Process(int id, int arrival, int burst, int pri = 0) 
        : pid(id), process_name("Process " + std::to_string(id)), arrival_time(arrival), burst_time(burst), priority(pri),
          remaining_time(burst), start_time(-1), completion_time(-1),
          waiting_time(0), turnaround_time(0), state(NEW) {}
};

struct GanttEntry {
    int process_id;
    std::string process_name;  // ADDED: Store process name in Gantt chart
    int start_time;
    int end_time;
    
    GanttEntry(int pid, const std::string& name, int start, int end)
        : process_id(pid), process_name(name), start_time(start), end_time(end) {}
};

class ProcessScheduler {
private:
    std::vector<Process> processes;
    std::queue<Process*> ready_queue;
    Process* current_process;
    int current_time;
    bool preemptive;
    std::vector<GanttEntry> gantt_chart;
    int next_pid;  // ADDED: Auto-incrementing PID counter
    std::string current_algorithm;

public:
    ProcessScheduler(bool preemptive = false) : current_process(nullptr), current_time(0), preemptive(preemptive), next_pid(1) {}
    
    // Scheduling algorithms
    void FCFS();
    void SJF();
    void PriorityScheduling();
    void RoundRobin(int time_quantum);
    
    // Process management
    void addProcess(const Process& process);
    void executeScheduler(const std::string& algorithm, int quantum = 2);
    void displayResults();
    void resetScheduler();
    void resetProcessStates();
    
    // Manual process management
    void addManualProcess();
    void editProcess(int process_id);
    void deleteProcess(int process_id);
    void clearAllProcesses();
    void displayCurrentProcesses();
    
    // Get current algorithm
    std::string getCurrentAlgorithm() const { return current_algorithm.empty() ? "None" : current_algorithm; }
    void generateRandomProcessesInteractive();
    bool validateProcess(const Process& process);
    
    // Gantt chart methods
    void displayGanttChart();
    void displayProcessTimeline();
    void displayVisualResults();
    void clearGanttChart();
    
    // Utility functions
    double getAverageWaitingTime();
    double getAverageTurnaroundTime();
    void generateRandomProcesses(int count);
    
    // Getter for processes (for UI)
    const std::vector<Process>& getProcesses() const { return processes; }
    const std::vector<GanttEntry>& getGanttChart() const { return gantt_chart; }
    
    // ADDED: Get next available PID
    int getNextPid() { return next_pid++; }
    
    // API helper methods
    Process* findProcess(int pid);
    bool editProcessAPI(int pid, const std::string& name, int arrival, int burst, int priority);
};

#endif
