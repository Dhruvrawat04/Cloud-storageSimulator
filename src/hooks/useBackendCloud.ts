import { useState, useEffect, useCallback } from 'react';
import { cloudApi, type BackendStats } from '@/services/cloudApi';
import { useToast } from '@/hooks/use-toast';

export interface CloudLog {
  id: string;
  timestamp: string;
  threadId: number;
  threadType: string;
  action: string;
  status: string;
  details?: string;
}

export interface ThreadInfo {
  id: number;
  type: 'READER' | 'WRITER' | 'DELETER';
  status: 'RUNNING' | 'COMPLETED' | 'ERROR';
  startTime: string;
  progress?: number;
}

export const useBackendCloud = () => {
  const [stats, setStats] = useState<BackendStats | null>(null);
  const [logs, setLogs] = useState<CloudLog[]>([]);
  const [threads, setThreads] = useState<ThreadInfo[]>([]);
  const [files, setFiles] = useState<any[]>([]);
  const [cloudData, setCloudData] = useState('');
  const [isConnected, setIsConnected] = useState(false);
  const [shouldPollThreads, setShouldPollThreads] = useState(true);
  const { toast } = useToast();

  // Check backend health
  const checkConnection = useCallback(async () => {
    try {
      await cloudApi.checkHealth();
      setIsConnected(true);
    } catch (error) {
      setIsConnected(false);
      console.error('Backend connection failed:', error);
    }
  }, []);

  // Fetch statistics
  const fetchStats = useCallback(async () => {
    try {
      const data = await cloudApi.getStats();
      setStats(data);
    } catch (error) {
      console.error('Failed to fetch stats:', error);
    }
  }, []);

  // Fetch logs
  const fetchLogs = useCallback(async () => {
    try {
      const data = await cloudApi.getLogs();
      const formattedLogs: CloudLog[] = data.map((log, index) => ({
        id: `log-${index}`,
        timestamp: log.timestamp || new Date().toLocaleTimeString(),
        threadId: 0,
        threadType: 'SYSTEM',
        action: 'LOG',
        status: 'INFO',
        details: log.message,
      }));
      setLogs(formattedLogs);
    } catch (error) {
      console.error('Failed to fetch logs:', error);
    }
  }, []);

  // Fetch threads
  const fetchThreads = useCallback(async () => {
    try {
      const data = await cloudApi.getThreads();
      const formattedThreads: ThreadInfo[] = data.map(thread => ({
        id: thread.id,
        type: 'READER' as const, // Backend doesn't send type, default to READER
        status: (thread.status === 'RUNNING' || thread.status === 'COMPLETED' || thread.status === 'ERROR') 
          ? thread.status 
          : 'RUNNING' as const,
        startTime: new Date().toLocaleTimeString(),
      }));
      setThreads(formattedThreads);
    } catch (error) {
      console.error('Failed to fetch threads:', error);
    }
  }, []);

  // Fetch files
  const fetchFiles = useCallback(async () => {
    try {
      const data = await cloudApi.getFiles();
      setFiles(data);
    } catch (error) {
      console.error('Failed to fetch files:', error);
    }
  }, []);

  // Upload file
  const uploadFile = useCallback(async (file: File) => {
    try {
      await cloudApi.uploadFile(file);
      await fetchStats();
      await fetchLogs();
      toast({
        title: 'File uploaded',
        description: `${file.name} uploaded successfully`,
      });
    } catch (error) {
      toast({
        title: 'Upload failed',
        description: 'Failed to upload file to backend',
        variant: 'destructive',
      });
      throw error;
    }
  }, [fetchStats, fetchLogs, toast]);

  // Download file
  const downloadFile = useCallback((filename: string) => {
    // Create a blob with current cloud data
    const blob = new Blob([cloudData], { type: 'text/plain' });
    const url = window.URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = filename;
    document.body.appendChild(a);
    a.click();
    window.URL.revokeObjectURL(url);
    document.body.removeChild(a);
  }, [cloudData]);

  // Spawn thread
  const spawnThread = useCallback(async (type: 'READER' | 'WRITER' | 'DELETER') => {
    try {
      const result = await cloudApi.spawnThread(type);
      
      // Add optimistic update
      const newThread: ThreadInfo = {
        id: result.threadId,
        type,
        status: 'RUNNING',
        startTime: new Date().toLocaleTimeString(),
        progress: 0,
      };
      setThreads(prev => [...prev, newThread]);
      
      // Fetch updated data
      setTimeout(() => {
        fetchThreads();
        fetchStats();
        fetchLogs();
      }, 500);
    } catch (error) {
      toast({
        title: 'Thread spawn failed',
        description: 'Failed to spawn thread on backend',
        variant: 'destructive',
      });
    }
  }, [fetchThreads, fetchStats, fetchLogs, toast]);

  // Run stress test
  const runStressTest = useCallback(async (numThreads: number) => {
    try {
      await cloudApi.runStressTest(numThreads);
      toast({
        title: 'Stress test started',
        description: `Running stress test with ${numThreads} threads`,
      });
      
      // Fetch updates periodically during stress test
      const interval = setInterval(() => {
        fetchThreads();
        fetchStats();
        fetchLogs();
      }, 1000);
      
      // Clear interval after 30 seconds
      setTimeout(() => clearInterval(interval), 30000);
    } catch (error) {
      toast({
        title: 'Stress test failed',
        description: 'Failed to start stress test',
        variant: 'destructive',
      });
    }
  }, [fetchThreads, fetchStats, fetchLogs, toast]);

  // Clear threads (just refresh the list)
  const clearThreads = useCallback(() => {
    setThreads([]);
    setShouldPollThreads(false);
    toast({
      title: 'Threads cleared',
      description: 'Thread list has been cleared',
    });
    // Resume polling after 5 seconds
    setTimeout(() => setShouldPollThreads(true), 5000);
  }, [toast]);

  // Initial connection check and data fetch
  useEffect(() => {
    checkConnection();
    fetchStats();
    fetchLogs();
    fetchThreads();
    fetchFiles();

    // Poll for updates every 3 seconds
    const interval = setInterval(() => {
      fetchStats();
      fetchLogs();
      if (shouldPollThreads) {
        fetchThreads();
      }
      fetchFiles();
    }, 3000);

    return () => clearInterval(interval);
  }, [checkConnection, fetchStats, fetchLogs, fetchThreads, fetchFiles]);

  return {
    cloudData: stats ? `Cloud storage data (${stats.cloudDataSize} bytes)` : 'Connecting...',
    logs,
    threads,
    files,
    stats,
    isConnected,
    uploadFile,
    downloadFile,
    spawnThread,
    runStressTest,
    clearThreads,
    cloudSize: stats?.cloudDataSize || 0,
  };
};
