import { useState, useEffect } from 'react';
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from '@/components/ui/card';
import { Button } from '@/components/ui/button';
import { Tabs, TabsContent, TabsList, TabsTrigger } from '@/components/ui/tabs';
import { Table, TableBody, TableCell, TableHead, TableHeader, TableRow } from '@/components/ui/table';
import { Badge } from '@/components/ui/badge';
import { Progress } from '@/components/ui/progress';
import { Dialog, DialogContent, DialogDescription, DialogFooter, DialogHeader, DialogTitle, DialogTrigger } from '@/components/ui/dialog';
import { Label } from '@/components/ui/label';
import { Input } from '@/components/ui/input';
import { cloudApi } from '@/services/cloudApi';
import { useToast } from '@/hooks/use-toast';
import { Cpu, FolderTree, MessageSquare, AlertTriangle, Play, TrendingUp, Clock, Activity, Plus, Edit, Trash2 } from 'lucide-react';
import { DeadlockGraphs } from '@/components/DeadlockGraphs';
import { AlertDialog, AlertDialogAction, AlertDialogCancel, AlertDialogContent, AlertDialogDescription, AlertDialogFooter, AlertDialogHeader, AlertDialogTitle } from '@/components/ui/alert-dialog';
import { BarChart, Bar, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer, PieChart, Pie, Cell, RadarChart, PolarGrid, PolarAngleAxis, PolarRadiusAxis, Radar } from 'recharts';

export default function OSSimulator() {
  const [processStats, setProcessStats] = useState<any>(null);
  const [fsStats, setFsStats] = useState<any>(null);
  const [ipcStats, setIpcStats] = useState<any>(null);
  const [deadlockStats, setDeadlockStats] = useState<any>(null);
  const [waitForGraph, setWaitForGraph] = useState<any>(null);
  const [loading, setLoading] = useState(false);
  const [schedulingActive, setSchedulingActive] = useState(false);
  const [lastAlgorithm, setLastAlgorithm] = useState<string | null>(null);
  const [lastQuantum, setLastQuantum] = useState<number>(2);
  const [dialogOpen, setDialogOpen] = useState(false);
  const [editDialogOpen, setEditDialogOpen] = useState(false);
  const [deleteDialogOpen, setDeleteDialogOpen] = useState(false);
  const [selectedProcess, setSelectedProcess] = useState<any>(null);
  const [sharedMemDialog, setSharedMemDialog] = useState(false);
  const [readMemDialog, setReadMemDialog] = useState(false);
  const [writeMemDialog, setWriteMemDialog] = useState(false);
  const [manualProcess, setManualProcess] = useState({
    processName: '',
    arrivalTime: 0,
    burstTime: 1,
    priority: 1,
  });
  const [sharedMemory, setSharedMemory] = useState({
    name: '',
    size: 1024,
    data: '',
  });
  const [memoryRead, setMemoryRead] = useState({
    name: '',
    data: '',
  });
  const [memoryWrite, setMemoryWrite] = useState({
    name: '',
    data: '',
  });
  const { toast } = useToast();

  const fetchAllStats = async (skipProcesses = false) => {
    try {
      const promises: Promise<any>[] = [];
      
      if (!skipProcesses) promises.push(cloudApi.getProcessStats());
      promises.push(cloudApi.getFileSystemStats());
      promises.push(cloudApi.getIPCStats());
      promises.push(cloudApi.getDeadlockStats());
      
      const results = await Promise.all(promises);
      
      let index = 0;
      if (!skipProcesses) setProcessStats(results[index++]);
      setFsStats(results[index++]);
      setIpcStats(results[index++]);
      setDeadlockStats(results[index++]);
    } catch (error) {
      console.error('Failed to fetch OS stats:', error);
    }
  };

  useEffect(() => {
    fetchAllStats();
    const interval = setInterval(() => {
      fetchAllStats(schedulingActive);
    }, 10000);
    return () => clearInterval(interval);
  }, [schedulingActive]);

  const runProcessScheduler = async (algorithm: string, quantum: number = 2) => {
    setLoading(true);
    setSchedulingActive(true);
    setLastAlgorithm(algorithm);
    setLastQuantum(quantum);
    try {
      const result = await cloudApi.scheduleProcesses(algorithm, quantum, 5);
      setProcessStats(result);
      toast({
        title: 'Process Scheduler',
        description: `${algorithm} scheduling completed successfully. Results will persist until next run.`,
      });
    } catch (error) {
      toast({
        title: 'Error',
        description: 'Failed to run process scheduler',
        variant: 'destructive',
      });
      setSchedulingActive(false);
    } finally {
      setLoading(false);
    }
  };

  const createFile = async () => {
    setLoading(true);
    try {
      const filename = `/test_${Date.now()}.txt`;
      const result = await cloudApi.createOSFile(filename, 'Sample file content');
      if (result.success) {
        await fetchAllStats();
        toast({
          title: 'File System',
          description: `Created file: ${filename}`,
        });
      }
    } catch (error) {
      toast({
        title: 'Error',
        description: 'Failed to create file',
        variant: 'destructive',
      });
    } finally {
      setLoading(false);
    }
  };

  const sendMessage = async () => {
    setLoading(true);
    try {
      const result = await cloudApi.sendIPCMessage(1, 100, 200, `Test message at ${new Date().toLocaleTimeString()}`);
      if (result.success) {
        await fetchAllStats();
        toast({
          title: 'IPC Message',
          description: 'Message sent successfully',
        });
      }
    } catch (error) {
      toast({
        title: 'Error',
        description: 'Failed to send IPC message',
        variant: 'destructive',
      });
    } finally {
      setLoading(false);
    }
  };

  const simulateDeadlock = async () => {
    setLoading(true);
    try {
      const result = await cloudApi.simulateDeadlock();
      await fetchAllStats();
      // Also fetch visualization data
      const graphData = await cloudApi.visualizeDeadlock();
      setWaitForGraph(graphData);
      toast({
        title: 'Deadlock Simulation',
        description: result.deadlockCreated ? 'Deadlock created!' : 'No deadlock detected',
        variant: result.deadlockCreated ? 'destructive' : 'default',
      });
    } catch (error) {
      toast({
        title: 'Error',
        description: 'Failed to simulate deadlock',
        variant: 'destructive',
      });
    } finally {
      setLoading(false);
    }
  };

  const visualizeWaitForGraph = async () => {
    setLoading(true);
    try {
      const result = await cloudApi.visualizeDeadlock();
      setWaitForGraph(result);
      toast({
        title: 'Wait-For Graph',
        description: result.hasDeadlock ? 'Deadlock cycle detected!' : 'No circular dependencies found',
        variant: result.hasDeadlock ? 'destructive' : 'default',
      });
    } catch (error) {
      toast({
        title: 'Error',
        description: 'Failed to visualize wait-for graph',
        variant: 'destructive',
      });
    } finally {
      setLoading(false);
    }
  };

  const recoverFromDeadlock = async () => {
    setLoading(true);
    try {
      const result = await cloudApi.recoverFromDeadlock();
      await fetchAllStats();
      setWaitForGraph(null);
      toast({
        title: 'Deadlock Recovery',
        description: `Terminated ${result.processesTerminated} process(es). System ${result.stillDeadlocked ? 'still deadlocked' : 'recovered'}`,
      });
    } catch (error) {
      toast({
        title: 'Error',
        description: 'Failed to recover from deadlock',
        variant: 'destructive',
      });
    } finally {
      setLoading(false);
    }
  };

  const editProcess = async () => {
    if (!selectedProcess || !selectedProcess.processName.trim()) {
      toast({
        title: 'Validation Error',
        description: 'Please enter a process name',
        variant: 'destructive',
      });
      return;
    }

    setLoading(true);
    try {
      const result = await cloudApi.editProcess(
        selectedProcess.pid,
        selectedProcess.processName,
        selectedProcess.arrivalTime,
        selectedProcess.burstTime,
        selectedProcess.priority
      );
      
      if (result.success) {
        await cloudApi.getProcessStats().then(setProcessStats);
        toast({
          title: 'Process Updated',
          description: `${selectedProcess.processName} (P${selectedProcess.pid}) updated successfully`,
        });
        setEditDialogOpen(false);
        setSelectedProcess(null);
      }
    } catch (error) {
      toast({
        title: 'Error',
        description: 'Failed to edit process',
        variant: 'destructive',
      });
    } finally {
      setLoading(false);
    }
  };

  const deleteProcess = async () => {
    if (!selectedProcess) return;

    setLoading(true);
    try {
      const result = await cloudApi.deleteProcess(selectedProcess.pid);
      
      if (result.success) {
        // Fetch updated stats (backend already re-ran scheduler if needed)
        const updatedStats = await cloudApi.getProcessStats();
        
        // If no processes left, clear the Gantt chart and reset scheduling state
        if (!updatedStats.processes || updatedStats.processes.length === 0) {
          setProcessStats({
            ...updatedStats,
            ganttChart: [],
            averageWaitingTime: 0,
            averageTurnaroundTime: 0,
            algorithm: 'None',
            processCount: 0,
          });
          setSchedulingActive(false);
          setLastAlgorithm(null);
        } else {
          setProcessStats(updatedStats);
        }
        
        toast({
          title: 'Process Deleted',
          description: `${result.deletedProcess.processName} (P${result.deletedProcess.pid}) deleted successfully`,
        });
        setDeleteDialogOpen(false);
        setSelectedProcess(null);
      }
    } catch (error) {
      toast({
        title: 'Error',
        description: 'Failed to delete process',
        variant: 'destructive',
      });
    } finally {
      setLoading(false);
    }
  };

  const createSharedMem = async () => {
    setLoading(true);
    try {
      const result = await cloudApi.createSharedMemory(sharedMemory.name, sharedMemory.size, sharedMemory.data);
      if (result.success) {
        await fetchAllStats();
        setSharedMemDialog(false);
        setSharedMemory({ name: '', size: 1024, data: '' });
        toast({
          title: 'Shared Memory',
          description: `Created segment "${result.name}" (${result.size} bytes)`,
        });
      }
    } catch (error) {
      toast({
        title: 'Error',
        description: 'Failed to create shared memory',
        variant: 'destructive',
      });
    } finally {
      setLoading(false);
    }
  };

  const readSharedMem = async () => {
    if (!memoryRead.name.trim()) {
      toast({
        title: 'Validation Error',
        description: 'Please enter a memory segment name',
        variant: 'destructive',
      });
      return;
    }

    setLoading(true);
    try {
      const result = await cloudApi.readSharedMemory(memoryRead.name);
      if (result.success) {
        setMemoryRead({ ...memoryRead, data: result.data });
        toast({
          title: 'Shared Memory Read',
          description: `Successfully read from segment "${result.name}"`,
        });
      }
    } catch (error) {
      toast({
        title: 'Error',
        description: 'Failed to read shared memory. Segment may not exist.',
        variant: 'destructive',
      });
    } finally {
      setLoading(false);
    }
  };

  const writeSharedMem = async () => {
    if (!memoryWrite.name.trim()) {
      toast({
        title: 'Validation Error',
        description: 'Please enter a memory segment name',
        variant: 'destructive',
      });
      return;
    }

    setLoading(true);
    try {
      const result = await cloudApi.writeSharedMemory(memoryWrite.name, memoryWrite.data);
      if (result.success) {
        toast({
          title: 'Shared Memory Write',
          description: `Successfully wrote to segment "${result.name}"`,
        });
        setWriteMemDialog(false);
        setMemoryWrite({ name: '', data: '' });
      }
    } catch (error) {
      toast({
        title: 'Error',
        description: 'Failed to write shared memory. Segment may not exist.',
        variant: 'destructive',
      });
    } finally {
      setLoading(false);
    }
  };

  const addManualProcess = async () => {
    if (!manualProcess.processName.trim()) {
      toast({
        title: 'Validation Error',
        description: 'Please enter a process name',
        variant: 'destructive',
      });
      return;
    }

    setLoading(true);
    try {
      const result = await cloudApi.addManualProcess(
        manualProcess.processName,
        manualProcess.arrivalTime,
        manualProcess.burstTime,
        manualProcess.priority
      );
      
      if (result.success) {
        await cloudApi.getProcessStats().then(setProcessStats);
        toast({
          title: 'Process Added',
          description: `${manualProcess.processName} (P${result.process.pid}) added successfully`,
        });
        setDialogOpen(false);
        setManualProcess({ processName: '', arrivalTime: 0, burstTime: 1, priority: 1 });
      }
    } catch (error) {
      toast({
        title: 'Error',
        description: 'Failed to add process',
        variant: 'destructive',
      });
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="container mx-auto p-4 md:p-6 space-y-6 bg-gradient-to-br from-background via-background to-primary/5 min-h-screen">
      <div className="flex flex-col md:flex-row items-start md:items-center justify-between gap-4">
        <div className="space-y-2">
          <h1 className="text-4xl md:text-5xl font-bold bg-gradient-to-r from-primary via-purple-500 to-pink-500 bg-clip-text text-transparent">
            Operating System Simulator
          </h1>
          <p className="text-muted-foreground text-lg">
            Comprehensive simulation of core OS concepts with real-time visualization
          </p>
        </div>
        <div className="flex items-center gap-2">
          <Activity className="w-5 h-5 text-green-500 animate-pulse" />
          <span className="text-sm text-muted-foreground">Live Simulation</span>
        </div>
      </div>

      <Tabs defaultValue="processes" className="space-y-6">
        <TabsList className="grid w-full grid-cols-2 md:grid-cols-4 h-auto gap-2 bg-muted/50 p-2">
          <TabsTrigger value="processes" className="data-[state=active]:bg-primary data-[state=active]:text-primary-foreground">
            <Cpu className="w-4 h-4 mr-2" />
            <span className="hidden sm:inline">CPU Scheduling</span>
            <span className="sm:hidden">CPU</span>
          </TabsTrigger>
          <TabsTrigger value="filesystem" className="data-[state=active]:bg-primary data-[state=active]:text-primary-foreground">
            <FolderTree className="w-4 h-4 mr-2" />
            <span className="hidden sm:inline">File System</span>
            <span className="sm:hidden">FS</span>
          </TabsTrigger>
          <TabsTrigger value="ipc" className="data-[state=active]:bg-primary data-[state=active]:text-primary-foreground">
            <MessageSquare className="w-4 h-4 mr-2" />
            IPC
          </TabsTrigger>
          <TabsTrigger value="deadlock" className="data-[state=active]:bg-primary data-[state=active]:text-primary-foreground">
            <AlertTriangle className="w-4 h-4 mr-2" />
            <span className="hidden sm:inline">Deadlock</span>
            <span className="sm:hidden">Lock</span>
          </TabsTrigger>
        </TabsList>

        {/* Process Scheduler Tab */}
        <TabsContent value="processes" className="space-y-4">
          <Card className="border-2 shadow-lg">
            <CardHeader className="bg-gradient-to-r from-blue-500/10 to-purple-500/10">
              <CardTitle className="flex items-center gap-2">
                <Cpu className="w-6 h-6" />
                CPU Process Scheduler
              </CardTitle>
              <CardDescription>
                Simulate different scheduling algorithms and view detailed process execution
              </CardDescription>
            </CardHeader>
            <CardContent className="space-y-6 pt-6">
              <div className="flex flex-col gap-3">
                <div className="flex justify-between items-center">
                  <p className="text-sm text-muted-foreground">Choose a scheduling algorithm:</p>
                  <Dialog open={dialogOpen} onOpenChange={setDialogOpen}>
                    <DialogTrigger asChild>
                      <Button variant="outline" size="sm" className="gap-2">
                        <Plus className="w-4 h-4" />
                        Add Manual Process
                      </Button>
                    </DialogTrigger>
                    <DialogContent>
                      <DialogHeader>
                        <DialogTitle>Add Custom Process</DialogTitle>
                        <DialogDescription>
                          Enter the details for your custom process
                        </DialogDescription>
                      </DialogHeader>
                      <div className="grid gap-4 py-4">
                        <div className="grid grid-cols-4 items-center gap-4">
                          <Label htmlFor="processName" className="text-right">
                            Name
                          </Label>
                          <Input
                            id="processName"
                            value={manualProcess.processName}
                            onChange={(e) => setManualProcess({...manualProcess, processName: e.target.value})}
                            placeholder="e.g., Web Browser"
                            className="col-span-3"
                          />
                        </div>
                        <div className="grid grid-cols-4 items-center gap-4">
                          <Label htmlFor="arrivalTime" className="text-right">
                            Arrival Time
                          </Label>
                          <Input
                            id="arrivalTime"
                            type="number"
                            value={manualProcess.arrivalTime}
                            onChange={(e) => setManualProcess({...manualProcess, arrivalTime: parseInt(e.target.value) || 0})}
                            className="col-span-3"
                          />
                        </div>
                        <div className="grid grid-cols-4 items-center gap-4">
                          <Label htmlFor="burstTime" className="text-right">
                            Burst Time
                          </Label>
                          <Input
                            id="burstTime"
                            type="number"
                            value={manualProcess.burstTime}
                            onChange={(e) => setManualProcess({...manualProcess, burstTime: parseInt(e.target.value) || 1})}
                            className="col-span-3"
                          />
                        </div>
                        <div className="grid grid-cols-4 items-center gap-4">
                          <Label htmlFor="priority" className="text-right">
                            Priority
                          </Label>
                          <Input
                            id="priority"
                            type="number"
                            value={manualProcess.priority}
                            onChange={(e) => setManualProcess({...manualProcess, priority: parseInt(e.target.value) || 1})}
                            className="col-span-3"
                          />
                        </div>
                      </div>
                      <DialogFooter>
                        <Button onClick={addManualProcess} disabled={loading}>
                          Add Process
                        </Button>
                      </DialogFooter>
                    </DialogContent>
                  </Dialog>
                </div>
                <div className="grid grid-cols-2 md:grid-cols-4 gap-3">
                  <Button 
                    onClick={() => runProcessScheduler('FCFS', 2)} 
                    disabled={loading}
                    className="bg-gradient-to-r from-blue-500 to-blue-600 hover:from-blue-600 hover:to-blue-700"
                  >
                    FCFS
                  </Button>
                  <Button 
                    onClick={() => runProcessScheduler('SJF', 2)} 
                    disabled={loading}
                    className="bg-gradient-to-r from-green-500 to-green-600 hover:from-green-600 hover:to-green-700"
                  >
                    SJF
                  </Button>
                  <Button 
                    onClick={() => runProcessScheduler('RR', 3)} 
                    disabled={loading}
                    className="bg-gradient-to-r from-orange-500 to-orange-600 hover:from-orange-600 hover:to-orange-700"
                  >
                    Round Robin
                  </Button>
                  <Button 
                    onClick={() => runProcessScheduler('PRIORITY', 2)} 
                    disabled={loading}
                    className="bg-gradient-to-r from-purple-500 to-purple-600 hover:from-purple-600 hover:to-purple-700"
                  >
                    Priority
                  </Button>
                </div>
              </div>
              
              {!processStats && (
                <Card className="bg-muted/30 border-dashed border-2">
                  <CardContent className="pt-6 text-center py-12">
                    <Cpu className="w-16 h-16 mx-auto text-muted-foreground/50 mb-4" />
                    <p className="text-lg font-semibold text-muted-foreground">No Scheduling Results Yet</p>
                    <p className="text-sm text-muted-foreground mt-2">
                      Click any algorithm button above to start process scheduling simulation
                    </p>
                  </CardContent>
                </Card>
              )}

              {processStats && (
                <>
                  <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
                    <Card className="bg-gradient-to-br from-blue-500/10 to-blue-600/5 border-blue-200">
                      <CardContent className="pt-6">
                        <div className="flex items-center justify-between">
                          <div>
                            <div className="text-3xl font-bold text-blue-600">
                              {processStats.averageWaitingTime?.toFixed(2) || '0.00'}
                            </div>
                            <p className="text-xs text-muted-foreground mt-1">Avg Wait Time (ms)</p>
                          </div>
                          <Clock className="w-8 h-8 text-blue-500 opacity-50" />
                        </div>
                      </CardContent>
                    </Card>
                    <Card className="bg-gradient-to-br from-green-500/10 to-green-600/5 border-green-200">
                      <CardContent className="pt-6">
                        <div className="flex items-center justify-between">
                          <div>
                            <div className="text-3xl font-bold text-green-600">
                              {processStats.averageTurnaroundTime?.toFixed(2) || '0.00'}
                            </div>
                            <p className="text-xs text-muted-foreground mt-1">Avg Turnaround (ms)</p>
                          </div>
                          <TrendingUp className="w-8 h-8 text-green-500 opacity-50" />
                        </div>
                      </CardContent>
                    </Card>
                    <Card className="bg-gradient-to-br from-purple-500/10 to-purple-600/5 border-purple-200">
                      <CardContent className="pt-6">
                        <div className="flex items-center justify-between">
                          <div>
                            <div className="text-3xl font-bold text-purple-600">
                              {processStats.processCount || 0}
                            </div>
                            <p className="text-xs text-muted-foreground mt-1">Total Processes</p>
                          </div>
                          <Cpu className="w-8 h-8 text-purple-500 opacity-50" />
                        </div>
                      </CardContent>
                    </Card>
                    <Card className="bg-gradient-to-br from-orange-500/10 to-orange-600/5 border-orange-200">
                      <CardContent className="pt-6">
                        <div className="flex items-center justify-between">
                          <div>
                            <div className="text-2xl font-bold text-orange-600">
                              {processStats.algorithm || 'N/A'}
                            </div>
                            <p className="text-xs text-muted-foreground mt-1">Algorithm</p>
                          </div>
                          <Activity className="w-8 h-8 text-orange-500 opacity-50" />
                        </div>
                      </CardContent>
                    </Card>
                  </div>

                  {processStats.ganttChart && processStats.ganttChart.length > 0 && (
                    <Card className="bg-gradient-to-br from-indigo-50 to-purple-50 dark:from-indigo-950/20 dark:to-purple-950/20">
                      <CardHeader>
                        <CardTitle className="text-lg flex items-center gap-2">
                          📊 Gantt Chart Visualization
                        </CardTitle>
                        <CardDescription>Process execution timeline (including CPU idle time)</CardDescription>
                      </CardHeader>
                      <CardContent>
                        <div className="space-y-4">
                          {(() => {
                            const maxTime = processStats.ganttChart[processStats.ganttChart.length - 1].endTime;
                            const colors = ['bg-blue-500', 'bg-green-500', 'bg-orange-500', 'bg-purple-500', 'bg-pink-500', 'bg-cyan-500'];
                            
                            // Create timeline for each process
                            return processStats.ganttChart.map((entry: any, index: number) => {
                              const color = colors[entry.processId % colors.length];
                              const duration = entry.endTime - entry.startTime;
                              const timeSlots = [];
                              
                              // Add idle blocks before process starts
                              for (let t = 0; t < entry.startTime; t++) {
                                timeSlots.push(
                                  <div
                                    key={`idle-${t}`}
                                    className="h-8 bg-gray-200 dark:bg-gray-700 border border-gray-300 dark:border-gray-600 flex items-center justify-center text-xs text-gray-400"
                                    style={{ width: `${(1 / maxTime) * 100}%` }}
                                  >
                                    ✕
                                  </div>
                                );
                              }
                              
                              // Add execution blocks
                              for (let t = entry.startTime; t < entry.endTime; t++) {
                                timeSlots.push(
                                  <div
                                    key={`exec-${t}`}
                                    className={`h-8 ${color} flex items-center justify-center text-white text-xs font-bold border border-white/20`}
                                    style={{ width: `${(1 / maxTime) * 100}%` }}
                                  >
                                    {t === entry.startTime ? entry.processId : ''}
                                  </div>
                                );
                              }
                              
                              // Add idle blocks after process ends
                              for (let t = entry.endTime; t < maxTime; t++) {
                                timeSlots.push(
                                  <div
                                    key={`idle-after-${t}`}
                                    className="h-8 bg-gray-200 dark:bg-gray-700 border border-gray-300 dark:border-gray-600 flex items-center justify-center text-xs text-gray-400"
                                    style={{ width: `${(1 / maxTime) * 100}%` }}
                                  >
                                    ✕
                                  </div>
                                );
                              }
                              
                              return (
                                <div key={index}>
                                  <div className="flex items-center gap-3 mb-1">
                                    <div className="w-32 text-sm font-semibold truncate">
                                      {entry.processName || `P${entry.processId}`}
                                    </div>
                                    <div className="text-xs text-muted-foreground">
                                      {entry.startTime} → {entry.endTime} ({duration} units)
                                    </div>
                                  </div>
                                  <div className="flex items-center gap-3">
                                    <div className="w-32 text-xs text-muted-foreground">
                                      Timeline:
                                    </div>
                                    <div className="flex-1 flex overflow-x-auto">
                                      {timeSlots}
                                    </div>
                                  </div>
                                </div>
                              );
                            });
                          })()}
                          
                          <div className="flex items-center gap-3 mt-4 pt-4 border-t">
                            <div className="w-32 text-xs font-semibold">
                              Legend:
                            </div>
                            <div className="flex gap-4 text-xs">
                              <div className="flex items-center gap-2">
                                <div className="w-6 h-6 bg-gray-200 dark:bg-gray-700 border border-gray-300 flex items-center justify-center text-gray-400">✕</div>
                                <span>CPU Idle</span>
                              </div>
                              <div className="flex items-center gap-2">
                                <div className="w-6 h-6 bg-blue-500 border border-white/20"></div>
                                <span>Process Executing</span>
                              </div>
                            </div>
                          </div>
                        </div>
                      </CardContent>
                    </Card>
                  )}

                  {processStats.processes && processStats.processes.length > 0 && (
                    <Card>
                      <CardHeader>
                        <CardTitle className="text-lg">Detailed Process Execution Table</CardTitle>
                        <CardDescription>Individual process metrics and timing information</CardDescription>
                      </CardHeader>
                      <CardContent>
                        <div className="overflow-x-auto">
                          <Table>
                            <TableHeader>
                              <TableRow>
                                <TableHead className="font-bold">PID</TableHead>
                                <TableHead className="font-bold">Name</TableHead>
                                <TableHead className="font-bold">Arrival</TableHead>
                                <TableHead className="font-bold">Burst</TableHead>
                                <TableHead className="font-bold">Priority</TableHead>
                                <TableHead className="font-bold">Start</TableHead>
                                <TableHead className="font-bold">Complete</TableHead>
                                <TableHead className="font-bold">Waiting</TableHead>
                                <TableHead className="font-bold">Turnaround</TableHead>
                                <TableHead className="font-bold">Actions</TableHead>
                              </TableRow>
                            </TableHeader>
                            <TableBody>
                              {processStats.processes.map((proc: any) => (
                                <TableRow key={proc.pid} className="hover:bg-muted/50">
                                  <TableCell className="font-semibold">
                                    <Badge variant="outline">P{proc.pid}</Badge>
                                  </TableCell>
                                  <TableCell className="font-medium text-sm">
                                    {proc.processName || `Process ${proc.pid}`}
                                  </TableCell>
                                  <TableCell>{proc.arrivalTime}</TableCell>
                                  <TableCell>{proc.burstTime}</TableCell>
                                  <TableCell>
                                    <Badge variant={proc.priority < 3 ? 'destructive' : 'secondary'}>
                                      {proc.priority}
                                    </Badge>
                                  </TableCell>
                                  <TableCell>{proc.startTime}</TableCell>
                                  <TableCell>{proc.completionTime}</TableCell>
                                  <TableCell className="font-semibold text-blue-600">
                                    {proc.waitingTime}
                                  </TableCell>
                                  <TableCell className="font-semibold text-green-600">
                                    {proc.turnaroundTime}
                                  </TableCell>
                                  <TableCell>
                                    <div className="flex gap-2">
                                      <Button
                                        size="sm"
                                        variant="outline"
                                        onClick={() => {
                                          setSelectedProcess(proc);
                                          setEditDialogOpen(true);
                                        }}
                                        disabled={loading}
                                      >
                                        <Edit className="w-3 h-3" />
                                      </Button>
                                      <Button
                                        size="sm"
                                        variant="destructive"
                                        onClick={() => {
                                          setSelectedProcess(proc);
                                          setDeleteDialogOpen(true);
                                        }}
                                        disabled={loading}
                                      >
                                        <Trash2 className="w-3 h-3" />
                                      </Button>
                                    </div>
                                  </TableCell>
                                </TableRow>
                              ))}
                            </TableBody>
                          </Table>
                        </div>
                      </CardContent>
                    </Card>
                  )}
                </>
              )}
            </CardContent>
          </Card>
        </TabsContent>

        {/* File System Tab */}
        <TabsContent value="filesystem" className="space-y-4">
          <Card className="border-2 shadow-lg">
            <CardHeader className="bg-gradient-to-r from-orange-500/10 to-amber-500/10">
              <CardTitle className="flex items-center gap-2">
                <FolderTree className="w-6 h-6" />
                File System Simulator
              </CardTitle>
              <CardDescription>
                Create files and manage disk space allocation
              </CardDescription>
            </CardHeader>
            <CardContent className="space-y-6 pt-6">
              <Button 
                onClick={createFile} 
                disabled={loading}
                className="bg-gradient-to-r from-orange-500 to-orange-600"
              >
                <Play className="w-4 h-4 mr-2" />
                Create Test File
              </Button>
              
              {!fsStats && (
                <Card className="bg-muted/30 border-dashed border-2">
                  <CardContent className="pt-6 text-center py-12">
                    <FolderTree className="w-16 h-16 mx-auto text-muted-foreground/50 mb-4" />
                    <p className="text-lg font-semibold text-muted-foreground">File System Not Initialized</p>
                    <p className="text-sm text-muted-foreground mt-2">
                      Create a test file to see file system metrics
                    </p>
                  </CardContent>
                </Card>
              )}

              {fsStats && (
                <div className="grid grid-cols-2 md:grid-cols-3 gap-4">
                  <Card className="bg-gradient-to-br from-orange-500/10 to-orange-600/5">
                    <CardContent className="pt-6">
                      <div className="text-3xl font-bold">{fsStats.totalBlocks}</div>
                      <p className="text-xs text-muted-foreground mt-1">Total Blocks</p>
                    </CardContent>
                  </Card>
                  <Card className="bg-gradient-to-br from-amber-500/10 to-amber-600/5">
                    <CardContent className="pt-6">
                      <div className="text-3xl font-bold">{fsStats.blockSize}</div>
                      <p className="text-xs text-muted-foreground mt-1">Block Size (bytes)</p>
                    </CardContent>
                  </Card>
                  <Card className="bg-gradient-to-br from-yellow-500/10 to-yellow-600/5">
                    <CardContent className="pt-6">
                      <div className="text-3xl font-bold">
                        {(fsStats.utilization * 100).toFixed(1)}%
                      </div>
                      <p className="text-xs text-muted-foreground mt-1">Disk Utilization</p>
                      <Progress value={fsStats.utilization * 100} className="mt-2" />
                    </CardContent>
                  </Card>
                </div>
              )}
            </CardContent>
          </Card>
        </TabsContent>

        {/* IPC Tab */}
        <TabsContent value="ipc" className="space-y-4">
          <Card className="border-2 shadow-lg">
            <CardHeader className="bg-gradient-to-r from-cyan-500/10 to-blue-500/10">
              <CardTitle className="flex items-center gap-2">
                <MessageSquare className="w-6 h-6" />
                Inter-Process Communication
              </CardTitle>
              <CardDescription>
                Simulate message passing between processes
              </CardDescription>
            </CardHeader>
            <CardContent className="space-y-6 pt-6">
              <div className="flex gap-2 flex-wrap">
                <Button 
                  onClick={sendMessage} 
                  disabled={loading}
                  className="bg-gradient-to-r from-cyan-500 to-cyan-600"
                >
                  <MessageSquare className="w-4 h-4 mr-2" />
                  Send Test Message
                </Button>
                
                <Dialog open={sharedMemDialog} onOpenChange={setSharedMemDialog}>
                  <DialogTrigger asChild>
                    <Button variant="outline" className="border-blue-500 text-blue-600 hover:bg-blue-500/10">
                      <Plus className="w-4 h-4 mr-2" />
                      Create Shared Memory
                    </Button>
                  </DialogTrigger>
                  <DialogContent>
                    <DialogHeader>
                      <DialogTitle>Create Shared Memory Segment</DialogTitle>
                      <DialogDescription>
                        Allocate a new shared memory segment for IPC
                      </DialogDescription>
                    </DialogHeader>
                    <div className="space-y-4 py-4">
                      <div className="space-y-2">
                        <Label htmlFor="mem-name">Memory Segment Name</Label>
                        <Input
                          id="mem-name"
                          placeholder="e.g., buffer_1"
                          value={sharedMemory.name}
                          onChange={(e) => setSharedMemory({ ...sharedMemory, name: e.target.value })}
                        />
                      </div>
                      <div className="space-y-2">
                        <Label htmlFor="mem-size">Size (bytes)</Label>
                        <Input
                          id="mem-size"
                          type="number"
                          min="128"
                          value={sharedMemory.size}
                          onChange={(e) => setSharedMemory({ ...sharedMemory, size: parseInt(e.target.value) || 1024 })}
                        />
                      </div>
                      <div className="space-y-2">
                        <Label htmlFor="mem-data">Initial Data (optional)</Label>
                        <Input
                          id="mem-data"
                          placeholder="e.g., Hello from shared memory"
                          value={sharedMemory.data}
                          onChange={(e) => setSharedMemory({ ...sharedMemory, data: e.target.value })}
                        />
                      </div>
                    </div>
                    <DialogFooter>
                      <Button onClick={createSharedMem} disabled={loading || !sharedMemory.name}>
                        Create Segment
                      </Button>
                    </DialogFooter>
                  </DialogContent>
                </Dialog>

                <Dialog open={readMemDialog} onOpenChange={setReadMemDialog}>
                  <DialogTrigger asChild>
                    <Button variant="outline" className="border-green-500 text-green-600 hover:bg-green-500/10">
                      Read Memory
                    </Button>
                  </DialogTrigger>
                  <DialogContent>
                    <DialogHeader>
                      <DialogTitle>Read Shared Memory</DialogTitle>
                      <DialogDescription>
                        Read data from an existing shared memory segment
                      </DialogDescription>
                    </DialogHeader>
                    <div className="space-y-4 py-4">
                      <div className="space-y-2">
                        <Label htmlFor="read-mem-name">Memory Segment Name</Label>
                        <Input
                          id="read-mem-name"
                          placeholder="e.g., buffer_1"
                          value={memoryRead.name}
                          onChange={(e) => setMemoryRead({ ...memoryRead, name: e.target.value })}
                        />
                      </div>
                      {memoryRead.data && (
                        <div className="space-y-2">
                          <Label>Data</Label>
                          <div className="p-3 bg-muted rounded-md font-mono text-sm">
                            {memoryRead.data || '(empty)'}
                          </div>
                        </div>
                      )}
                    </div>
                    <DialogFooter>
                      <Button onClick={readSharedMem} disabled={loading || !memoryRead.name}>
                        Read Data
                      </Button>
                    </DialogFooter>
                  </DialogContent>
                </Dialog>

                <Dialog open={writeMemDialog} onOpenChange={setWriteMemDialog}>
                  <DialogTrigger asChild>
                    <Button variant="outline" className="border-purple-500 text-purple-600 hover:bg-purple-500/10">
                      Write Memory
                    </Button>
                  </DialogTrigger>
                  <DialogContent>
                    <DialogHeader>
                      <DialogTitle>Write to Shared Memory</DialogTitle>
                      <DialogDescription>
                        Write data to an existing shared memory segment
                      </DialogDescription>
                    </DialogHeader>
                    <div className="space-y-4 py-4">
                      <div className="space-y-2">
                        <Label htmlFor="write-mem-name">Memory Segment Name</Label>
                        <Input
                          id="write-mem-name"
                          placeholder="e.g., buffer_1"
                          value={memoryWrite.name}
                          onChange={(e) => setMemoryWrite({ ...memoryWrite, name: e.target.value })}
                        />
                      </div>
                      <div className="space-y-2">
                        <Label htmlFor="write-mem-data">Data to Write</Label>
                        <Input
                          id="write-mem-data"
                          placeholder="Enter data to write"
                          value={memoryWrite.data}
                          onChange={(e) => setMemoryWrite({ ...memoryWrite, data: e.target.value })}
                        />
                      </div>
                    </div>
                    <DialogFooter>
                      <Button onClick={writeSharedMem} disabled={loading || !memoryWrite.name}>
                        Write Data
                      </Button>
                    </DialogFooter>
                  </DialogContent>
                </Dialog>
              </div>
              
              {!ipcStats && (
                <Card className="bg-muted/30 border-dashed border-2">
                  <CardContent className="pt-6 text-center py-12">
                    <MessageSquare className="w-16 h-16 mx-auto text-muted-foreground/50 mb-4" />
                    <p className="text-lg font-semibold text-muted-foreground">No IPC Activity</p>
                    <p className="text-sm text-muted-foreground mt-2">
                      Send a test message to initialize IPC communication
                    </p>
                  </CardContent>
                </Card>
              )}

              {ipcStats && (
                <div className="grid grid-cols-2 gap-4">
                  <Card className="bg-gradient-to-br from-cyan-500/10 to-cyan-600/5">
                    <CardContent className="pt-6">
                      <div className="text-3xl font-bold">{ipcStats.messageQueues}</div>
                      <p className="text-xs text-muted-foreground mt-1">Message Queues</p>
                    </CardContent>
                  </Card>
                  <Card className="bg-gradient-to-br from-blue-500/10 to-blue-600/5">
                    <CardContent className="pt-6">
                      <div className="text-3xl font-bold">{ipcStats.sharedMemorySegments}</div>
                      <p className="text-xs text-muted-foreground mt-1">Shared Memory Segments</p>
                    </CardContent>
                  </Card>
                </div>
              )}
            </CardContent>
          </Card>
        </TabsContent>

        {/* Deadlock Tab */}
        <TabsContent value="deadlock" className="space-y-4">
          <Card className="border-2 shadow-lg">
            <CardHeader className="bg-gradient-to-r from-red-500/10 to-pink-500/10">
              <CardTitle className="flex items-center gap-2">
                <AlertTriangle className="w-6 h-6" />
                Deadlock Detection & Prevention
              </CardTitle>
              <CardDescription>
                Simulate deadlock scenarios and detection algorithms
              </CardDescription>
            </CardHeader>
            <CardContent className="space-y-6 pt-6">
              <div className="flex gap-2 flex-wrap">
                <Button 
                  onClick={simulateDeadlock} 
                  disabled={loading} 
                  variant="destructive"
                  className="bg-gradient-to-r from-red-500 to-red-600"
                >
                  <AlertTriangle className="w-4 h-4 mr-2" />
                  Simulate Deadlock Scenario
                </Button>
                <Button 
                  onClick={visualizeWaitForGraph} 
                  disabled={loading} 
                  variant="outline"
                  className="border-purple-500 text-purple-600 hover:bg-purple-500/10"
                >
                  <Activity className="w-4 h-4 mr-2" />
                  Visualize Wait-For Graph
                </Button>
                <Button 
                  onClick={recoverFromDeadlock} 
                  disabled={loading || !deadlockStats?.hasDeadlock} 
                  variant="outline"
                  className="border-green-500 text-green-600 hover:bg-green-500/10"
                >
                  <Play className="w-4 h-4 mr-2" />
                  Recover from Deadlock
                </Button>
              </div>
              
              {!deadlockStats && (
                <Card className="bg-muted/30 border-dashed border-2">
                  <CardContent className="pt-6 text-center py-12">
                    <AlertTriangle className="w-16 h-16 mx-auto text-muted-foreground/50 mb-4" />
                    <p className="text-lg font-semibold text-muted-foreground">Deadlock Detection Not Run</p>
                    <p className="text-sm text-muted-foreground mt-2">
                      Simulate a deadlock scenario to see detection results
                    </p>
                  </CardContent>
                </Card>
              )}

              {deadlockStats && (
                <div className="grid grid-cols-2 gap-4">
                  <Card className={`border-2 ${deadlockStats.hasDeadlock ? 'border-red-500 bg-red-500/5' : 'border-green-500 bg-green-500/5'}`}>
                    <CardContent className="pt-6">
                      <div className="flex items-center justify-between">
                        <div>
                          <div className={`text-3xl font-bold ${deadlockStats.hasDeadlock ? 'text-red-600' : 'text-green-600'}`}>
                            {deadlockStats.hasDeadlock ? 'Detected' : 'None'}
                          </div>
                          <p className="text-xs text-muted-foreground mt-1">Deadlock Status</p>
                        </div>
                        <AlertTriangle className={`w-8 h-8 ${deadlockStats.hasDeadlock ? 'text-red-500' : 'text-green-500'}`} />
                      </div>
                    </CardContent>
                  </Card>
                  <Card className={`border-2 ${deadlockStats.safeState ? 'border-green-500 bg-green-500/5' : 'border-orange-500 bg-orange-500/5'}`}>
                    <CardContent className="pt-6">
                      <div className="flex items-center justify-between">
                        <div>
                          <div className={`text-3xl font-bold ${deadlockStats.safeState ? 'text-green-600' : 'text-orange-600'}`}>
                            {deadlockStats.safeState ? 'Safe' : 'Unsafe'}
                          </div>
                          <p className="text-xs text-muted-foreground mt-1">System State</p>
                        </div>
                        <Activity className={`w-8 h-8 ${deadlockStats.safeState ? 'text-green-500' : 'text-orange-500'}`} />
                      </div>
                    </CardContent>
                  </Card>
                </div>
              )}

              {/* RAG and WFG Graphs */}
              {waitForGraph && waitForGraph.ragEdges && waitForGraph.resources && (
                <DeadlockGraphs 
                  ragEdges={waitForGraph.ragEdges}
                  wfgEdges={waitForGraph.waitForGraph || []}
                  processes={waitForGraph.processes || []}
                  resources={waitForGraph.resources || []}
                  hasDeadlock={waitForGraph.hasDeadlock || false}
                />
              )}

              {/* Wait-For Graph Visualization */}
              {waitForGraph && (
                <>
                  {/* Resource Allocation Chart */}
                  {waitForGraph.processes && waitForGraph.processes.length > 0 && (
                    <Card className="border-blue-500/50 bg-blue-500/5">
                      <CardHeader>
                        <CardTitle className="text-lg flex items-center gap-2">
                          <TrendingUp className="w-5 h-5 text-blue-600" />
                          Resource Allocation Analysis
                        </CardTitle>
                        <CardDescription>
                          Visual representation of allocated and needed resources per process
                        </CardDescription>
                      </CardHeader>
                      <CardContent>
                        <ResponsiveContainer width="100%" height={300}>
                          <BarChart
                            data={waitForGraph.processes.map((proc: any) => ({
                              name: proc.name,
                              allocated: proc.allocated?.reduce((sum: number, r: any) => sum + r.amount, 0) || 0,
                              needed: proc.needed?.reduce((sum: number, r: any) => sum + r.amount, 0) || 0,
                            }))}
                            margin={{ top: 20, right: 30, left: 20, bottom: 5 }}
                          >
                            <CartesianGrid strokeDasharray="3 3" />
                            <XAxis dataKey="name" />
                            <YAxis />
                            <Tooltip />
                            <Legend />
                            <Bar dataKey="allocated" fill="#3b82f6" name="Allocated Resources" />
                            <Bar dataKey="needed" fill="#f97316" name="Needed Resources" />
                          </BarChart>
                        </ResponsiveContainer>
                        
                        <div className="mt-4 pt-4 border-t">
                          <h4 className="font-semibold text-sm mb-3">Detailed Resource Allocation</h4>
                          <div className="space-y-2">
                            {waitForGraph.processes.map((proc: any) => (
                              <div key={proc.id} className="text-sm">
                                <span className="font-medium">{proc.name}:</span>
                                {proc.allocated && proc.allocated.length > 0 && (
                                  <span className="text-blue-600 ml-2">
                                    Holds: {proc.allocated.map((r: any) => `${r.name}(${r.amount})`).join(', ')}
                                  </span>
                                )}
                                {proc.needed && proc.needed.length > 0 && (
                                  <span className="text-orange-600 ml-2">
                                    Needs: {proc.needed.map((r: any) => `${r.name}(${r.amount})`).join(', ')}
                                  </span>
                                )}
                              </div>
                            ))}
                          </div>
                        </div>
                      </CardContent>
                    </Card>
                  )}

                  {/* Wait-For Graph Dependency Visualization */}
                  <Card className="border-purple-500/50 bg-purple-500/5">
                    <CardHeader>
                      <CardTitle className="text-lg flex items-center gap-2">
                        <Activity className="w-5 h-5 text-purple-600" />
                        Wait-For Graph Dependencies
                      </CardTitle>
                      <CardDescription>
                        Process waiting relationships and circular dependency detection
                      </CardDescription>
                    </CardHeader>
                    <CardContent>
                      {waitForGraph.waitForGraph && waitForGraph.waitForGraph.length > 0 ? (
                        <>
                          {/* Dependency Flow Diagram */}
                          <div className="space-y-3 mb-6">
                            {waitForGraph.waitForGraph.map((edge: any, idx: number) => (
                              <div 
                                key={edge.processId} 
                                className={`p-4 border-2 rounded-lg transition-all ${
                                  edge.waitingFor && edge.waitingFor.length > 0 
                                    ? 'border-orange-500 bg-orange-50 dark:bg-orange-950/20' 
                                    : 'border-green-500 bg-green-50 dark:bg-green-950/20'
                                }`}
                              >
                                <div className="flex items-center justify-between mb-2">
                                  <div className="font-semibold text-sm flex items-center gap-2">
                                    <div className={`w-8 h-8 rounded-full flex items-center justify-center text-white font-bold ${
                                      edge.waitingFor && edge.waitingFor.length > 0 ? 'bg-orange-500' : 'bg-green-500'
                                    }`}>
                                      P{edge.processId}
                                    </div>
                                    {edge.processName}
                                  </div>
                                  <Badge variant={edge.waitingFor && edge.waitingFor.length > 0 ? 'destructive' : 'default'}>
                                    {edge.waitingFor && edge.waitingFor.length > 0 ? 'Waiting' : 'Active'}
                                  </Badge>
                                </div>
                                {edge.waitingFor && edge.waitingFor.length > 0 ? (
                                  <div className="ml-10 space-y-2">
                                    <div className="text-sm text-muted-foreground flex items-center gap-2">
                                      <div className="w-px h-6 bg-orange-400"></div>
                                      <span className="font-medium">Waiting for:</span>
                                    </div>
                                    {edge.waitingFor.map((w: any, wIdx: number) => (
                                      <div key={wIdx} className="ml-2 flex items-center gap-2 text-sm">
                                        <div className="w-6 h-6 rounded-full flex items-center justify-center bg-orange-200 dark:bg-orange-900 text-xs font-bold">
                                          P{w.processId}
                                        </div>
                                        <span>{w.processName}</span>
                                      </div>
                                    ))}
                                  </div>
                                ) : (
                                  <div className="ml-10 text-sm text-green-600 flex items-center gap-2">
                                    <div className="w-px h-6 bg-green-400"></div>
                                    ✓ Not waiting (Ready to execute)
                                  </div>
                                )}
                              </div>
                            ))}
                          </div>

                          {/* Deadlock Cycle Detection Alert */}
                          {waitForGraph.hasDeadlock && (
                            <div className="p-4 border-2 border-red-500 bg-red-50 dark:bg-red-950/20 rounded-lg">
                              <div className="flex items-center gap-2 text-red-600 font-semibold mb-2">
                                <AlertTriangle className="w-5 h-5" />
                                Circular Dependency Detected (Deadlock!)
                              </div>
                              <p className="text-sm text-muted-foreground">
                                The wait-for graph contains a cycle, indicating processes are waiting in a circular chain. 
                                This creates a deadlock situation that requires intervention.
                              </p>
                            </div>
                          )}
                        </>
                      ) : (
                        <div className="text-center py-8">
                          <div className="w-16 h-16 mx-auto mb-4 rounded-full bg-green-100 dark:bg-green-950/30 flex items-center justify-center">
                            <Activity className="w-8 h-8 text-green-600" />
                          </div>
                          <p className="text-lg font-semibold text-green-600">No circular dependencies detected</p>
                          <p className="text-sm text-muted-foreground mt-2">
                            The system is in a safe state with no deadlock cycles
                          </p>
                        </div>
                      )}
                    </CardContent>
                  </Card>
                </>
              )}
            </CardContent>
          </Card>
        </TabsContent>
      </Tabs>

      {/* Edit Process Dialog */}
      <Dialog open={editDialogOpen} onOpenChange={setEditDialogOpen}>
        <DialogContent>
          <DialogHeader>
            <DialogTitle>Edit Process</DialogTitle>
            <DialogDescription>
              Modify the process details below
            </DialogDescription>
          </DialogHeader>
          {selectedProcess && (
            <div className="grid gap-4 py-4">
              <div className="grid grid-cols-4 items-center gap-4">
                <Label htmlFor="edit-processName" className="text-right">Name</Label>
                <Input
                  id="edit-processName"
                  value={selectedProcess.processName}
                  onChange={(e) => setSelectedProcess({...selectedProcess, processName: e.target.value})}
                  className="col-span-3"
                />
              </div>
              <div className="grid grid-cols-4 items-center gap-4">
                <Label htmlFor="edit-arrivalTime" className="text-right">Arrival Time</Label>
                <Input
                  id="edit-arrivalTime"
                  type="number"
                  value={selectedProcess.arrivalTime}
                  onChange={(e) => setSelectedProcess({...selectedProcess, arrivalTime: parseInt(e.target.value) || 0})}
                  className="col-span-3"
                />
              </div>
              <div className="grid grid-cols-4 items-center gap-4">
                <Label htmlFor="edit-burstTime" className="text-right">Burst Time</Label>
                <Input
                  id="edit-burstTime"
                  type="number"
                  value={selectedProcess.burstTime}
                  onChange={(e) => setSelectedProcess({...selectedProcess, burstTime: parseInt(e.target.value) || 1})}
                  className="col-span-3"
                />
              </div>
              <div className="grid grid-cols-4 items-center gap-4">
                <Label htmlFor="edit-priority" className="text-right">Priority</Label>
                <Input
                  id="edit-priority"
                  type="number"
                  value={selectedProcess.priority}
                  onChange={(e) => setSelectedProcess({...selectedProcess, priority: parseInt(e.target.value) || 1})}
                  className="col-span-3"
                />
              </div>
            </div>
          )}
          <DialogFooter>
            <Button onClick={editProcess} disabled={loading}>Update Process</Button>
          </DialogFooter>
        </DialogContent>
      </Dialog>

      {/* Delete Process Confirmation */}
      <AlertDialog open={deleteDialogOpen} onOpenChange={setDeleteDialogOpen}>
        <AlertDialogContent>
          <AlertDialogHeader>
            <AlertDialogTitle>Are you sure?</AlertDialogTitle>
            <AlertDialogDescription>
              {selectedProcess && `This will permanently delete ${selectedProcess.processName} (P${selectedProcess.pid}). This action cannot be undone.`}
            </AlertDialogDescription>
          </AlertDialogHeader>
          <AlertDialogFooter>
            <AlertDialogCancel>Cancel</AlertDialogCancel>
            <AlertDialogAction onClick={deleteProcess} disabled={loading} className="bg-destructive text-destructive-foreground">
              Delete
            </AlertDialogAction>
          </AlertDialogFooter>
        </AlertDialogContent>
      </AlertDialog>
    </div>
  );
}
