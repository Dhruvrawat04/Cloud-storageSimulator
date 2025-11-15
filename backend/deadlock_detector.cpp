#include "deadlock_detector.h"
#include <iostream>
#include <iomanip>

DeadlockDetector::DeadlockDetector() {
    // Initialize with basic resources (1 unit each for proper circular deadlock)
    addResource(1, 1, "Printer");
    addResource(2, 1, "Scanner");
    addResource(3, 1, "USB_Drive");
    addResource(4, 1, "Network_Port");
}

void DeadlockDetector::addResource(int resource_id, int total_units, const std::string& name) {
    resources.emplace_back(resource_id, total_units, name);
    std::cout << "✓ Added resource: " << name << " (" << total_units << " units)\n";
}

void DeadlockDetector::addProcess(int process_id, const std::string& name) {
    processes.emplace_back(process_id, name);
    std::cout << "✓ Added process: " << name << "\n";
}

bool DeadlockDetector::requestResource(int process_id, int resource_id, int units) {
    auto proc_it = std::find_if(processes.begin(), processes.end(),
                               [process_id](const DLProcess& p) { return p.process_id == process_id; });
    auto res_it = std::find_if(resources.begin(), resources.end(),
                              [resource_id](const Resource& r) { return r.resource_id == resource_id; });
    
    if (proc_it == processes.end() || res_it == resources.end()) {
        std::cout << "❌ Invalid process or resource ID\n";
        return false;
    }
    
    DLProcess& process = *proc_it;
    Resource& resource = *res_it;
    
    // Check if request can be granted
    if (units > resource.available_units) {
        std::cout << "⏳ Process " << process_id << " must wait for resource " 
                  << resource_id << " (only " << resource.available_units << " available)\n";
        
        // Record the blocked request: update maximum and needed to reflect the wait state
        int current_allocated = process.allocated[resource_id];
        int current_maximum = (process.maximum.find(resource_id) != process.maximum.end()) 
                            ? process.maximum[resource_id] : 0;
        process.maximum[resource_id] = std::max(current_maximum, current_allocated + units);
        process.needed[resource_id] = process.maximum[resource_id] - current_allocated;
        
        return false;
    }
    
    // Update allocation
    process.allocated[resource_id] += units;
    resource.available_units -= units;
    
    // Initialize maximum and needed if not set
    if (process.maximum.find(resource_id) == process.maximum.end()) {
        process.maximum[resource_id] = units;
    }
    process.needed[resource_id] = process.maximum[resource_id] - process.allocated[resource_id];
    
    std::cout << "✓ Allocated " << units << " units of " << resource.resource_name 
              << " to " << process.process_name << "\n";
    return true;
}

bool DeadlockDetector::releaseResource(int process_id, int resource_id, int units) {
    auto proc_it = std::find_if(processes.begin(), processes.end(),
                               [process_id](const DLProcess& p) { return p.process_id == process_id; });
    auto res_it = std::find_if(resources.begin(), resources.end(),
                              [resource_id](const Resource& r) { return r.resource_id == resource_id; });
    
    if (proc_it == processes.end() || res_it == resources.end()) {
        std::cout << "❌ Invalid process or resource ID\n";
        return false;
    }
    
    DLProcess& process = *proc_it;
    Resource& resource = *res_it;
    
    if (process.allocated[resource_id] < units) {
        std::cout << "❌ Process " << process_id << " doesn't have " << units 
                  << " units allocated\n";
        return false;
    }
    
    process.allocated[resource_id] -= units;
    resource.available_units += units;
    
    std::cout << "✓ Released " << units << " units from " << process.process_name << "\n";
    return true;
}

bool DeadlockDetector::detectDeadlock() {
    buildWaitForGraph();
    
    int n = processes.size();
    if (n == 0) return false;
    
    std::vector<bool> visited(n, false);
    std::vector<bool> rec_stack(n, false);
    
    for (int i = 0; i < n; i++) {
        if (hasCycle(i, visited, rec_stack)) {
            return true;
        }
    }
    return false;
}

void DeadlockDetector::buildWaitForGraph() {
    wait_for_graph.clear();
    
    for (const auto& process : processes) {
        std::vector<int> waiting_for;
        
        for (const auto& alloc : process.allocated) {
            int resource_id = alloc.first;
            
            auto res_it = std::find_if(resources.begin(), resources.end(),
                                     [resource_id](const Resource& r) { return r.resource_id == resource_id; });
            
            auto needed_it = process.needed.find(resource_id);
            int needed_units = (needed_it != process.needed.end()) ? needed_it->second : 0;
            
            if (res_it != resources.end() && needed_units > res_it->available_units) {
                for (const auto& other_process : processes) {
                    if (other_process.process_id != process.process_id) {
                        auto other_alloc_it = other_process.allocated.find(resource_id);
                        if (other_alloc_it != other_process.allocated.end() && other_alloc_it->second > 0) {
                            waiting_for.push_back(other_process.process_id);
                        }
                    }
                }
            }
        }
        
        wait_for_graph[process.process_id] = waiting_for;
    }
}

bool DeadlockDetector::hasCycle(int node, std::vector<bool>& visited, std::vector<bool>& rec_stack) {
    if (node < 0 || node >= static_cast<int>(processes.size())) return false;
    
    if (!visited[node]) {
        visited[node] = true;
        rec_stack[node] = true;
        
        int process_id = processes[node].process_id;
        auto wait_it = wait_for_graph.find(process_id);
        
        if (wait_it != wait_for_graph.end()) {
            for (int neighbor_id : wait_it->second) {
                auto neighbor_it = std::find_if(processes.begin(), processes.end(),
                                              [neighbor_id](const DLProcess& p) { return p.process_id == neighbor_id; });
                if (neighbor_it != processes.end()) {
                    int neighbor_index = std::distance(processes.begin(), neighbor_it);
                    if (!visited[neighbor_index] && hasCycle(neighbor_index, visited, rec_stack)) {
                        return true;
                    } else if (rec_stack[neighbor_index]) {
                        return true;
                    }
                }
            }
        }
    }
    
    rec_stack[node] = false;
    return false;
}

std::vector<int> DeadlockDetector::findDeadlockedProcesses() {
    std::vector<int> deadlocked;
    
    if (detectDeadlock()) {
        for (const auto& process : processes) {
            if (!process.allocated.empty()) {
                bool is_waiting = false;
                for (const auto& alloc : process.allocated) {
                    auto needed_it = process.needed.find(alloc.first);
                    if (needed_it != process.needed.end() && needed_it->second > 0) {
                        is_waiting = true;
                        break;
                    }
                }
                if (is_waiting) {
                    deadlocked.push_back(process.process_id);
                }
            }
        }
    }
    
    return deadlocked;
}

void DeadlockDetector::recoverFromDeadlock() {
    auto deadlocked = findDeadlockedProcesses();
    if (deadlocked.empty()) {
        std::cout << "✓ No deadlock to recover from\n";
        return;
    }
    
    // Simple recovery: terminate first deadlocked process
    int victim = deadlocked.front();
    auto it = std::find_if(processes.begin(), processes.end(),
                          [victim](const DLProcess& p) { return p.process_id == victim; });
    
    if (it != processes.end()) {
        std::cout << "🔄 Terminating " << it->process_name << " to break deadlock...\n";
        
        // Release all resources
        for (const auto& alloc : it->allocated) {
            releaseResource(victim, alloc.first, alloc.second);
        }
        
        processes.erase(it);
        std::cout << "✓ Deadlock resolved!\n";
    }
}

void DeadlockDetector::displaySystemState() {
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "SYSTEM STATE\n";
    std::cout << std::string(50, '=') << "\n";
    
    std::cout << "Resources:\n";
    for (const auto& resource : resources) {
        std::cout << "  " << resource.resource_name 
                  << ": " << resource.available_units << "/" << resource.total_units << " available\n";
    }
    
    std::cout << "\nProcesses:\n";
    for (const auto& process : processes) {
        std::cout << "  " << process.process_name << ":\n";
        std::cout << "    Allocated: ";
        for (const auto& alloc : process.allocated) {
            auto res_it = std::find_if(resources.begin(), resources.end(),
                                     [alloc](const Resource& r) { return r.resource_id == alloc.first; });
            std::cout << (res_it != resources.end() ? res_it->resource_name : "R" + std::to_string(alloc.first)) 
                      << "=" << alloc.second << " ";
        }
        std::cout << "\n    Needed: ";
        for (const auto& need : process.needed) {
            if (need.second > 0) {
                auto res_it = std::find_if(resources.begin(), resources.end(),
                                         [need](const Resource& r) { return r.resource_id == need.first; });
                std::cout << (res_it != resources.end() ? res_it->resource_name : "R" + std::to_string(need.first)) 
                          << "=" << need.second << " ";
            }
        }
        std::cout << "\n";
    }
    
    if (detectDeadlock()) {
        std::cout << "\n🚨 DEADLOCK DETECTED!\n";
    } else {
        std::cout << "\n✓ System is safe\n";
    }
}

void DeadlockDetector::displayResourceAllocation() {
    std::cout << "\n=== RESOURCE ALLOCATION MATRIX ===\n";
    
    std::cout << "Process\\Resource";
    for (const auto& resource : resources) {
        std::cout << std::setw(8) << ("R" + std::to_string(resource.resource_id));
    }
    std::cout << "\n";
    
    for (const auto& process : processes) {
        std::cout << "P" << std::setw(14) << process.process_id;
        for (const auto& resource : resources) {
            auto it = process.allocated.find(resource.resource_id);
            std::cout << std::setw(8) << (it != process.allocated.end() ? it->second : 0);
        }
        std::cout << "\n";
    }
}

void DeadlockDetector::visualizeWaitForGraph() {
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "WAIT-FOR GRAPH VISUALIZATION\n";
    std::cout << std::string(50, '=') << "\n";
    
    // First, ensure we have a system state to visualize
    if (processes.empty() || resources.empty()) {
        std::cout << "⚠️  No processes or resources defined. Creating sample scenario...\n";
        
        // Create a sample deadlock scenario
        addResource(1, 1, "Printer");
        addResource(2, 1, "Scanner");
        addProcess(1, "Process A");
        addProcess(2, "Process B");
        
        // Create the classic deadlock
        requestResource(1, 1, 1);  // Process A gets Printer
        requestResource(2, 2, 1);  // Process B gets Scanner
        
        // Set up the waiting relationships
        auto procA = std::find_if(processes.begin(), processes.end(),
                                [](const DLProcess& p) { return p.process_id == 1; });
        auto procB = std::find_if(processes.begin(), processes.end(),
                                [](const DLProcess& p) { return p.process_id == 2; });
        
        if (procA != processes.end()) {
            procA->maximum[2] = 1; // Process A needs Scanner
            procA->needed[2] = 1;
        }
        if (procB != processes.end()) {
            procB->maximum[1] = 1; // Process B needs Printer  
            procB->needed[1] = 1;
        }
    }
    
    buildWaitForGraph();
    
    std::cout << "\n🔍 CURRENT WAIT-FOR RELATIONSHIPS:\n";
    std::cout << std::string(40, '-') << "\n";
    
    bool has_edges = false;
    for (const auto& entry : wait_for_graph) {
        int process_id = entry.first;
        const auto& waiting_for = entry.second;
        if (!waiting_for.empty()) {
            has_edges = true;
            auto proc_it = std::find_if(processes.begin(), processes.end(),
                                      [process_id](const DLProcess& p) { return p.process_id == process_id; });
            std::cout << "🔄 " << (proc_it != processes.end() ? proc_it->process_name : "P" + std::to_string(process_id)) 
                      << " is waiting for: ";
            
            for (size_t i = 0; i < waiting_for.size(); i++) {
                auto wait_it = std::find_if(processes.begin(), processes.end(),
                                          [waiting_for, i](const DLProcess& p) { return p.process_id == waiting_for[i]; });
                std::cout << (wait_it != processes.end() ? wait_it->process_name : "P" + std::to_string(waiting_for[i]));
                if (i < waiting_for.size() - 1) std::cout << ", ";
            }
            std::cout << "\n";
        }
    }
    
    if (!has_edges) {
        std::cout << "✅ No circular wait dependencies detected\n";
        std::cout << "💡 The system is currently deadlock-free!\n";
    } else {
        // Check for cycles
        if (detectDeadlock()) {
            std::cout << "\n🚨 DEADLOCK DETECTED IN WAIT-FOR GRAPH!\n";
            std::cout << "💥 Circular wait condition exists!\n";
        } else {
            std::cout << "\n⚠️  Wait dependencies exist but no deadlock detected\n";
        }
    }
    
    // Show visual graph representation
    std::cout << "\n📊 GRAPH REPRESENTATION:\n";
    std::cout << std::string(40, '-') << "\n";
    
    for (const auto& process : processes) {
        std::cout << "• " << process.process_name << " [P" << process.process_id << "]\n";
        
        auto wait_it = wait_for_graph.find(process.process_id);
        if (wait_it != wait_for_graph.end() && !wait_it->second.empty()) {
            for (int waiting_for_id : wait_it->second) {
                auto target_proc = std::find_if(processes.begin(), processes.end(),
                                              [waiting_for_id](const DLProcess& p) { return p.process_id == waiting_for_id; });
                std::cout << "    ↓ waiting for → " 
                          << (target_proc != processes.end() ? target_proc->process_name : "P" + std::to_string(waiting_for_id))
                          << "\n";
            }
        } else {
            std::cout << "    ✓ Not waiting for any processes\n";
        }
    }
    
    // Show resource allocation status
    std::cout << "\n🔗 RESOURCE ALLOCATION SUMMARY:\n";
    std::cout << std::string(40, '-') << "\n";
    for (const auto& process : processes) {
        std::cout << "• " << process.process_name << " holds: ";
        bool holds_resources = false;
        for (const auto& alloc : process.allocated) {
            auto res_it = std::find_if(resources.begin(), resources.end(),
                                     [alloc](const Resource& r) { return r.resource_id == alloc.first; });
            if (res_it != resources.end() && alloc.second > 0) {
                std::cout << res_it->resource_name << "(" << alloc.second << ") ";
                holds_resources = true;
            }
        }
        if (!holds_resources) std::cout << "nothing";
        std::cout << "\n";
        
        std::cout << "  Needs: ";
        bool needs_resources = false;
        for (const auto& need : process.needed) {
            if (need.second > 0) {
                auto res_it = std::find_if(resources.begin(), resources.end(),
                                         [need](const Resource& r) { return r.resource_id == need.first; });
                if (res_it != resources.end()) {
                    std::cout << res_it->resource_name << "(" << need.second << ") ";
                    needs_resources = true;
                }
            }
        }
        if (!needs_resources) std::cout << "nothing";
        std::cout << "\n";
    }
}

void DeadlockDetector::simulateDeadlockScenario() {
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "DEADLOCK SCENARIO SIMULATION (Enhanced)\n";
    std::cout << std::string(50, '=') << "\n";
    
    processes.clear();
    resources.clear();
    
    // Use 1 unit per resource to create true circular deadlock
    addResource(1, 1, "Printer");
    addResource(2, 1, "Scanner");
    addResource(3, 1, "USB_Drive");
    addResource(4, 1, "Network_Port");
    
    addProcess(1, "P1");
    addProcess(2, "P2");
    addProcess(3, "P3");
    addProcess(4, "P4");
    
    std::cout << "\nStep 1: P1 acquires 1 Printer\n";
    requestResource(1, 1, 1);
    
    std::cout << "\nStep 2: P2 acquires 1 Scanner\n";
    requestResource(2, 2, 1);
    
    std::cout << "\nStep 3: P3 acquires USB_Drive\n";
    requestResource(3, 3, 1);
    
    std::cout << "\nStep 4: P4 acquires Network_Port\n";
    requestResource(4, 4, 1);
    
    std::cout << "\nStep 5: P1 requests 1 Scanner (waits for P2)\n";
    requestResource(1, 2, 1);
    
    std::cout << "\nStep 6: P2 requests USB_Drive (waits for P3)\n";
    requestResource(2, 3, 1);
    
    std::cout << "\nStep 7: P3 requests Network_Port (waits for P4)\n";
    requestResource(3, 4, 1);
    
    std::cout << "\nStep 8: P4 requests 1 Printer (CIRCULAR DEADLOCK!)\n";
    requestResource(4, 1, 1);
    
    displaySystemState();
}


bool DeadlockDetector::bankersAlgorithm(int process_id, int resource_id, int units) {
    std::cout << "🔒 Banker's Algorithm: Checking safety for request...\n";
    
    // Simplified safety check
    for (const auto& resource : resources) {
        if (resource.resource_id == resource_id) {
            if (units > resource.available_units) {
                std::cout << "❌ Request would make system unsafe\n";
                return false;
            }
        }
    }
    
    std::cout << "✓ Request is safe to grant\n";
    return true;
}

bool DeadlockDetector::isSafeState() {
    return !detectDeadlock();
}

// Generate Resource Allocation Graph data for visualization
std::vector<RAGEdge> DeadlockDetector::getResourceAllocationGraph() {
    std::vector<RAGEdge> edges;
    
    // Add edges for allocated resources (Resource -> Process, type "hold")
    for (const auto& proc : processes) {
        for (const auto& alloc : proc.allocated) {
            if (alloc.second > 0) {
                int resource_id = alloc.first;
                auto res_it = std::find_if(resources.begin(), resources.end(),
                    [resource_id](const Resource& r) { return r.resource_id == resource_id; });
                
                if (res_it != resources.end()) {
                    RAGEdge edge;
                    edge.type = "hold";
                    edge.from_id = resource_id;
                    edge.from_type = "resource";
                    edge.from_name = res_it->resource_name;
                    edge.to_id = proc.process_id;
                    edge.to_type = "process";
                    edge.to_name = proc.process_name;
                    edge.units = alloc.second;
                    edges.push_back(edge);
                }
            }
        }
    }
    
    // Add edges for requested resources (Process -> Resource, type "request")
    for (const auto& proc : processes) {
        for (const auto& need : proc.needed) {
            if (need.second > 0) {
                int resource_id = need.first;
                auto res_it = std::find_if(resources.begin(), resources.end(),
                    [resource_id](const Resource& r) { return r.resource_id == resource_id; });
                
                if (res_it != resources.end()) {
                    RAGEdge edge;
                    edge.type = "request";
                    edge.from_id = proc.process_id;
                    edge.from_type = "process";
                    edge.from_name = proc.process_name;
                    edge.to_id = resource_id;
                    edge.to_type = "resource";
                    edge.to_name = res_it->resource_name;
                    edge.units = need.second;
                    edges.push_back(edge);
                }
            }
        }
    }
    
    return edges;
}
