const API_BASE_URL = '/api';

export interface BackendStats {
  totalFiles: number;
  totalSize: string;
  cloudDataSize: number;
  activeReaders: number;
  activeWriters: number;
  activeDeleters: number;
  completedReads: number;
  completedWrites: number;
  completedDeletes: number;
  activeThreads: number;
}

export interface BackendThread {
  id: number;
  status: string;
}

export interface BackendLog {
  message: string;
  timestamp: string;
}

export interface BackendFile {
  id: string;
  name: string;
  size: number;
  modified: string;
  type: string;
}

export interface ProcessDetail {
  pid: number;
  processName?: string;
  arrivalTime: number;
  burstTime: number;
  priority: number;
  startTime: number;
  completionTime: number;
  waitingTime: number;
  turnaroundTime: number;
}

export interface GanttEntry {
  processId: number;
  processName: string;
  startTime: number;
  endTime: number;
}

export interface OSProcessStats {
  averageWaitingTime: number;
  averageTurnaroundTime: number;
  processCount: number;
  algorithm: string;
  processes?: ProcessDetail[];
  ganttChart?: GanttEntry[];
}

export interface OSFileSystemStats {
  totalBlocks: number;
  blockSize: number;
  utilization: number;
  status: string;
}

export interface OSIPCStats {
  messageQueues: number;
  sharedMemorySegments: number;
  status: string;
}

export interface OSDeadlockStats {
  hasDeadlock: boolean;
  safeState: boolean;
  status: string;
}

export const cloudApi = {
  // Statistics
  async getStats(): Promise<BackendStats> {
    const response = await fetch(`${API_BASE_URL}/stats`);
    if (!response.ok) throw new Error('Failed to fetch stats');
    return response.json();
  },

  // Files
  async getFiles(): Promise<BackendFile[]> {
    const response = await fetch(`${API_BASE_URL}/files`);
    if (!response.ok) throw new Error('Failed to fetch files');
    const data = await response.json();
    return data.files;
  },

  async uploadFile(file: File): Promise<void> {
    const formData = new FormData();
    formData.append('file', file);
    
    const response = await fetch(`${API_BASE_URL}/files/upload`, {
      method: 'POST',
      body: await file.text(), // Send file content directly
      headers: {
        'Content-Type': 'text/plain',
      }
    });
    
    if (!response.ok) throw new Error('Failed to upload file');
  },

  async deleteFile(fileId: string): Promise<void> {
    const response = await fetch(`${API_BASE_URL}/files/${fileId}`, {
      method: 'DELETE',
    });
    if (!response.ok) throw new Error('Failed to delete file');
  },

  // Threads
  async getThreads(): Promise<BackendThread[]> {
    const response = await fetch(`${API_BASE_URL}/threads`);
    if (!response.ok) throw new Error('Failed to fetch threads');
    const data = await response.json();
    return data.threads;
  },

  async spawnThread(type: 'READER' | 'WRITER' | 'DELETER'): Promise<{ threadId: number }> {
    const response = await fetch(`${API_BASE_URL}/threads/spawn`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({ type }),
    });
    
    if (!response.ok) throw new Error('Failed to spawn thread');
    return response.json();
  },

  async runStressTest(count: number): Promise<void> {
    const response = await fetch(`${API_BASE_URL}/threads/stress-test`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({ count }),
    });
    
    if (!response.ok) throw new Error('Failed to start stress test');
  },

  // Logs
  async getLogs(): Promise<BackendLog[]> {
    const response = await fetch(`${API_BASE_URL}/logs`);
    if (!response.ok) throw new Error('Failed to fetch logs');
    const data = await response.json();
    return data.logs;
  },

  // Health check
  async checkHealth(): Promise<{ status: string }> {
    const response = await fetch(`${API_BASE_URL}/health`);
    if (!response.ok) throw new Error('Backend is not responding');
    return response.json();
  },

  // OS Module APIs
  // Process Scheduler
  async getProcessStats(): Promise<OSProcessStats> {
    const response = await fetch(`${API_BASE_URL}/os/processes`);
    if (!response.ok) throw new Error('Failed to fetch process stats');
    return response.json();
  },

  async scheduleProcesses(algorithm: string, quantum: number = 2, processCount: number = 5): Promise<OSProcessStats> {
    const response = await fetch(`${API_BASE_URL}/os/processes/schedule`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ algorithm, quantum, processCount }),
    });
    if (!response.ok) throw new Error('Failed to schedule processes');
    return response.json();
  },

  async addManualProcess(processName: string, arrivalTime: number, burstTime: number, priority: number): Promise<{ success: boolean; process: ProcessDetail }> {
    const response = await fetch(`${API_BASE_URL}/os/processes/add`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ processName, arrivalTime, burstTime, priority }),
    });
    if (!response.ok) throw new Error('Failed to add process');
    return response.json();
  },

  async editProcess(pid: number, processName: string, arrivalTime: number, burstTime: number, priority: number): Promise<{ success: boolean; process: ProcessDetail }> {
    const response = await fetch(`${API_BASE_URL}/os/processes/edit/${pid}`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ processName, arrivalTime, burstTime, priority }),
    });
    if (!response.ok) throw new Error('Failed to edit process');
    return response.json();
  },

  async deleteProcess(pid: number): Promise<{ success: boolean; deletedProcess: { pid: number; processName: string } }> {
    const response = await fetch(`${API_BASE_URL}/os/processes/${pid}`, {
      method: 'DELETE',
    });
    if (!response.ok) throw new Error('Failed to delete process');
    return response.json();
  },

  async getIPCMessages(): Promise<{ totalMessages: number; queues: number; sharedMemorySegments: number; status: string }> {
    const response = await fetch(`${API_BASE_URL}/os/ipc/messages`);
    if (!response.ok) throw new Error('Failed to fetch IPC messages');
    return response.json();
  },

  // File System
  async getFileSystemStats(): Promise<OSFileSystemStats> {
    const response = await fetch(`${API_BASE_URL}/os/filesystem`);
    if (!response.ok) throw new Error('Failed to fetch filesystem stats');
    return response.json();
  },

  async createOSFile(path: string, data: string): Promise<{ success: boolean; utilization: number }> {
    const response = await fetch(`${API_BASE_URL}/os/filesystem/create`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ path, data }),
    });
    if (!response.ok) throw new Error('Failed to create file');
    return response.json();
  },

  // IPC Manager
  async getIPCStats(): Promise<OSIPCStats> {
    const response = await fetch(`${API_BASE_URL}/os/ipc`);
    if (!response.ok) throw new Error('Failed to fetch IPC stats');
    return response.json();
  },

  async sendIPCMessage(queueId: number, sender: number, receiver: number, content: string): Promise<{ success: boolean }> {
    const response = await fetch(`${API_BASE_URL}/os/ipc/message`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ queueId, sender, receiver, content }),
    });
    if (!response.ok) throw new Error('Failed to send IPC message');
    return response.json();
  },

  // Deadlock Detector
  async getDeadlockStats(): Promise<OSDeadlockStats> {
    const response = await fetch(`${API_BASE_URL}/os/deadlock`);
    if (!response.ok) throw new Error('Failed to fetch deadlock stats');
    return response.json();
  },

  async simulateDeadlock(): Promise<{ success: boolean; deadlockCreated: boolean }> {
    const response = await fetch(`${API_BASE_URL}/os/deadlock/simulate`, {
      method: 'POST',
    });
    if (!response.ok) throw new Error('Failed to simulate deadlock');
    return response.json();
  },

  async visualizeDeadlock(): Promise<any> {
    const response = await fetch(`${API_BASE_URL}/os/deadlock/visualize`);
    if (!response.ok) throw new Error('Failed to visualize deadlock');
    return response.json();
  },

  async recoverFromDeadlock(): Promise<{ success: boolean; processesTerminated: number; stillDeadlocked: boolean }> {
    const response = await fetch(`${API_BASE_URL}/os/deadlock/recover`, {
      method: 'POST',
    });
    if (!response.ok) throw new Error('Failed to recover from deadlock');
    return response.json();
  },

  async createSharedMemory(name: string, size: number, data?: string): Promise<{ success: boolean; name: string; size: number }> {
    const response = await fetch(`${API_BASE_URL}/os/ipc/shared-memory`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ name, size, data }),
    });
    if (!response.ok) throw new Error('Failed to create shared memory');
    return response.json();
  },

  async readSharedMemory(name: string): Promise<{ success: boolean; name: string; data: string }> {
    const response = await fetch(`${API_BASE_URL}/os/ipc/shared-memory?name=${encodeURIComponent(name)}`);
    if (!response.ok) throw new Error('Failed to read shared memory');
    return response.json();
  },

  async writeSharedMemory(name: string, data: string): Promise<{ success: boolean; name: string }> {
    const response = await fetch(`${API_BASE_URL}/os/ipc/shared-memory/write`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ name, data }),
    });
    if (!response.ok) throw new Error('Failed to write shared memory');
    return response.json();
  },

  // Run OS simulations
  async runOSSimulation(module: string = 'all'): Promise<{ success: boolean; module: string }> {
    const response = await fetch(`${API_BASE_URL}/os/simulate`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ module }),
    });
    if (!response.ok) throw new Error('Failed to run OS simulation');
    return response.json();
  },
};
