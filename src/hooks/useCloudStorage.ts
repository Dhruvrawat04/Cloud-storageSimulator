import { useState, useCallback, useRef } from "react";

export interface CloudLog {
  id: string;
  timestamp: string;
  threadId: number;
  threadType: string;
  action: string;
  status: string;
  details?: string;
}

export interface CloudFile {
  name: string;
  size: number;
  content: string;
  uploadTime: string;
}

export interface ThreadInfo {
  id: number;
  type: 'READER' | 'WRITER' | 'DELETER';
  status: 'RUNNING' | 'COMPLETED' | 'ERROR';
  startTime: string;
  progress?: number;
}

const useCloudStorage = () => {
  const [cloudData, setCloudData] = useState<string>("InitialFile - Cloud Storage Simulator Started");
  const [logs, setLogs] = useState<CloudLog[]>([]);
  const [threads, setThreads] = useState<ThreadInfo[]>([]);
  const [files, setFiles] = useState<CloudFile[]>([]);
  const threadIdCounter = useRef(1);

  const addLog = useCallback((threadId: number, threadType: string, action: string, status: string, details?: string) => {
    const newLog: CloudLog = {
      id: `${Date.now()}-${Math.random()}`,
      timestamp: new Date().toLocaleTimeString(),
      threadId,
      threadType,
      action,
      status,
      details
    };
    
    setLogs(prev => [newLog, ...prev].slice(0, 50)); // Keep last 50 logs
  }, []);

  const uploadFile = useCallback(async (file: File) => {
    return new Promise<void>((resolve) => {
      const reader = new FileReader();
      reader.onload = (e) => {
        const content = e.target?.result as string;
        const newFile: CloudFile = {
          name: file.name,
          size: content.length,
          content,
          uploadTime: new Date().toISOString()
        };
        
        setCloudData(content);
        setFiles(prev => [newFile, ...prev].slice(0, 10)); // Keep last 10 files
        addLog(0, "SYSTEM", "UPLOAD", "SUCCESS", `"${file.name}" (${content.length} bytes)`);
        resolve();
      };
      reader.readAsText(file);
    });
  }, [addLog]);

  const downloadFile = useCallback((filename: string) => {
    const blob = new Blob([cloudData], { type: 'text/plain' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = filename;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
    
    addLog(0, "SYSTEM", "DOWNLOAD", "SUCCESS", `"${filename}" (${cloudData.length} bytes)`);
  }, [cloudData, addLog]);

  const spawnThread = useCallback((type: 'READER' | 'WRITER' | 'DELETER') => {
    const threadId = threadIdCounter.current++;
    const newThread: ThreadInfo = {
      id: threadId,
      type,
      status: 'RUNNING',
      startTime: new Date().toLocaleTimeString(),
      progress: 0
    };
    
    setThreads(prev => [...prev, newThread]);
    addLog(threadId, type, type, "STARTED");
    
    // Simulate thread work
    const simulateWork = async () => {
      for (let i = 0; i <= 100; i += 10) {
        await new Promise(resolve => setTimeout(resolve, 200));
        setThreads(prev => prev.map(t => 
          t.id === threadId ? { ...t, progress: i } : t
        ));
      }
      
      let finalStatus = "SUCCESS";
      let details = "";
      
      if (type === 'READER') {
        details = `Read ${cloudData.length} bytes`;
      } else if (type === 'WRITER') {
        const sampleContent = `Sample data written by thread ${threadId} at ${new Date().toISOString()}`;
        setCloudData(prev => prev + "\n" + sampleContent);
        details = `Wrote ${sampleContent.length} bytes`;
      } else if (type === 'DELETER') {
        setCloudData("Cloud data cleared by deleter thread");
        details = "Cloud data cleared";
      }
      
      setThreads(prev => prev.map(t => 
        t.id === threadId ? { ...t, status: 'COMPLETED' as const } : t
      ));
      addLog(threadId, type, type, finalStatus, details);
    };
    
    simulateWork();
  }, [cloudData, addLog]);

  const runStressTest = useCallback((numThreads: number = 10) => {
    addLog(0, "SYSTEM", "STRESS_TEST", "STARTED", `${numThreads} threads`);
    
    for (let i = 0; i < numThreads; i++) {
      const type = Math.random() < 0.6 ? 'READER' : Math.random() < 0.8 ? 'WRITER' : 'DELETER';
      setTimeout(() => spawnThread(type), i * 100);
    }
  }, [spawnThread, addLog]);

  const clearThreads = useCallback(() => {
    setThreads([]);
    addLog(0, "SYSTEM", "CLEAR", "SUCCESS", "All threads cleared");
  }, [addLog]);

  return {
    cloudData,
    logs,
    threads,
    files,
    uploadFile,
    downloadFile,
    spawnThread,
    runStressTest,
    clearThreads,
    cloudSize: cloudData.length
  };
};

export { useCloudStorage };