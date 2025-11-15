#include "unified_os.h"
#include "cloud.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

// Global variables are declared via unified_os.h (extern)

void display_system_status() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "SYSTEM STATUS OVERVIEW\n";
    std::cout << std::string(60, '=') << "\n";
    
    std::cout << "✓ Process Scheduler: Ready (with Gantt Charts & Manual Entry)\n";
    std::cout << "✓ File System: Ready\n";
    std::cout << "✓ IPC Manager: Ready\n";
    std::cout << "✓ Deadlock Detector: Ready (with Visualizations)\n";
    std::cout << "✓ Cloud Storage: Ready\n";
    
    std::cout << "\n🎯 KEY FEATURES:\n";
    std::cout << "  • Gantt Chart Visualizations\n";
    std::cout << "  • Manual Process Entry & Management\n";
    std::cout << "  • Wait-For Graph Visualization\n";
    std::cout << "  • Interactive Deadlock Scenarios\n";
    std::cout << "  • Real-time System Monitoring\n";
    
    std::cout << "\nAll Modules Integrated Successfully!\n";
}

void run_all_demos() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "RUNNING COMPLETE DEMONSTRATION SUITE\n";
    std::cout << std::string(60, '=') << "\n";
    
    // Cloud Storage Demo
    std::cout << "\n>>> STARTING CLOUD STORAGE DEMO <<<\n";
    run_cloud_storage_demo();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Process Scheduling with new features
    std::cout << "\n>>> STARTING ENHANCED PROCESS SCHEDULING DEMO <<<\n";
    run_process_scheduler_demo();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // File System
    std::cout << "\n>>> STARTING FILE SYSTEM DEMO <<<\n";
    run_file_system_demo();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // IPC
    std::cout << "\n>>> STARTING IPC DEMO <<<\n";
    run_ipc_demo();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // Deadlock Detection
    std::cout << "\n>>> STARTING DEADLOCK DETECTION DEMO <<<\n";
    run_deadlock_detection_demo();
    
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "ALL DEMOS COMPLETED SUCCESSFULLY!\n";
    std::cout << std::string(60, '=') << "\n";
}

void display_unified_menu() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "UNIFIED OPERATING SYSTEM SIMULATOR v2.2\n";
    std::cout << "🎯 Enhanced with Visualizations & Interactive Demos!\n";
    std::cout << std::string(60, '=') << "\n";
    
    std::cout << "\nCLOUD STORAGE MODULE:\n";
    std::cout << " 1. Cloud Storage Operations\n";
    std::cout << " 2. Run Cloud Stress Test\n";
    std::cout << " 3. Cloud Performance Report\n";
    
    std::cout << "\nPROCESS SCHEDULING MODULE:\n";
    std::cout << " 4. ⚙️  Process Management Console (Manual Entry)\n";
    std::cout << " 5. 📊 Process Scheduling Demo (Predefined)\n";
    
    std::cout << "\nOS CONCEPTS MODULE:\n";
    std::cout << " 6. File System Demo\n";
    std::cout << " 7. IPC Mechanisms Demo\n";
    std::cout << " 8. Deadlock Detection Demo\n";
    std::cout << " 9. Comprehensive OS Demo\n";
    
    std::cout << "\nENHANCED VISUALIZATIONS:\n";
    std::cout << "10. 📈 View Wait-For Graph\n";
    std::cout << "11. 🔄 Run Deadlock Scenario\n";
    
    std::cout << "\nSYSTEM TOOLS:\n";
    std::cout << "12. System Status\n";
    std::cout << "13. Run All Demos\n";
    std::cout << "14. Exit\n";
    
    std::cout << "\nEnter choice (1-14): ";
}

// Enhanced deadlock demonstration with visualizations
void run_enhanced_deadlock_demo() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "ENHANCED DEADLOCK DETECTION DEMONSTRATION\n";
    std::cout << std::string(60, '=') << "\n";
    
    // Display initial state
    std::cout << "Initial System State:\n";
    deadlock_detector.displaySystemState();
    
    // Show wait-for graph visualization
    std::cout << "\n--- Wait-For Graph Visualization ---\n";
    deadlock_detector.visualizeWaitForGraph();
    
    // Run deadlock scenario
    std::cout << "\n--- Deadlock Scenario Simulation ---\n";
    deadlock_detector.simulateDeadlockScenario();
    
    // Show recovery
    std::cout << "\n--- Deadlock Recovery ---\n";
    deadlock_detector.recoverFromDeadlock();
    
    // Final state
    std::cout << "\n--- Final System State ---\n";
    deadlock_detector.displaySystemState();
}

// MAIN FUNCTION
int main() {
    int choice;
    
    std::cout << "=== UNIFIED OS SIMULATOR v2.2 INITIALIZED ===\n";
    std::cout << "🎯 Now with Enhanced Visualizations & Interactive Demos!\n";
    std::cout << "System Version: 2.2 | Modules: 5 | Status: OPERATIONAL\n";
    
    // Initialize directories
    ensure_directories_exist();
    
    while (true) {
        display_unified_menu();
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(1024, '\n');
            std::cout << "❌ Invalid input. Please enter a number (1-14).\n";
            continue;
        }
        
        switch (choice) {
            case 1:
                run_cloud_storage_demo();
                break;
            case 2:
                run_cloud_stress_test();
                break;
            case 3:
                show_cloud_performance_report();
                break;
            case 4:
                run_process_management_demo();
                break;
            case 5:
                run_process_scheduler_demo();
                break;
            case 6:
                run_file_system_demo();
                break;
            case 7:
                run_ipc_demo();
                break;
            case 8:
                run_enhanced_deadlock_demo();  // Use enhanced version
                break;
            case 9:
                run_comprehensive_os_demo();
                break;
            case 10:
                deadlock_detector.visualizeWaitForGraph();
                break;
            case 11:
                deadlock_detector.simulateDeadlockScenario();
                break;
            case 12:
                display_system_status();
                break;
            case 13:
                run_all_demos();
                break;
            case 14:
                std::cout << "\nThank you for using Unified OS Simulator v2.2!\n";
                std::cout << "Enhanced with Visualizations & Interactive Demos\n";
                std::cout << "Exiting gracefully...\n";
                return 0;
            default:
                std::cout << "❌ Invalid choice. Please enter a number between 1-14.\n";
        }
        
        // Pause for readability
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    }
    
    return 0;
}
