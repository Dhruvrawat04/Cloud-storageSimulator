#ifndef DEADLOCK_DETECTOR_H
#define DEADLOCK_DETECTOR_H

#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <tuple>
#include <string>

struct Resource {
    int resource_id;
    int total_units;
    int available_units;
    std::string resource_name;
    
    Resource(int id, int total, const std::string& name = "")
        : resource_id(id), total_units(total), available_units(total), resource_name(name) {}
};

struct DLProcess {
    int process_id;
    std::string process_name;
    std::map<int, int> allocated;
    std::map<int, int> maximum;
    std::map<int, int> needed;
    
    DLProcess(int pid, const std::string& name) 
        : process_id(pid), process_name(name) {}
};

// RAG edge types: HOLD (process holds resource) and REQUEST (process requests resource)
struct RAGEdge {
    std::string type;  // "hold" or "request"
    int from_id;
    std::string from_type;  // "process" or "resource"
    std::string from_name;
    int to_id;
    std::string to_type;
    std::string to_name;
    int units;  // number of units allocated/requested
};

class DeadlockDetector {
private:
    std::vector<DLProcess> processes;
    std::vector<Resource> resources;
    std::map<int, std::vector<int>> wait_for_graph;

public:
    DeadlockDetector();
    
    // Resource management
    void addResource(int resource_id, int total_units, const std::string& name = "");
    void addProcess(int process_id, const std::string& name = "");
    
    // Resource allocation
    bool requestResource(int process_id, int resource_id, int units);
    bool releaseResource(int process_id, int resource_id, int units);
    
    // Deadlock detection algorithms
    bool detectDeadlock();
    std::vector<int> findDeadlockedProcesses();
    bool bankersAlgorithm(int process_id, int resource_id, int units);
    
    // Prevention and avoidance
    bool isSafeState();
    void recoverFromDeadlock();
    
    // Utility functions
    void displaySystemState();
    void displayResourceAllocation();
    void visualizeWaitForGraph();
    void simulateDeadlockScenario();
    
    // New graph visualization methods
    std::vector<RAGEdge> getResourceAllocationGraph();
    
    // Getters for HTTP API
    const std::vector<DLProcess>& getProcesses() const { return processes; }
    const std::vector<Resource>& getResources() const { return resources; }
    const std::map<int, std::vector<int>>& getWaitForGraph() { buildWaitForGraph(); return wait_for_graph; }
    
private:
    void buildWaitForGraph();
    bool hasCycle(int node, std::vector<bool>& visited, std::vector<bool>& rec_stack);
};

#endif
