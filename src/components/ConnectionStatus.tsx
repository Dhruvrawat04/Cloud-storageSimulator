import { Badge } from "@/components/ui/badge";
import { Wifi, WifiOff } from "lucide-react";

interface ConnectionStatusProps {
  isConnected: boolean;
}

export const ConnectionStatus = ({ isConnected }: ConnectionStatusProps) => {
  return (
    <Badge 
      variant={isConnected ? "default" : "destructive"}
      className={`animate-fade-in px-4 py-2 text-sm font-semibold shadow-lg ${
        isConnected ? 'animate-pulse-glow' : ''
      }`}
    >
      {isConnected ? (
        <>
          <Wifi className="h-4 w-4 mr-2 animate-pulse" />
          Backend Connected
        </>
      ) : (
        <>
          <WifiOff className="h-4 w-4 mr-2" />
          Backend Offline
        </>
      )}
    </Badge>
  );
};
