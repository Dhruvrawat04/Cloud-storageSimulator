import { CloudHeader } from "@/components/CloudHeader";
import { CloudStats } from "@/components/CloudStats";
import { FileOperations } from "@/components/FileOperations";
import { ThreadManager } from "@/components/ThreadManager";
import { LogViewer } from "@/components/LogViewer";
import { PerformanceMetrics } from "@/components/PerformanceMetrics";
import { ConnectionStatus } from "@/components/ConnectionStatus";
import { useBackendCloud } from "@/hooks/useBackendCloud";
import { Toaster } from "@/components/ui/toaster";
import { Alert, AlertDescription } from "@/components/ui/alert";
import { AlertCircle } from "lucide-react";

const Index = () => {
  const {
    cloudData,
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
    cloudSize
  } = useBackendCloud();

  const activeThreads = threads.filter(t => t.status === 'RUNNING').length;
  const totalOperations = (stats?.completedReads || 0) + (stats?.completedWrites || 0) + (stats?.completedDeletes || 0);

  return (
    <div className="min-h-screen bg-background">
      <CloudHeader />
      
      <main className="container mx-auto px-6 py-8">
        {/* Connection Status Banner */}
        <div className="mb-4 flex justify-between items-center">
          <h1 className="text-2xl font-bold">Advanced Cloud Storage</h1>
          <ConnectionStatus isConnected={isConnected} />
        </div>

        {!isConnected && (
          <Alert variant="destructive" className="mb-6">
            <AlertCircle className="h-4 w-4" />
            <AlertDescription>
              Backend server is not running. Start the backend with: <code className="bg-muted px-2 py-1 rounded">cd backend && ./build-wsl.sh</code>
            </AlertDescription>
          </Alert>
        )}
        <CloudStats 
          cloudSize={cloudSize}
          activeThreads={activeThreads}
          totalOperations={totalOperations}
          filesCount={files.length}
        />
        
        <FileOperations 
          onUpload={uploadFile}
          onDownload={downloadFile}
          cloudData={cloudData}
        />
        
        <ThreadManager 
          threads={threads}
          onSpawnThread={spawnThread}
          onRunStressTest={runStressTest}
          onClearThreads={clearThreads}
        />
        
        <PerformanceMetrics stats={stats} />
        
        <LogViewer logs={logs} />
      </main>
      
      <Toaster />
    </div>
  );
};

export default Index;
