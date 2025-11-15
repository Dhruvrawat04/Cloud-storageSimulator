import { CloudHeader } from "@/components/CloudHeader";
import { ThreadManager } from "@/components/ThreadManager";
import { LogViewer } from "@/components/LogViewer";
import { ConnectionStatus } from "@/components/ConnectionStatus";
import { useBackendCloud } from "@/hooks/useBackendCloud";

const Monitoring = () => {
  const { logs, threads, isConnected, spawnThread, runStressTest, clearThreads } = useBackendCloud();

  return (
    <div className="min-h-screen bg-gradient-to-br from-background via-background to-muted/20">
      <CloudHeader />
      
      <main className="container mx-auto px-6 py-8">
        <div className="mb-6 flex justify-between items-center animate-fade-in-up">
          <h1 className="text-3xl font-bold gradient-text">Thread Monitoring</h1>
          <ConnectionStatus isConnected={isConnected} />
        </div>

        <div className="animate-fade-in-up" style={{ animationDelay: '0.1s' }}>
          <ThreadManager 
            threads={threads}
            onSpawnThread={spawnThread}
            onRunStressTest={runStressTest}
            onClearThreads={clearThreads}
          />
        </div>
        
        <div className="animate-fade-in-up" style={{ animationDelay: '0.2s' }}>
          <LogViewer logs={logs} />
        </div>
      </main>
    </div>
  );
};

export default Monitoring;
