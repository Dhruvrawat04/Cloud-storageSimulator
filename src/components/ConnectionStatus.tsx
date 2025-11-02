import { Badge } from "@/components/ui/badge";
import { Wifi, WifiOff } from "lucide-react";

interface ConnectionStatusProps {
  isConnected: boolean;
}

export const ConnectionStatus = ({ isConnected }: ConnectionStatusProps) => {
  return (
    <Badge 
      variant={isConnected ? "default" : "destructive"}
      className="animate-fade-in"
    >
      {isConnected ? (
        <>
          <Wifi className="h-3 w-3 mr-1" />
          Backend Connected
        </>
      ) : (
        <>
          <WifiOff className="h-3 w-3 mr-1" />
          Backend Offline
        </>
      )}
    </Badge>
  );
};