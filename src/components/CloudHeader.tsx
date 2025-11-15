import { Cloud, Database, Activity } from "lucide-react";

export const CloudHeader = () => {
  return (
    <header className="bg-gradient-hero text-primary-foreground shadow-glow relative overflow-hidden">
      {/* Animated background elements */}
      <div className="absolute inset-0 overflow-hidden opacity-20">
        <div className="absolute -top-10 -right-10 w-40 h-40 bg-white rounded-full blur-3xl animate-float" style={{ animationDelay: '0s' }}></div>
        <div className="absolute top-20 -left-10 w-32 h-32 bg-white rounded-full blur-3xl animate-float" style={{ animationDelay: '1s' }}></div>
        <div className="absolute bottom-10 right-1/3 w-36 h-36 bg-white rounded-full blur-3xl animate-float" style={{ animationDelay: '2s' }}></div>
      </div>
      
      <div className="container mx-auto px-6 py-10 relative z-10">
        <div className="flex items-center gap-5 mb-6 animate-fade-in-up">
          <div className="p-4 bg-white/20 rounded-2xl backdrop-blur-sm shadow-lg hover:shadow-glow transition-all duration-300 hover:scale-110 animate-pulse-glow">
            <Cloud size={40} className="text-white" />
          </div>
          <div>
            <h1 className="text-4xl font-bold mb-2 drop-shadow-lg">Advanced Cloud Storage System</h1>
            <p className="text-white/95 text-lg font-medium">Pthread Threading • Microsecond Timing • Real File Operations</p>
          </div>
        </div>
        
        <div className="grid grid-cols-1 md:grid-cols-3 gap-5 mt-6 animate-fade-in-up" style={{ animationDelay: '0.2s' }}>
          <div className="group bg-white/15 backdrop-blur-md rounded-xl p-5 border border-white/30 hover:bg-white/25 hover:border-white/40 transition-all duration-300 hover:shadow-glow hover:scale-105 cursor-pointer">
            <div className="flex items-center gap-3 mb-2">
              <div className="p-2 bg-white/20 rounded-lg group-hover:scale-110 transition-transform duration-300">
                <Database size={22} className="text-white" />
              </div>
              <span className="font-semibold text-lg">Cloud Storage</span>
            </div>
            <p className="text-sm text-white/90 leading-relaxed">Real file I/O with downloads, uploads & backups</p>
          </div>
          
          <div className="group bg-white/15 backdrop-blur-md rounded-xl p-5 border border-white/30 hover:bg-white/25 hover:border-white/40 transition-all duration-300 hover:shadow-glow hover:scale-105 cursor-pointer">
            <div className="flex items-center gap-3 mb-2">
              <div className="p-2 bg-white/20 rounded-lg group-hover:scale-110 transition-transform duration-300">
                <Activity size={22} className="text-white" />
              </div>
              <span className="font-semibold text-lg">Multi-Threading</span>
            </div>
            <p className="text-sm text-white/90 leading-relaxed">Real pthread threads with reader-writer locks</p>
          </div>
          
          <div className="group bg-white/15 backdrop-blur-md rounded-xl p-5 border border-white/30 hover:bg-white/25 hover:border-white/40 transition-all duration-300 hover:shadow-glow hover:scale-105 cursor-pointer">
            <div className="flex items-center gap-3 mb-2">
              <div className="p-2 bg-white/20 rounded-lg group-hover:scale-110 transition-transform duration-300">
                <Cloud size={22} className="text-white" />
              </div>
              <span className="font-semibold text-lg">Real-time Ops</span>
            </div>
            <p className="text-sm text-white/90 leading-relaxed">Microsecond-precision performance tracking</p>
          </div>
        </div>
      </div>
    </header>
  );
};