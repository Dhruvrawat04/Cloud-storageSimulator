import { Cloud, Database, Activity } from "lucide-react";

export const CloudHeader = () => {
  return (
    <header className="bg-gradient-hero text-primary-foreground shadow-cloud">
      <div className="container mx-auto px-6 py-8">
        <div className="flex items-center gap-4 mb-4">
          <div className="p-3 bg-white/20 rounded-xl backdrop-blur-sm">
            <Cloud size={32} className="text-white" />
          </div>
          <div>
            <h1 className="text-3xl font-bold">Advanced Cloud Storage System</h1>
            <p className="text-white/90 text-lg">Pthread Threading • Microsecond Timing • Real File Operations</p>
          </div>
        </div>
        
        <div className="grid grid-cols-1 md:grid-cols-3 gap-4 mt-6">
          <div className="bg-white/10 backdrop-blur-sm rounded-lg p-4 border border-white/20">
            <div className="flex items-center gap-2">
              <Database size={20} />
              <span className="font-medium">Cloud Storage</span>
            </div>
            <p className="text-sm text-white/80 mt-1">Real file I/O with downloads, uploads & backups</p>
          </div>
          
          <div className="bg-white/10 backdrop-blur-sm rounded-lg p-4 border border-white/20">
            <div className="flex items-center gap-2">
              <Activity size={20} />
              <span className="font-medium">Multi-Threading</span>
            </div>
            <p className="text-sm text-white/80 mt-1">Real pthread threads with reader-writer locks</p>
          </div>
          
          <div className="bg-white/10 backdrop-blur-sm rounded-lg p-4 border border-white/20">
            <div className="flex items-center gap-2">
              <Cloud size={20} />
              <span className="font-medium">Real-time Ops</span>
            </div>
            <p className="text-sm text-white/80 mt-1">Microsecond-precision performance tracking</p>
          </div>
        </div>
      </div>
    </header>
  );
};