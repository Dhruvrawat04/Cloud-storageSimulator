import { useState, useRef } from "react";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Upload, Download, FileText, FolderOpen } from "lucide-react";
import { useToast } from "@/hooks/use-toast";

interface FileOperationsProps {
  onUpload: (file: File) => Promise<void>;
  onDownload: (filename: string) => void;
  cloudData: string;
}

export const FileOperations = ({ onUpload, onDownload, cloudData }: FileOperationsProps) => {
  const [downloadFilename, setDownloadFilename] = useState("cloud_data.txt");
  const [isUploading, setIsUploading] = useState(false);
  const fileInputRef = useRef<HTMLInputElement>(null);
  const { toast } = useToast();

  const handleFileUpload = async (event: React.ChangeEvent<HTMLInputElement>) => {
    const file = event.target.files?.[0];
    if (!file) return;

    setIsUploading(true);
    try {
      await onUpload(file);
      toast({
        title: "Upload successful",
        description: `${file.name} has been uploaded to cloud storage`,
      });
    } catch (error) {
      toast({
        title: "Upload failed",
        description: "There was an error uploading the file",
        variant: "destructive",
      });
    } finally {
      setIsUploading(false);
      if (fileInputRef.current) {
        fileInputRef.current.value = "";
      }
    }
  };

  const handleDownload = () => {
    if (!downloadFilename.trim()) {
      toast({
        title: "Invalid filename",
        description: "Please enter a valid filename",
        variant: "destructive",
      });
      return;
    }
    
    onDownload(downloadFilename);
    toast({
      title: "Download started",
      description: `Downloading cloud data as ${downloadFilename}`,
    });
  };

  return (
    <div className="grid grid-cols-1 lg:grid-cols-2 gap-6 mb-8">
      <Card className="group bg-gradient-card shadow-card hover:shadow-hover border-0 transition-all duration-300 overflow-hidden relative">
        <div className="absolute top-0 right-0 w-32 h-32 bg-primary/5 rounded-full blur-3xl"></div>
        <CardHeader className="relative z-10">
          <CardTitle className="flex items-center gap-3 text-xl">
            <div className="p-2 bg-gradient-primary rounded-lg shadow-md">
              <Upload className="h-5 w-5 text-primary-foreground" />
            </div>
            Upload File
          </CardTitle>
        </CardHeader>
        <CardContent className="space-y-4">
          <div>
            <Label htmlFor="file-upload">Select file to upload to cloud</Label>
            <Input
              id="file-upload"
              type="file"
              ref={fileInputRef}
              onChange={handleFileUpload}
              disabled={isUploading}
              className="mt-2"
            />
          </div>
          <Button 
            onClick={() => fileInputRef.current?.click()} 
            disabled={isUploading}
            variant="hero"
            className="w-full"
          >
            {isUploading ? "Uploading..." : "Choose File to Upload"}
          </Button>
        </CardContent>
      </Card>

      <Card className="group bg-gradient-card shadow-card hover:shadow-hover border-0 transition-all duration-300 overflow-hidden relative">
        <div className="absolute top-0 right-0 w-32 h-32 bg-accent/5 rounded-full blur-3xl"></div>
        <CardHeader className="relative z-10">
          <CardTitle className="flex items-center gap-3 text-xl">
            <div className="p-2 bg-gradient-accent rounded-lg shadow-md">
              <Download className="h-5 w-5 text-accent-foreground" />
            </div>
            Download Cloud Data
          </CardTitle>
        </CardHeader>
        <CardContent className="space-y-4">
          <div>
            <Label htmlFor="download-filename">Filename for download</Label>
            <Input
              id="download-filename"
              value={downloadFilename}
              onChange={(e) => setDownloadFilename(e.target.value)}
              placeholder="Enter filename..."
              className="mt-2"
            />
          </div>
          <Button 
            onClick={handleDownload}
            variant="cloud"
            className="w-full"
          >
            Download Cloud Data
          </Button>
        </CardContent>
      </Card>

      <Card className="group lg:col-span-2 bg-gradient-card shadow-card hover:shadow-hover border-0 transition-all duration-300 overflow-hidden relative">
        <div className="absolute top-0 right-0 w-40 h-40 bg-success/5 rounded-full blur-3xl"></div>
        <CardHeader className="relative z-10">
          <CardTitle className="flex items-center gap-3 text-xl">
            <div className="p-2 bg-gradient-success rounded-lg shadow-md">
              <FileText className="h-5 w-5 text-success-foreground" />
            </div>
            Cloud Data Preview
          </CardTitle>
        </CardHeader>
        <CardContent className="relative z-10">
          <div className="bg-muted/50 backdrop-blur-sm rounded-xl p-5 max-h-48 overflow-y-auto border border-border/50 hover:border-border transition-colors">
            <pre className="text-sm text-muted-foreground whitespace-pre-wrap font-mono">
              {cloudData.length > 1000 
                ? cloudData.substring(0, 1000) + "\n... (truncated, " + cloudData.length + " total bytes)"
                : cloudData || "No data in cloud storage"
              }
            </pre>
          </div>
          <p className="text-xs text-muted-foreground mt-3 font-medium">
            Current cloud data size: <span className="text-success font-bold">{cloudData.length.toLocaleString()}</span> bytes
          </p>
        </CardContent>
      </Card>
    </div>
  );
};