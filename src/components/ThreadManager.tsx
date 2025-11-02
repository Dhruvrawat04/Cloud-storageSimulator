import { useState } from "react";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Progress } from "@/components/ui/progress";
import { Badge } from "@/components/ui/badge";
import { Users, Play, Trash2, Zap, Eye, Edit, Trash } from "lucide-react";
import { useToast } from "@/hooks/use-toast";
import type { ThreadInfo } from "@/hooks/useBackendCloud";

interface ThreadManagerProps {
  threads: ThreadInfo[];
  onSpawnThread: (type: 'READER' | 'WRITER' | 'DELETER') => void;
  onRunStressTest: (numThreads: number) => void;
  onClearThreads: () => void;
}

export const ThreadManager = ({ threads, onSpawnThread, onRunStressTest, onClearThreads }: ThreadManagerProps) => {
  const { toast } = useToast();
  const [stressTestCount, setStressTestCount] = useState(10);
  
  const activeThreads = threads.filter(t => t.status === 'RUNNING');
  const completedThreads = threads.filter(t => t.status === 'COMPLETED');

  const handleSpawnThread = (type: 'READER' | 'WRITER' | 'DELETER') => {
    onSpawnThread(type);
    toast({
      title: `${type} thread spawned`,
      description: `New ${type.toLowerCase()} thread started`,
    });
  };

  const handleStressTest = () => {
    const count = Math.max(1, Math.min(100, stressTestCount)); // Limit between 1-100
    onRunStressTest(count);
    toast({
      title: "Stress test started",
      description: `Running stress test with ${count} concurrent threads`,
    });
  };

  const getThreadIcon = (type: string) => {
    switch (type) {
      case 'READER': return <Eye className="h-4 w-4" />;
      case 'WRITER': return <Edit className="h-4 w-4" />;
      case 'DELETER': return <Trash className="h-4 w-4" />;
      default: return <Users className="h-4 w-4" />;
    }
  };

  const getThreadBadgeVariant = (status: string) => {
    switch (status) {
      case 'RUNNING': return 'default';
      case 'COMPLETED': return 'secondary';
      case 'ERROR': return 'destructive';
      default: return 'outline';
    }
  };

  return (
    <div className="grid grid-cols-1 lg:grid-cols-2 gap-6 mb-6">
      <Card className="bg-gradient-card shadow-card border-0">
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <Users className="h-5 w-5 text-primary" />
            Thread Control Panel
          </CardTitle>
        </CardHeader>
        <CardContent className="space-y-4">
          <div className="grid grid-cols-1 gap-3">
            <Button 
              onClick={() => handleSpawnThread('READER')}
              variant="cloud"
              className="w-full justify-start"
            >
              <Eye className="h-4 w-4 mr-2" />
              Spawn Reader Thread
            </Button>
            
            <Button 
              onClick={() => handleSpawnThread('WRITER')}
              variant="cloud"
              className="w-full justify-start"
            >
              <Edit className="h-4 w-4 mr-2" />
              Spawn Writer Thread  
            </Button>
            
            <Button 
              onClick={() => handleSpawnThread('DELETER')}
              variant="cloud"
              className="w-full justify-start"
            >
              <Trash className="h-4 w-4 mr-2" />
              Spawn Deleter Thread
            </Button>
          </div>
          
          <div className="pt-4 border-t space-y-3">
            <div className="space-y-2">
              <Label htmlFor="stress-test-count">Stress Test Threads</Label>
              <Input
                id="stress-test-count"
                type="number"
                min="1"
                max="100"
                value={stressTestCount}
                onChange={(e) => setStressTestCount(parseInt(e.target.value) || 10)}
                className="w-full"
              />
            </div>
            <Button 
              onClick={handleStressTest}
              variant="warning"
              className="w-full"
            >
              <Zap className="h-4 w-4 mr-2" />
              Run Stress Test ({stressTestCount} threads)
            </Button>
            
            <Button 
              onClick={onClearThreads}
              variant="outline"
              className="w-full"
              disabled={threads.length === 0}
            >
              <Trash2 className="h-4 w-4 mr-2" />
              Clear All Threads
            </Button>
          </div>
          
          <div className="pt-4 border-t">
            <div className="text-sm text-muted-foreground space-y-1">
              <p>Active: {activeThreads.length} | Completed: {completedThreads.length}</p>
              <p>Total threads: {threads.length}</p>
            </div>
          </div>
        </CardContent>
      </Card>

      <Card className="bg-gradient-card shadow-card border-0">
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <Play className="h-5 w-5 text-accent" />
            Active Threads
          </CardTitle>
        </CardHeader>
        <CardContent>
          <div className="space-y-3 max-h-80 overflow-y-auto">
            {threads.length === 0 ? (
              <p className="text-muted-foreground text-center py-8">
                No threads running. Spawn some threads to see them here.
              </p>
            ) : (
              threads.map((thread) => (
                <div key={thread.id} className="border rounded-lg p-3 bg-background/50">
                  <div className="flex items-center justify-between mb-2">
                    <div className="flex items-center gap-2">
                      {getThreadIcon(thread.type)}
                      <span className="font-medium">Thread {thread.id}</span>
                      <Badge variant={getThreadBadgeVariant(thread.status)}>
                        {thread.type}
                      </Badge>
                    </div>
                    <Badge variant={getThreadBadgeVariant(thread.status)}>
                      {thread.status}
                    </Badge>
                  </div>
                  
                  {thread.status === 'RUNNING' && thread.progress !== undefined && (
                    <div className="space-y-1">
                      <div className="flex justify-between text-xs text-muted-foreground">
                        <span>Progress</span>
                        <span>{thread.progress}%</span>
                      </div>
                      <Progress value={thread.progress} className="h-2" />
                    </div>
                  )}
                  
                  <p className="text-xs text-muted-foreground mt-2">
                    Started: {thread.startTime}
                  </p>
                </div>
              ))
            )}
          </div>
        </CardContent>
      </Card>
    </div>
  );
};