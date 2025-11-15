#include "cloud.h"
#include <iostream>
#include <string>

void run_cloud_simulator() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "CLOUD STORAGE SIMULATOR - INTERACTIVE MODE\n";
    std::cout << std::string(60, '=') << "\n";
    
    int choice;
    do {
        std::cout << "\n=== Cloud Storage Operations Menu ===\n";
        std::cout << "1. Upload File to Cloud\n";
        std::cout << "2. Download File from Cloud\n";
        std::cout << "3. Run Stress Test\n";
        std::cout << "4. Show Performance Report\n";
        std::cout << "5. Show Directory Structure\n";
        std::cout << "6. Reset Statistics\n";
        std::cout << "0. Exit Cloud Simulator\n";
        std::cout << "\nEnter your choice: ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(1024, '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        std::cin.ignore(1024, '\n');
        
        switch (choice) {
            case 1: {
                std::string filename;
                std::cout << "Enter filename to upload: ";
                std::getline(std::cin, filename);
                if (!filename.empty()) {
                    uploadFile(filename);
                } else {
                    std::cout << "Invalid filename.\n";
                }
                break;
            }
            case 2: {
                std::string filename;
                std::cout << "Enter filename to download: ";
                std::getline(std::cin, filename);
                if (!filename.empty()) {
                    downloadFile(filename);
                } else {
                    std::cout << "Invalid filename.\n";
                }
                break;
            }
            case 3: {
                int num_threads;
                std::cout << "Enter number of threads for stress test (1-1000): ";
                if (std::cin >> num_threads && num_threads > 0 && num_threads <= 1000) {
                    run_stress_test(num_threads);
                } else {
                    std::cout << "Invalid number. Using default: 50\n";
                    run_stress_test(50);
                }
                std::cin.ignore(1024, '\n');
                break;
            }
            case 4:
                print_performance_report();
                break;
            case 5:
                show_directory_structure();
                break;
            case 6:
                reset_statistics();
                std::cout << "Statistics reset successfully.\n";
                break;
            case 0:
                std::cout << "Exiting Cloud Simulator...\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);
}
