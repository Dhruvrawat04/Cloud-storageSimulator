import { CloudHeader } from "@/components/CloudHeader";
import { FileOperations } from "@/components/FileOperations";
import { ConnectionStatus } from "@/components/ConnectionStatus";
import { useBackendCloud } from "@/hooks/useBackendCloud";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";
import { Badge } from "@/components/ui/badge";
import { FolderOpen, FileText, HardDrive, Database } from "lucide-react";

const Files = () => {
  const { cloudData, files, stats, isConnected, uploadFile, downloadFile } = useBackendCloud();

  const dataSizeInKB = cloudData ? (cloudData.length / 1024).toFixed(2) : '0.00';

  return (
    <div className="min-h-screen bg-gradient-to-br from-background via-background to-muted/20">
      <CloudHeader />
      
      <main className="container mx-auto px-6 py-8">
        <div className="mb-6 flex flex-col sm:flex-row justify-between items-start sm:items-center gap-4 animate-fade-in-up">
          <div>
            <h1 className="text-3xl md:text-4xl font-bold bg-gradient-to-r from-primary via-indigo-500 to-purple-500 bg-clip-text text-transparent">
              File Management
            </h1>
            <p className="text-muted-foreground mt-2">Upload, download, and manage your cloud storage files</p>
          </div>
          <ConnectionStatus isConnected={isConnected} />
        </div>

        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6 mb-8 animate-fade-in-up" style={{ animationDelay: '0.1s' }}>
          <Card className="border-0 shadow-lg bg-gradient-to-br from-indigo-500/5 to-indigo-600/5">
            <CardHeader className="pb-3">
              <CardTitle className="text-sm font-medium text-muted-foreground">Total Files</CardTitle>
            </CardHeader>
            <CardContent>
              <div className="flex items-center gap-3">
                <FolderOpen className="w-8 h-8 text-indigo-500" />
                <div>
                  <p className="text-3xl font-bold">{stats?.totalFiles || 0}</p>
                  <p className="text-xs text-muted-foreground">In Storage</p>
                </div>
              </div>
            </CardContent>
          </Card>

          <Card className="border-0 shadow-lg bg-gradient-to-br from-purple-500/5 to-purple-600/5">
            <CardHeader className="pb-3">
              <CardTitle className="text-sm font-medium text-muted-foreground">Storage Used</CardTitle>
            </CardHeader>
            <CardContent>
              <div className="flex items-center gap-3">
                <HardDrive className="w-8 h-8 text-purple-500" />
                <div>
                  <p className="text-3xl font-bold">{dataSizeInKB}</p>
                  <p className="text-xs text-muted-foreground">KB</p>
                </div>
              </div>
            </CardContent>
          </Card>

          <Card className="border-0 shadow-lg bg-gradient-to-br from-blue-500/5 to-blue-600/5">
            <CardHeader className="pb-3">
              <CardTitle className="text-sm font-medium text-muted-foreground">Cloud Size</CardTitle>
            </CardHeader>
            <CardContent>
              <div className="flex items-center gap-3">
                <Database className="w-8 h-8 text-blue-500" />
                <div>
                  <p className="text-3xl font-bold">{stats?.cloudDataSize || 0}</p>
                  <p className="text-xs text-muted-foreground">Bytes</p>
                </div>
              </div>
            </CardContent>
          </Card>

          <Card className="border-0 shadow-lg bg-gradient-to-br from-green-500/5 to-green-600/5">
            <CardHeader className="pb-3">
              <CardTitle className="text-sm font-medium text-muted-foreground">File Cache</CardTitle>
            </CardHeader>
            <CardContent>
              <div className="flex items-center gap-3">
                <FileText className="w-8 h-8 text-green-500" />
                <div>
                  <p className="text-3xl font-bold">{files.length}</p>
                  <p className="text-xs text-muted-foreground">Cached Files</p>
                </div>
              </div>
            </CardContent>
          </Card>
        </div>

        <div className="animate-fade-in-up" style={{ animationDelay: '0.2s' }}>
          <FileOperations 
            onUpload={uploadFile}
            onDownload={downloadFile}
            cloudData={cloudData}
          />
        </div>

        {files.length > 0 && (
          <Card className="mt-8 border-0 shadow-lg animate-fade-in-up" style={{ animationDelay: '0.3s' }}>
            <CardHeader>
              <CardTitle className="flex items-center gap-2">
                <FolderOpen className="w-5 h-5" />
                Cached Files
              </CardTitle>
              <CardDescription>Files currently in cache memory</CardDescription>
            </CardHeader>
            <CardContent>
              <div className="space-y-2 max-h-64 overflow-y-auto">
                {files.map((file: any, index: number) => (
                  <div key={index} className="flex items-center justify-between p-3 rounded-lg bg-muted/50 hover:bg-muted transition-colors">
                    <div className="flex items-center gap-3">
                      <FileText className="w-4 h-4 text-muted-foreground" />
                      <div>
                        <p className="text-sm font-medium">{file.name || `File ${index + 1}`}</p>
                        <p className="text-xs text-muted-foreground">
                          {file.modified || 'Unknown date'}
                        </p>
                      </div>
                    </div>
                    <Badge variant="outline">{file.size || 0} bytes</Badge>
                  </div>
                ))}
              </div>
            </CardContent>
          </Card>
        )}
      </main>
    </div>
  );
};

export default Files;
