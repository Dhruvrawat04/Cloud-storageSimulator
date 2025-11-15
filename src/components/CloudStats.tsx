import { Card } from "@/components/ui/card";
import { Activity, Database, Users, Zap } from "lucide-react";

interface CloudStatsProps {
  cloudSize: number;
  activeThreads: number;
  totalOperations: number;
  filesCount: number;
}

export const CloudStats = ({ cloudSize, activeThreads, totalOperations, filesCount }: CloudStatsProps) => {
  return (
    <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-5 mb-8">
      <Card className="group p-5 bg-gradient-card shadow-card hover:shadow-hover border-0 transition-all duration-300 hover:scale-105 cursor-pointer overflow-hidden relative">
        <div className="absolute top-0 right-0 w-20 h-20 bg-primary/5 rounded-full blur-2xl"></div>
        <div className="relative z-10 flex items-center gap-4">
          <div className="p-3 bg-gradient-primary rounded-xl shadow-lg group-hover:scale-110 transition-transform duration-300">
            <Database className="h-6 w-6 text-primary-foreground" />
          </div>
          <div>
            <p className="text-sm text-muted-foreground font-medium mb-1">Cloud Size</p>
            <p className="text-3xl font-bold text-primary">
              {(cloudSize !== undefined && cloudSize !== null ? cloudSize : 0).toLocaleString()}
            </p>
            <p className="text-xs text-muted-foreground/70 mt-1">bytes stored</p>
          </div>
        </div>
      </Card>

      <Card className="group p-5 bg-gradient-card shadow-card hover:shadow-hover border-0 transition-all duration-300 hover:scale-105 cursor-pointer overflow-hidden relative">
        <div className="absolute top-0 right-0 w-20 h-20 bg-accent/5 rounded-full blur-2xl"></div>
        <div className="relative z-10 flex items-center gap-4">
          <div className="p-2 rounded-xl shadow-md bg-background/80 ring-1 ring-black/5">
            <div className="p-3 bg-gradient-accent rounded-lg shadow group-hover:scale-110 transition-transform duration-300">
              <Users className="h-6 w-6 text-blue-500" />
            </div>
          </div>
          <div>
            <p className="text-sm text-muted-foreground font-medium mb-1">Active Threads</p>
            <p className="text-3xl font-bold text-foreground">
              {(activeThreads !== undefined && activeThreads !== null ? activeThreads : 0).toLocaleString()}
            </p>
            <p className="text-xs text-muted-foreground/70 mt-1">threads running</p>
          </div>
        </div>
      </Card>

      <Card className="group p-5 bg-gradient-card shadow-card hover:shadow-hover border-0 transition-all duration-300 hover:scale-105 cursor-pointer overflow-hidden relative">
        <div className="absolute top-0 right-0 w-20 h-20 bg-success/5 rounded-full blur-2xl"></div>
        <div className="relative z-10 flex items-center gap-4">
          <div className="p-2 rounded-xl shadow-md bg-background/80 ring-1 ring-black/5">
            <div className="p-3 bg-gradient-success rounded-lg shadow group-hover:scale-110 transition-transform duration-300">
              <Activity className="h-6 w-6 text-blue-500" />
            </div>
          </div>
          <div>
            <p className="text-sm text-muted-foreground font-medium mb-1">Total Operations</p>
            <p className="text-3xl font-bold text-foreground">
              {(totalOperations !== undefined && totalOperations !== null ? totalOperations : 0).toLocaleString()}
            </p>
            <p className="text-xs text-muted-foreground/70 mt-1">operations logged</p>
          </div>
        </div>
      </Card>

      <Card className="group p-5 bg-gradient-card shadow-card hover:shadow-hover border-0 transition-all duration-300 hover:scale-105 cursor-pointer overflow-hidden relative">
        <div className="absolute top-0 right-0 w-20 h-20 bg-warning/5 rounded-full blur-2xl"></div>
        <div className="relative z-10 flex items-center gap-4">
          <div className="p-3 bg-warning rounded-xl shadow-lg group-hover:scale-110 transition-transform duration-300">
            <Zap className="h-6 w-6 text-warning-foreground" />
          </div>
          <div>
            <p className="text-sm text-muted-foreground font-medium mb-1">Files Stored</p>
            <p className="text-3xl font-bold text-warning">
              {filesCount !== undefined && filesCount !== null ? filesCount : 0}
            </p>
            <p className="text-xs text-muted-foreground/70 mt-1">files in cache</p>
          </div>
        </div>
      </Card>
    </div>
  );
};