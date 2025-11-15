import { Link, useLocation } from "react-router-dom";
import { Home, Files, Activity, BarChart3, Cpu } from "lucide-react";
import { cn } from "@/lib/utils";

const Navigation = () => {
  const location = useLocation();

  const links = [
    { to: "/", label: "Dashboard", icon: Home },
    { to: "/files", label: "Files", icon: Files },
    { to: "/monitoring", label: "Monitoring", icon: Activity },
    { to: "/performance", label: "Performance", icon: BarChart3 },
    { to: "/os-simulator", label: "OS Simulator", icon: Cpu },
  ];

  return (
    <nav className="sticky top-0 z-50 border-b border-border/40 bg-background/95 backdrop-blur supports-[backdrop-filter]:bg-background/60 shadow-lg">
      <div className="container mx-auto px-6">
        <div className="flex h-16 items-center justify-between">
          <div className="flex items-center space-x-2">
            <div className="text-2xl font-bold gradient-text">Cloud Storage</div>
          </div>
          <div className="flex items-center space-x-1">
            {links.map(({ to, label, icon: Icon }) => (
              <Link
                key={to}
                to={to}
                className={cn(
                  "flex items-center space-x-2 px-4 py-2 rounded-lg text-sm font-medium transition-all duration-200",
                  location.pathname === to
                    ? "bg-primary text-primary-foreground shadow-md"
                    : "text-muted-foreground hover:bg-muted hover:text-foreground"
                )}
              >
                <Icon className="h-4 w-4" />
                <span>{label}</span>
              </Link>
            ))}
          </div>
        </div>
      </div>
    </nav>
  );
};

export default Navigation;
