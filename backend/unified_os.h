#ifndef UNIFIED_OS_H
#define UNIFIED_OS_H

#include "cloud.h"
#include "process_scheduler.h"
#include "file_system.h"
#include "ipc_manager.h"
#include "deadlock_detector.h"

// Global instances - DECLARATIONS ONLY (use extern)
extern ProcessScheduler process_scheduler;
extern FileSystem file_system;
extern IPCManager ipc_manager;
extern DeadlockDetector deadlock_detector;

// Function declarations from os_demo_ui.cpp
void run_process_scheduler_demo();
void run_file_system_demo();
void run_ipc_demo();
void run_deadlock_detection_demo();
void run_comprehensive_os_demo();

// NEW: Process management functions
void run_process_management_demo();

// Cloud functions
void run_cloud_storage_demo();
void run_cloud_stress_test();
void show_cloud_performance_report();

// Unified functions
void display_unified_menu();
void display_system_status();
void run_all_demos();

#endif
