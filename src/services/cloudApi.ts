const API_BASE_URL = 'http://localhost:3001/api';

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
};
