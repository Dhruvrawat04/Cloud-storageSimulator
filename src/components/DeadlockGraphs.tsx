import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "./ui/card";
import { Table, TableBody, TableCell, TableHead, TableHeader, TableRow } from "./ui/table";
import { Badge } from "./ui/badge";
import { Circle } from "lucide-react";
import { useMemo } from "react";
import ReactFlow, {
  Node,
  Edge,
  Background,
  Controls,
  MiniMap,
  Position,
  MarkerType,
  Handle,
} from "reactflow";
import "reactflow/dist/style.css";

interface RAGEdge {
  type: string;
  from: { id: number; type: string; name: string };
  to: { id: number; type: string; name: string };
  units: number;
}

interface Resource {
  id: number;
  name: string;
  total: number;
  available: number;
}

interface Process {
  id: number;
  name: string;
}

interface WFGEdge {
  processId: number;
  processName: string;
  waitingFor: Array<{ processId: number; processName: string }>;
}

interface DeadlockGraphsProps {
  ragEdges: RAGEdge[];
  wfgEdges: WFGEdge[];
  processes: Process[];
  resources: Resource[];
  hasDeadlock: boolean;
}

// Custom Process Node (Circle)
const ProcessNode = ({ data }: any) => {
  return (
    <div className="flex flex-col items-center">
      <div
        className="w-20 h-20 rounded-full bg-green-100 border-4 border-green-600 flex items-center justify-center shadow-lg"
        style={{ borderColor: data.hasDeadlock ? "#ef4444" : "#16a34a" }}
      >
        <span className="font-bold text-green-800 text-lg">{data.label}</span>
      </div>
      <Handle type="target" position={Position.Bottom} />
      <Handle type="source" position={Position.Bottom} />
    </div>
  );
};

// Custom Resource Node (Rectangle)
const ResourceNode = ({ data }: any) => {
  return (
    <div className="flex flex-col items-center">
      <Handle type="source" position={Position.Top} />
      <Handle type="target" position={Position.Top} />
      <div
        className="w-24 h-16 bg-blue-100 border-4 border-blue-600 flex items-center justify-center shadow-lg"
        style={{ borderColor: data.hasDeadlock ? "#ef4444" : "#2563eb" }}
      >
        <span className="font-bold text-blue-800 text-sm text-center">{data.label}</span>
      </div>
    </div>
  );
};

const nodeTypes = {
  processNode: ProcessNode,
  resourceNode: ResourceNode,
};

export function DeadlockGraphs({ ragEdges = [], wfgEdges = [], processes = [], resources = [], hasDeadlock = false }: DeadlockGraphsProps) {
  // Add defensive defaults
  const safeRagEdges = ragEdges || [];
  const safeWfgEdges = wfgEdges || [];
  const safeProcesses = processes || [];
  const safeResources = resources || [];
  
  // Group RAG edges by type
  const holdEdges = safeRagEdges.filter(e => e.type === "hold");
  const requestEdges = safeRagEdges.filter(e => e.type === "request");

  // Build React Flow nodes and edges
  const { nodes, edges } = useMemo(() => {
    const flowNodes: Node[] = [];
    const flowEdges: Edge[] = [];

    // Layout configuration
    const processY = 80;
    const resourceY = 280;
    const spacing = 200;
    const startX = 100;

    // Create process nodes (circles) - top row
    safeProcesses.forEach((proc, idx) => {
      flowNodes.push({
        id: `process-${proc.id}`,
        type: "processNode",
        position: { x: startX + idx * spacing, y: processY },
        data: { label: proc.name, hasDeadlock },
        sourcePosition: Position.Bottom,
        targetPosition: Position.Top,
      });
    });

    // Create resource nodes (rectangles) - bottom row
    safeResources.forEach((res, idx) => {
      flowNodes.push({
        id: `resource-${res.id}`,
        type: "resourceNode",
        position: { x: startX + idx * spacing, y: resourceY },
        data: { label: res.name, hasDeadlock },
        sourcePosition: Position.Top,
        targetPosition: Position.Bottom,
      });
    });

    // Create hold edges (Resource → Process, meaning Process holds Resource) - purple
    holdEdges.forEach((edge, idx) => {
      const fromId = `${edge.from.type}-${edge.from.id}`;
      const toId = `${edge.to.type}-${edge.to.id}`;
      
      flowEdges.push({
        id: `hold-${idx}`,
        source: fromId,
        target: toId,
        type: "smoothstep",
        animated: false,
        style: { stroke: "#9333ea", strokeWidth: 3 },
        markerEnd: {
          type: MarkerType.ArrowClosed,
          color: "#9333ea",
          width: 20,
          height: 20,
        },
      });
    });

    // Create request edges (Process → Resource) - orange
    requestEdges.forEach((edge, idx) => {
      const fromId = `${edge.from.type}-${edge.from.id}`;
      const toId = `${edge.to.type}-${edge.to.id}`;
      
      flowEdges.push({
        id: `request-${idx}`,
        source: fromId,
        target: toId,
        type: "smoothstep",
        animated: true,
        style: { stroke: "#ea580c", strokeWidth: 3 },
        markerEnd: {
          type: MarkerType.ArrowClosed,
          color: "#ea580c",
          width: 20,
          height: 20,
        },
      });
    });

    return { nodes: flowNodes, edges: flowEdges };
  }, [safeProcesses, safeResources, holdEdges, requestEdges, hasDeadlock]);

  // Build WFG React Flow nodes and edges
  const { nodes: wfgNodes, edges: wfgFlowEdges } = useMemo(() => {
    const flowNodes: Node[] = [];
    const flowEdges: Edge[] = [];

    if (safeProcesses.length === 0) {
      return { nodes: flowNodes, edges: flowEdges };
    }

    // Layout configuration - circular layout for WFG
    const centerX = 400;
    const centerY = 250;
    const radius = 150;
    const angleStep = (2 * Math.PI) / safeProcesses.length;

    // Create process nodes in a circle
    safeProcesses.forEach((proc, idx) => {
      const angle = idx * angleStep - Math.PI / 2;
      const x = centerX + radius * Math.cos(angle);
      const y = centerY + radius * Math.sin(angle);

      flowNodes.push({
        id: `wfg-process-${proc.id}`,
        type: "processNode",
        position: { x, y },
        data: { label: proc.name, hasDeadlock },
        sourcePosition: Position.Bottom,
        targetPosition: Position.Top,
      });
    });

    // Create wait-for edges (Pi → Pj means Pi is waiting for Pj)
    safeWfgEdges.forEach((wfgEdge, idx) => {
      wfgEdge.waitingFor.forEach((waitingProc, wIdx) => {
        flowEdges.push({
          id: `wfg-edge-${idx}-${wIdx}`,
          source: `wfg-process-${wfgEdge.processId}`,
          target: `wfg-process-${waitingProc.processId}`,
          type: "smoothstep",
          animated: true,
          style: { stroke: "#dc2626", strokeWidth: 3 },
          markerEnd: {
            type: MarkerType.ArrowClosed,
            color: "#dc2626",
            width: 20,
            height: 20,
          },
        });
      });
    });

    return { nodes: flowNodes, edges: flowEdges };
  }, [safeProcesses, safeWfgEdges, hasDeadlock]);

  return (
    <div className="grid gap-6">
      {/* Resource Allocation Graph */}
      <Card className={`border-2 ${hasDeadlock ? 'border-red-500' : 'border-blue-500'}`}>
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <Circle className="w-5 h-5" />
            Resource Allocation Graph (RAG)
          </CardTitle>
          <CardDescription>
            Visual representation showing which processes hold which resources and which resources they're waiting for
          </CardDescription>
        </CardHeader>
        <CardContent className="space-y-4">
          {/* Legend */}
          <div className="p-3 bg-gray-50 rounded-lg">
            <p className="text-sm font-medium mb-2">📊 Graph Legend:</p>
            <div className="grid grid-cols-2 md:grid-cols-4 gap-3 text-sm">
              <div className="flex items-center gap-2">
                <div className="w-8 h-8 rounded-full bg-green-100 border-2 border-green-600"></div>
                <span>Process (Circle)</span>
              </div>
              <div className="flex items-center gap-2">
                <div className="w-8 h-6 bg-blue-100 border-2 border-blue-600"></div>
                <span>Resource (Rectangle)</span>
              </div>
              <div className="flex items-center gap-2">
                <div className="w-8 h-0.5 bg-purple-600"></div>
                <span className="text-purple-600">Holding</span>
              </div>
              <div className="flex items-center gap-2">
                <div className="w-8 h-0.5 bg-orange-600"></div>
                <span className="text-orange-600">Waiting</span>
              </div>
            </div>
          </div>

          {/* React Flow Graph */}
          <div className="w-full h-[500px] bg-white border-2 border-gray-200 rounded-lg">
            {nodes.length > 0 ? (
              <ReactFlow
                nodes={nodes}
                edges={edges}
                nodeTypes={nodeTypes}
                fitView
                attributionPosition="bottom-left"
                minZoom={0.5}
                maxZoom={2}
                defaultViewport={{ x: 0, y: 0, zoom: 1 }}
              >
                <Background />
                <Controls />
                <MiniMap 
                  nodeColor={(node) => {
                    if (node.type === 'processNode') return '#16a34a';
                    if (node.type === 'resourceNode') return '#2563eb';
                    return '#94a3b8';
                  }}
                  maskColor="rgba(0, 0, 0, 0.1)"
                />
              </ReactFlow>
            ) : (
              <div className="flex items-center justify-center h-full text-gray-500">
                No processes or resources to display
              </div>
            )}
          </div>

          {/* Status Summary */}
          {hasDeadlock && (
            <div className="p-4 bg-red-50 border-2 border-red-300 rounded-lg">
              <p className="text-red-700 font-semibold flex items-center gap-2">
                <Circle className="w-5 h-5 fill-red-500" />
                DEADLOCK DETECTED: Circular wait condition exists in the system!
              </p>
            </div>
          )}
          {!hasDeadlock && (holdEdges.length > 0 || requestEdges.length > 0) && (
            <div className="p-4 bg-green-50 border-2 border-green-300 rounded-lg">
              <p className="text-green-700 font-semibold flex items-center gap-2">
                <Circle className="w-5 h-5 fill-green-500" />
                NO DEADLOCK: System is in a safe state
              </p>
            </div>
          )}
        </CardContent>
      </Card>

      {/* Wait-For Graph */}
      <Card className={`border-2 ${hasDeadlock ? 'border-red-500' : 'border-green-500'}`}>
        <CardHeader>
          <CardTitle className="flex items-center gap-2">
            <Circle className="w-5 h-5" />
            Wait-For Graph (WFG)
          </CardTitle>
          <CardDescription>
            Simplified graph showing only processes. Edge from Pi to Pj means Pi is waiting for a resource held by Pj (cycle = deadlock)
          </CardDescription>
        </CardHeader>
        <CardContent className="space-y-4">
          {/* Legend */}
          <div className="p-3 bg-gray-50 rounded-lg">
            <p className="text-sm font-medium mb-2">📊 Graph Legend:</p>
            <div className="grid grid-cols-2 gap-3 text-sm">
              <div className="flex items-center gap-2">
                <div className="w-8 h-8 rounded-full bg-green-100 border-2 border-green-600"></div>
                <span>Process (Circle)</span>
              </div>
              <div className="flex items-center gap-2">
                <div className="w-8 h-0.5 bg-red-600"></div>
                <span className="text-red-600">Waiting For</span>
              </div>
            </div>
          </div>

          {/* React Flow Graph */}
          <div className="w-full h-[500px] bg-white border-2 border-gray-200 rounded-lg">
            {wfgNodes.length > 0 ? (
              <ReactFlow
                nodes={wfgNodes}
                edges={wfgFlowEdges}
                nodeTypes={nodeTypes}
                fitView
                attributionPosition="bottom-left"
                minZoom={0.5}
                maxZoom={2}
                defaultViewport={{ x: 0, y: 0, zoom: 1 }}
              >
                <Background />
                <Controls />
                <MiniMap 
                  nodeColor={(node) => '#16a34a'}
                  maskColor="rgba(0, 0, 0, 0.1)"
                />
              </ReactFlow>
            ) : (
              <div className="flex items-center justify-center h-full text-gray-500">
                No processes to display
              </div>
            )}
          </div>

          {/* Status Summary */}
          {hasDeadlock && (
            <div className="p-4 bg-red-50 border-2 border-red-300 rounded-lg">
              <p className="text-red-700 font-semibold flex items-center gap-2">
                <Circle className="w-5 h-5 fill-red-500" />
                DEADLOCK DETECTED: Circular wait dependency exists in the Wait-For Graph!
              </p>
            </div>
          )}
          {!hasDeadlock && wfgFlowEdges.length > 0 && (
            <div className="p-4 bg-green-50 border-2 border-green-300 rounded-lg">
              <p className="text-green-700 font-semibold flex items-center gap-2">
                <Circle className="w-5 h-5 fill-green-500" />
                NO DEADLOCK: No circular wait condition detected
              </p>
            </div>
          )}
        </CardContent>
      </Card>
    </div>
  );
}
