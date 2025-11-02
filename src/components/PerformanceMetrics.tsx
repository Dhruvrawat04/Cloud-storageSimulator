import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { Activity, TrendingUp, Clock, Zap } from "lucide-react";
import type { BackendStats } from "@/services/cloudApi";

interface PerformanceMetricsProps {
  stats: BackendStats | null;
}

export const PerformanceMetrics = ({ stats }: PerformanceMetricsProps) => {
  if (!stats) {
    return (
      <Card className="bg-gradient-card shadow-card border-0">
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <Activity className="h-5 w-5 text-primary" />
            Performance Metrics
          </CardTitle>
        </CardHeader>
        <CardContent>
          <p className="text-muted-foreground text-center py-4">
            Connecting to backend...
          </p>
        </CardContent>
      </Card>
    );
  }

  const metrics = [
    {
      label: 'Active Readers',
      value: stats.activeReaders,
      icon: Activity,
      color: 'text-blue-500',
    },
    {
      label: 'Active Writers',
      value: stats.activeWriters,
      icon: TrendingUp,
      color: 'text-green-500',
    },
    {
      label: 'Active Deleters',
      value: stats.activeDeleters,
      icon: Zap,
      color: 'text-red-500',
    },
  ];

  const completed = [
    {
      label: 'Completed Reads',
      value: stats.completedReads,
      color: 'bg-blue-500/10 text-blue-500',
    },
    {
      label: 'Completed Writes',
      value: stats.completedWrites,
      color: 'bg-green-500/10 text-green-500',
    },
    {
      label: 'Completed Deletes',
      value: stats.completedDeletes,
      color: 'bg-red-500/10 text-red-500',
    },
  ];

  return (
    <Card className="bg-gradient-card shadow-card border-0">
      <CardHeader>
        <CardTitle className="flex items-center gap-2">
          <Activity className="h-5 w-5 text-primary" />
          Performance Metrics
          <Badge variant="outline" className="ml-auto">
            <Clock className="h-3 w-3 mr-1" />
            Real-time
          </Badge>
        </CardTitle>
      </CardHeader>
      <CardContent className="space-y-6">
        {/* Active Operations */}
        <div>
          <h3 className="text-sm font-semibold mb-3 text-muted-foreground">Active Operations</h3>
          <div className="grid grid-cols-3 gap-4">
            {metrics.map((metric) => (
              <div key={metric.label} className="text-center">
                <div className={`inline-flex p-3 rounded-lg bg-background/50 mb-2 ${metric.color}`}>
                  <metric.icon className="h-5 w-5" />
                </div>
                <p className="text-2xl font-bold">{metric.value}</p>
                <p className="text-xs text-muted-foreground">{metric.label}</p>
              </div>
            ))}
          </div>
        </div>

        {/* Completed Operations */}
        <div>
          <h3 className="text-sm font-semibold mb-3 text-muted-foreground">Completed Operations</h3>
          <div className="space-y-2">
            {completed.map((item) => (
              <div key={item.label} className="flex items-center justify-between p-3 rounded-lg bg-background/50">
                <span className="text-sm">{item.label}</span>
                <Badge className={item.color}>
                  {item.value}
                </Badge>
              </div>
            ))}
          </div>
        </div>

        {/* Storage Info */}
        <div className="pt-4 border-t">
          <div className="grid grid-cols-2 gap-4 text-sm">
            <div>
              <p className="text-muted-foreground">Total Files</p>
              <p className="text-lg font-semibold">{stats.totalFiles}</p>
            </div>
            <div>
              <p className="text-muted-foreground">Total Size</p>
              <p className="text-lg font-semibold">{stats.totalSize}</p>
            </div>
            <div>
              <p className="text-muted-foreground">Cloud Data</p>
              <p className="text-lg font-semibold">{stats.cloudDataSize} bytes</p>
            </div>
            <div>
              <p className="text-muted-foreground">Active Threads</p>
              <p className="text-lg font-semibold">{stats.activeThreads}</p>
            </div>
          </div>
        </div>
      </CardContent>
    </Card>
  );
};
