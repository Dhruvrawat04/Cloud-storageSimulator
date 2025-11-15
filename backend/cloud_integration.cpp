#include "cloud.h"
#include <iostream>

// Implementation of cloud integration functions
void run_cloud_storage_demo() {
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "CLOUD STORAGE DEMONSTRATION - FULL INTERFACE\n";
    std::cout << std::string(50, '=') << "\n";
    
    // Launch the complete cloud simulator interface
    run_cloud_simulator();
}

void run_cloud_stress_test() {
    std::cout << "\n=== CLOUD STRESS TEST ===\n";
    int num_threads;
    std::cout << "Enter number of threads for stress test: ";
    std::cin >> num_threads;
    
    if (num_threads > 0 && num_threads <= 1000) {
        run_stress_test(num_threads);
    } else {
        std::cout << "Invalid number of threads. Using default: 50\n";
        run_stress_test(50);
    }
}

void show_cloud_performance_report() {
    std::cout << "\n=== CLOUD PERFORMANCE REPORT ===\n";
    print_performance_report();
}
