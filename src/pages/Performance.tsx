import { CloudHeader } from "@/components/CloudHeader";
import { PerformanceMetrics } from "@/components/PerformanceMetrics";
import { ConnectionStatus } from "@/components/ConnectionStatus";
import { useBackendCloud } from "@/hooks/useBackendCloud";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { Progress } from "@/components/ui/progress";
import { Gauge, Zap, TrendingUp, Activity } from "lucide-react";

const Performance = () => {
  const { stats, threads, isConnected } = useBackendCloud();
  
  const activeOps = (stats?.activeReaders || 0) + (stats?.activeWriters || 0) + (stats?.activeDeleters || 0);
  const completedOps = (stats?.completedReads || 0) + (stats?.completedWrites || 0) + (stats?.completedDeletes || 0);
  const totalOps = activeOps + completedOps;
  const efficiency = totalOps > 0 ? ((completedOps / totalOps) * 100).toFixed(1) : 0;

  return (
    <div className="min-h-screen bg-gradient-to-br from-background via-background to-muted/20">
      <CloudHeader />
      
      <main className="container mx-auto px-6 py-8">
        <div className="mb-6 flex flex-col sm:flex-row justify-between items-start sm:items-center gap-4 animate-fade-in-up">
          <div>
            <h1 className="text-3xl md:text-4xl font-bold bg-gradient-to-r from-primary via-blue-500 to-cyan-500 bg-clip-text text-transparent">
              Performance Analytics
            </h1>
            <p className="text-muted-foreground mt-2">Real-time performance metrics and system analytics</p>
          </div>
          <ConnectionStatus isConnected={isConnected} />
        </div>

        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6 mb-8 animate-fade-in-up" style={{ animationDelay: '0.1s' }}>
          <Card className="border-0 shadow-lg bg-gradient-to-br from-cyan-500/5 to-cyan-600/5">
            <CardHeader className="pb-3">
              <CardTitle className="text-sm font-medium text-muted-foreground">System Efficiency</CardTitle>
            </CardHeader>
            <CardContent>
              <div className="flex items-center gap-3">
                <Gauge className="w-8 h-8 text-cyan-500" />
                <div>
                  <p className="text-3xl font-bold">{efficiency}%</p>
                  <p className="text-xs text-muted-foreground">Completion Rate</p>
                </div>
              </div>
            </CardContent>
          </Card>

          <Card className="border-0 shadow-lg bg-gradient-to-br from-green-500/5 to-green-600/5">
            <CardHeader className="pb-3">
              <CardTitle className="text-sm font-medium text-muted-foreground">Active Operations</CardTitle>
            </CardHeader>
            <CardContent>
              <div className="flex items-center gap-3">
                <Activity className="w-8 h-8 text-green-500" />
                <div>
                  <p className="text-3xl font-bold">{activeOps}</p>
                  <p className="text-xs text-muted-foreground">In Progress</p>
                </div>
              </div>
            </CardContent>
          </Card>

          <Card className="border-0 shadow-lg bg-gradient-to-br from-blue-500/5 to-blue-600/5">
            <CardHeader className="pb-3">
              <CardTitle className="text-sm font-medium text-muted-foreground">Completed Ops</CardTitle>
            </CardHeader>
            <CardContent>
              <div className="flex items-center gap-3">
                <TrendingUp className="w-8 h-8 text-blue-500" />
                <div>
                  <p className="text-3xl font-bold">{completedOps}</p>
                  <p className="text-xs text-muted-foreground">Total Finished</p>
                </div>
              </div>
            </CardContent>
          </Card>

          <Card className="border-0 shadow-lg bg-gradient-to-br from-orange-500/5 to-orange-600/5">
            <CardHeader className="pb-3">
              <CardTitle className="text-sm font-medium text-muted-foreground">Active Threads</CardTitle>
            </CardHeader>
            <CardContent>
              <div className="flex items-center gap-3">
                <Zap className="w-8 h-8 text-orange-500" />
                <div>
                  <p className="text-3xl font-bold">{stats?.activeThreads || 0}</p>
                  <p className="text-xs text-muted-foreground">Running Now</p>
                </div>
              </div>
            </CardContent>
          </Card>
        </div>

        <div className="animate-fade-in-up" style={{ animationDelay: '0.2s' }}>
          <PerformanceMetrics stats={stats} />
        </div>

        {stats && (
          <div className="grid grid-cols-1 md:grid-cols-3 gap-6 mt-8 animate-fade-in-up" style={{ animationDelay: '0.3s' }}>
            <Card className="border-0 shadow-lg">
              <CardHeader>
                <CardTitle className="text-lg">Read Operations</CardTitle>
                <CardDescription>Performance breakdown</CardDescription>
              </CardHeader>
              <CardContent className="space-y-3">
                <div className="flex justify-between items-center">
                  <span className="text-sm text-muted-foreground">Active</span>
                  <Badge className="bg-blue-500">{stats.activeReaders}</Badge>
                </div>
                <div className="flex justify-between items-center">
                  <span className="text-sm text-muted-foreground">Completed</span>
                  <Badge variant="secondary">{stats.completedReads}</Badge>
                </div>
                {(stats.activeReaders + stats.completedReads) > 0 && (
                  <Progress 
                    value={(stats.completedReads / (stats.activeReaders + stats.completedReads)) * 100} 
                    className="mt-2" 
                  />
                )}
              </CardContent>
            </Card>

            <Card className="border-0 shadow-lg">
              <CardHeader>
                <CardTitle className="text-lg">Write Operations</CardTitle>
                <CardDescription>Performance breakdown</CardDescription>
              </CardHeader>
              <CardContent className="space-y-3">
                <div className="flex justify-between items-center">
                  <span className="text-sm text-muted-foreground">Active</span>
                  <Badge className="bg-green-500">{stats.activeWriters}</Badge>
                </div>
                <div className="flex justify-between items-center">
                  <span className="text-sm text-muted-foreground">Completed</span>
                  <Badge variant="secondary">{stats.completedWrites}</Badge>
                </div>
                {(stats.activeWriters + stats.completedWrites) > 0 && (
                  <Progress 
                    value={(stats.completedWrites / (stats.activeWriters + stats.completedWrites)) * 100} 
                    className="mt-2" 
                  />
                )}
              </CardContent>
            </Card>

            <Card className="border-0 shadow-lg">
              <CardHeader>
                <CardTitle className="text-lg">Delete Operations</CardTitle>
                <CardDescription>Performance breakdown</CardDescription>
              </CardHeader>
              <CardContent className="space-y-3">
                <div className="flex justify-between items-center">
                  <span className="text-sm text-muted-foreground">Active</span>
                  <Badge className="bg-red-500">{stats.activeDeleters}</Badge>
                </div>
                <div className="flex justify-between items-center">
                  <span className="text-sm text-muted-foreground">Completed</span>
                  <Badge variant="secondary">{stats.completedDeletes}</Badge>
                </div>
                {(stats.activeDeleters + stats.completedDeletes) > 0 && (
                  <Progress 
                    value={(stats.completedDeletes / (stats.activeDeleters + stats.completedDeletes)) * 100} 
                    className="mt-2" 
                  />
                )}
              </CardContent>
            </Card>
          </div>
        )}
      </main>
    </div>
  );
};

export default Performance;
