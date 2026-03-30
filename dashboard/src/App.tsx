import React, { useState, useEffect, useRef } from 'react';
import './App.css';
import VitalsLog from './components/VitalsLog';
import RiskGrid from './components/RiskGrid';
import VitalsWaveform from './components/VitalsWaveform';
import TelemetryCluster from './components/TelemetryCluster';
import EdgeController from './components/EdgeController';
import SystemBar from './components/SystemBar';

function App() {
  const [metrics, setMetrics] = useState({
    throughput: 0,
    latency: 0,
    alerts: 0,
    active_monitors: 1
  });
  
  const [vitals, setVitals] = useState<any[]>([]);
  const [isConnected, setIsConnected] = useState(false);
  
  useEffect(() => {
    const eventSource = new EventSource('http://localhost:18080/api/v1/stream');
    
    eventSource.onopen = () => setIsConnected(true);
    eventSource.onmessage = (event) => {
      const data = JSON.parse(event.data);
      setMetrics(prev => ({
        ...prev,
        throughput: data.metrics.throughput,
        latency: data.metrics.latency,
        alerts: data.alerts
      }));
      
      if (data.heartbeat) {
        setVitals(prev => [data.heartbeat, ...prev.slice(0, 19)]);
      }
    };
    
    eventSource.onerror = () => {
      setIsConnected(false);
      eventSource.close();
    };
    
    return () => eventSource.close();
  }, []);

  return (
    <div className="min-h-screen bg-[#0a0f0e] text-[#e0f2f1] font-mono selection:bg-[#00ffcc] selection:text-[#000]">
      <SystemBar isConnected={isConnected} />
      
      <main className="p-4 grid grid-cols-12 gap-4 h-[calc(100vh-40px)]">
        {/* Left Column: Waveforms & Logic */}
        <div className="col-span-8 flex flex-col gap-4 overflow-hidden">
          <div className="flex-1 bg-[#121a19] border border-[#1e2d2b] rounded-lg p-4 shadow-[0_0_20px_rgba(0,255,204,0.05)]">
            <h2 className="text-[#00ffcc] text-xs font-bold uppercase mb-4 flex items-center gap-2">
              <span className="w-2 h-2 bg-[#00ffcc] rounded-full animate-pulse"></span>
              Live Channel: Alpha (Fetal Heart Rate)
            </h2>
            <VitalsWaveform color="#00ffcc" />
          </div>
          
          <div className="flex-1 bg-[#121a19] border border-[#1e2d2b] rounded-lg p-4 shadow-[0_0_20px_rgba(255,51,102,0.05)]">
             <h2 className="text-[#ff3366] text-xs font-bold uppercase mb-4 flex items-center gap-2">
              <span className="w-2 h-2 bg-[#ff3366] rounded-full"></span>
              Live Channel: Beta (Maternal SPO2)
            </h2>
            <VitalsWaveform color="#ff3366" />
          </div>

          <div className="h-1/3 overflow-hidden">
            <RiskGrid />
          </div>
        </div>

        {/* Right Column: Telemetry & Logs */}
        <div className="col-span-4 flex flex-col gap-4 overflow-hidden">
          <TelemetryCluster metrics={metrics} />
          
          <div className="flex-1 overflow-hidden">
             <VitalsLog vitals={vitals} />
          </div>
          
          <EdgeController />
        </div>
      </main>
    </div>
  );
}

export default App;
