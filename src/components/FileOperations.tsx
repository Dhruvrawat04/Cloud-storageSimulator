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
    <div className="grid grid-cols-1 lg:grid-cols-2 gap-6 mb-6">
      <Card className="bg-gradient-card shadow-card border-0">
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <Upload className="h-5 w-5 text-primary" />
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

      <Card className="bg-gradient-card shadow-card border-0">
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <Download className="h-5 w-5 text-accent" />
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

      <Card className="lg:col-span-2 bg-gradient-card shadow-card border-0">
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <FileText className="h-5 w-5 text-success" />
            Cloud Data Preview
          </CardTitle>
        </CardHeader>
        <CardContent>
          <div className="bg-muted/50 rounded-lg p-4 max-h-48 overflow-y-auto">
            <pre className="text-sm text-muted-foreground whitespace-pre-wrap">
              {cloudData.length > 1000 
                ? cloudData.substring(0, 1000) + "\n... (truncated, " + cloudData.length + " total bytes)"
                : cloudData || "No data in cloud storage"
              }
            </pre>
          </div>
          <p className="text-xs text-muted-foreground mt-2">
            Current cloud data size: {cloudData.length.toLocaleString()} bytes
          </p>
        </CardContent>
      </Card>
    </div>
  );
};