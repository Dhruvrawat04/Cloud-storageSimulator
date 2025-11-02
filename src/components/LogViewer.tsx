import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { ScrollArea } from "@/components/ui/scroll-area";
import { FileText, Clock, User, Activity } from "lucide-react";
import type { CloudLog } from "@/hooks/useCloudStorage";

interface LogViewerProps {
  logs: CloudLog[];
}

export const LogViewer = ({ logs }: LogViewerProps) => {
  const getActionBadgeVariant = (action: string) => {
    switch (action.toUpperCase()) {
      case 'UPLOAD':
      case 'SUCCESS':
        return 'default';
      case 'DOWNLOAD':
        return 'secondary';
      case 'ERROR':
        return 'destructive';
      case 'STARTED':
        return 'outline';
      case 'READ':
      case 'READER':
        return 'secondary';
      case 'WRITE':
      case 'WRITER':
        return 'default';
      case 'DELETE':
      case 'DELETER':
        return 'destructive';
      default:
        return 'outline';
    }
  };

  const getThreadTypeIcon = (threadType: string) => {
    switch (threadType.toUpperCase()) {
      case 'READER':
        return <FileText className="h-3 w-3" />;
      case 'WRITER':
        return <Activity className="h-3 w-3" />;
      case 'DELETER':
        return <Activity className="h-3 w-3" />;
      case 'SYSTEM':
        return <User className="h-3 w-3" />;
      default:
        return <Activity className="h-3 w-3" />;
    }
  };

  return (
    <Card className="bg-gradient-card shadow-card border-0">
      <CardHeader>
        <CardTitle className="flex items-center gap-2">
          <FileText className="h-5 w-5 text-primary" />
          System Logs
          <Badge variant="outline" className="ml-auto">
            {logs.length} entries
          </Badge>
        </CardTitle>
      </CardHeader>
      <CardContent>
        <ScrollArea className="h-96">
          <div className="space-y-2">
            {logs.length === 0 ? (
              <div className="text-center py-8 text-muted-foreground">
                <FileText className="h-12 w-12 mx-auto mb-4 opacity-50" />
                <p>No logs yet. Start performing operations to see logs here.</p>
              </div>
            ) : (
              logs.map((log) => (
                <div key={log.id} className="border rounded-lg p-3 bg-background/50 hover:bg-background/70 transition-colors">
                  <div className="flex items-start justify-between gap-2 mb-2">
                    <div className="flex items-center gap-2 flex-1 min-w-0">
                      <div className="flex items-center gap-1 text-xs text-muted-foreground">
                        <Clock className="h-3 w-3" />
                        {log.timestamp}
                      </div>
                      
                      {log.threadId > 0 && (
                        <div className="flex items-center gap-1 text-xs">
                          {getThreadTypeIcon(log.threadType)}
                          <span className="font-mono">#{log.threadId}</span>
                        </div>
                      )}
                    </div>
                    
                    <div className="flex items-center gap-1">
                      <Badge variant={getActionBadgeVariant(log.threadType)} className="text-xs">
                        {log.threadType}
                      </Badge>
                      <Badge variant={getActionBadgeVariant(log.status)} className="text-xs">
                        {log.status}
                      </Badge>
                    </div>
                  </div>
                  
                  <div className="text-sm">
                    <span className="font-medium">{log.action}</span>
                    {log.details && (
                      <span className="text-muted-foreground ml-2">
                        - {log.details}
                      </span>
                    )}
                  </div>
                </div>
              ))
            )}
          </div>
        </ScrollArea>
      </CardContent>
    </Card>
  );
};