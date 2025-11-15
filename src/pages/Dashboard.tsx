import { CloudHeader } from "@/components/CloudHeader";
import { CloudStats } from "@/components/CloudStats";
import { ConnectionStatus } from "@/components/ConnectionStatus";
import { useBackendCloud } from "@/hooks/useBackendCloud";
import { Alert, AlertDescription, AlertTitle } from "@/components/ui/alert";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { Progress } from "@/components/ui/progress";
import { AlertCircle, Activity, TrendingUp, Database, Cpu } from "lucide-react";

const Dashboard = () => {
  const { logs, threads, files, stats, isConnected, cloudSize } = useBackendCloud();
  const activeThreads = threads.filter(t => t.status === 'RUNNING').length;
  const completedThreads = threads.filter(t => t.status === 'COMPLETED').length;
  const totalOperations = (stats?.completedReads || 0) + (stats?.completedWrites || 0) + (stats?.completedDeletes || 0);

  return (
    <div className="min-h-screen bg-gradient-to-br from-background via-background to-muted/20">
      <CloudHeader />
      
      <main className="container mx-auto px-6 py-8">
        <div className="mb-6 flex flex-col sm:flex-row justify-between items-start sm:items-center gap-4 animate-fade-in-up">
          <div>
            <h1 className="text-3xl md:text-4xl font-bold bg-gradient-to-r from-primary via-purple-500 to-pink-500 bg-clip-text text-transparent">
              Cloud Storage Dashboard
            </h1>
            <p className="text-muted-foreground mt-2">Monitor your cloud storage operations in real-time</p>
          </div>
          <ConnectionStatus isConnected={isConnected} />
        </div>

        {!isConnected && (
          <Alert variant="destructive" className="mb-6 animate-scale-in glass-effect border-2">
            <AlertCircle className="h-5 w-5" />
            <AlertTitle className="font-semibold">Backend Connection Required</AlertTitle>
            <AlertDescription className="mt-2">
              <p className="mb-2">Your C++ backend is running on your local PC (WSL). To connect it to this Replit frontend:</p>
              <ol className="list-decimal list-inside space-y-1 text-sm">
                <li>Keep your backend running: <code className="bg-muted px-2 py-1 rounded font-mono text-xs">cd backend && ./build-wsl.sh</code></li>
                <li>Use a tunneling service like ngrok: <code className="bg-muted px-2 py-1 rounded font-mono text-xs">ngrok http 3001</code></li>
                <li>Update the Vite proxy configuration with the ngrok URL</li>
              </ol>
            </AlertDescription>
          </Alert>
        )}

        <div className="animate-fade-in-up" style={{ animationDelay: '0.1s' }}>
          <CloudStats 
            cloudSize={cloudSize}
            activeThreads={activeThreads}
            totalOperations={totalOperations}
            filesCount={files.length}
          />
        </div>

        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6 mt-8 animate-fade-in-up" style={{ animationDelay: '0.2s' }}>
          <Card className="border-0 shadow-lg bg-gradient-to-br from-blue-500/5 to-blue-600/5">
            <CardHeader>
              <CardTitle className="flex items-center gap-2 text-blue-600">
                <Activity className="w-5 h-5" />
                Thread Activity
              </CardTitle>
              <CardDescription>Real-time thread execution status</CardDescription>
            </CardHeader>
            <CardContent className="space-y-4">
              <div className="flex items-center justify-between">
                <span className="text-sm text-muted-foreground">Active</span>
                <Badge variant="default" className="bg-green-500">{activeThreads}</Badge>
              </div>
              <div className="flex items-center justify-between">
                <span className="text-sm text-muted-foreground">Completed</span>
                <Badge variant="secondary">{completedThreads}</Badge>
              </div>
              <div className="flex items-center justify-between">
                <span className="text-sm text-muted-foreground">Total</span>
                <Badge variant="outline">{threads.length}</Badge>
              </div>
              {threads.length > 0 && (
                <Progress value={(completedThreads / threads.length) * 100} className="mt-2" />
              )}
            </CardContent>
          </Card>

          <Card className="border-0 shadow-lg bg-gradient-to-br from-green-500/5 to-green-600/5">
            <CardHeader>
              <CardTitle className="flex items-center gap-2 text-green-600">
                <TrendingUp className="w-5 h-5" />
                Operations Summary
              </CardTitle>
              <CardDescription>Completed operations breakdown</CardDescription>
            </CardHeader>
            <CardContent className="space-y-4">
              <div className="flex items-center justify-between">
                <span className="text-sm text-muted-foreground">Reads</span>
                <Badge className="bg-blue-500/10 text-blue-600">{stats?.completedReads || 0}</Badge>
              </div>
              <div className="flex items-center justify-between">
                <span className="text-sm text-muted-foreground">Writes</span>
                <Badge className="bg-green-500/10 text-green-600">{stats?.completedWrites || 0}</Badge>
              </div>
              <div className="flex items-center justify-between">
                <span className="text-sm text-muted-foreground">Deletes</span>
                <Badge className="bg-red-500/10 text-red-600">{stats?.completedDeletes || 0}</Badge>
              </div>
              <div className="pt-2 border-t">
                <div className="flex items-center justify-between font-semibold">
                  <span className="text-sm">Total Operations</span>
                  <span className="text-lg">{totalOperations}</span>
                </div>
              </div>
            </CardContent>
          </Card>

          <Card className="border-0 shadow-lg bg-gradient-to-br from-purple-500/5 to-purple-600/5">
            <CardHeader>
              <CardTitle className="flex items-center gap-2 text-purple-600">
                <Database className="w-5 h-5" />
                Storage Information
              </CardTitle>
              <CardDescription>Current storage metrics</CardDescription>
            </CardHeader>
            <CardContent className="space-y-4">
              <div className="flex items-center justify-between">
                <span className="text-sm text-muted-foreground">Total Files</span>
                <span className="text-xl font-bold">{stats?.totalFiles || 0}</span>
              </div>
              <div className="flex items-center justify-between">
                <span className="text-sm text-muted-foreground">Total Size</span>
                <span className="text-xl font-bold">{stats?.totalSize || '0 B'}</span>
              </div>
              <div className="flex items-center justify-between">
                <span className="text-sm text-muted-foreground">Cloud Data</span>
                <span className="text-xl font-bold">{cloudSize.toLocaleString()} B</span>
              </div>
            </CardContent>
          </Card>
        </div>

        {logs.length > 0 && (
          <Card className="mt-8 border-0 shadow-lg animate-fade-in-up" style={{ animationDelay: '0.3s' }}>
            <CardHeader>
              <CardTitle className="flex items-center gap-2">
                <Cpu className="w-5 h-5" />
                Recent Activity
              </CardTitle>
              <CardDescription>Latest system operations and events</CardDescription>
            </CardHeader>
            <CardContent>
              <div className="space-y-2 max-h-64 overflow-y-auto">
                {logs.slice(-10).reverse().map((log) => (
                  <div key={log.id} className="flex items-start gap-3 p-3 rounded-lg bg-muted/50 hover:bg-muted transition-colors">
                    <Badge variant="outline" className="mt-0.5">{log.timestamp}</Badge>
                    <p className="text-sm flex-1">{log.details}</p>
                  </div>
                ))}
              </div>
            </CardContent>
          </Card>
        )}
      </main>
    </div>
  );
};

export default Dashboard;
