#ifndef OS_CONCEPTS_H
#define OS_CONCEPTS_H

#include "cloud.h"
#include "process_scheduler.h"
#include "file_system.h"
#include "ipc_manager.h"
#include "deadlock_detector.h"

// Forward declarations (defined in os_demo_ui.cpp)
extern ProcessScheduler process_scheduler;
extern FileSystem file_system;
extern IPCManager ipc_manager;
extern DeadlockDetector deadlock_detector;

// Demo function declarations
void run_process_scheduler_demo();
void run_file_system_demo();
void run_ipc_demo();
void run_deadlock_detection_demo();
void run_comprehensive_os_demo();

#endif
