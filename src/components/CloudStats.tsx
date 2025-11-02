import { Card } from "@/components/ui/card";
import { Activity, Database, Users, Zap } from "lucide-react";

interface CloudStatsProps {
  cloudSize: number;
  activeThreads: number;
  totalLogs: number;
  filesCount: number;
}

export const CloudStats = ({ cloudSize, activeThreads, totalLogs, filesCount }: CloudStatsProps) => {
  return (
    <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4 mb-6">
      <Card className="p-4 bg-gradient-card shadow-card border-0">
        <div className="flex items-center gap-3">
          <div className="p-2 bg-primary/10 rounded-lg">
            <Database className="h-5 w-5 text-primary" />
          </div>
          <div>
            <p className="text-sm text-muted-foreground">Cloud Size</p>
            <p className="text-2xl font-bold text-foreground">{cloudSize.toLocaleString()}</p>
            <p className="text-xs text-muted-foreground">bytes</p>
          </div>
        </div>
      </Card>

      <Card className="p-4 bg-gradient-card shadow-card border-0">
        <div className="flex items-center gap-3">
          <div className="p-2 bg-accent/10 rounded-lg">
            <Users className="h-5 w-5 text-accent" />
          </div>
          <div>
            <p className="text-sm text-muted-foreground">Active Threads</p>
            <p className="text-2xl font-bold text-foreground">{activeThreads}</p>
            <p className="text-xs text-muted-foreground">running</p>
          </div>
        </div>
      </Card>

      <Card className="p-4 bg-gradient-card shadow-card border-0">
        <div className="flex items-center gap-3">
          <div className="p-2 bg-success/10 rounded-lg">
            <Activity className="h-5 w-5 text-success" />
          </div>
          <div>
            <p className="text-sm text-muted-foreground">Total Operations</p>
            <p className="text-2xl font-bold text-foreground">{totalLogs}</p>
            <p className="text-xs text-muted-foreground">logged</p>
          </div>
        </div>
      </Card>

      <Card className="p-4 bg-gradient-card shadow-card border-0">
        <div className="flex items-center gap-3">
          <div className="p-2 bg-warning/10 rounded-lg">
            <Zap className="h-5 w-5 text-warning" />
          </div>
          <div>
            <p className="text-sm text-muted-foreground">Files Stored</p>
            <p className="text-2xl font-bold text-foreground">{filesCount}</p>
            <p className="text-xs text-muted-foreground">in cache</p>
          </div>
        </div>
      </Card>
    </div>
  );
};