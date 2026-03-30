import React from 'react';

interface Props {
  metrics: {
    throughput: number;
    latency: number;
    alerts: number;
    active_monitors: number;
  };
}

const TelemetryCluster: React.FC<Props> = ({ metrics }) => {
  return (
    <div className="grid grid-cols-2 gap-3">
      {/* Throughput */}
      <div className="bg-[#121a19] border border-[#1e2d2b] p-3 rounded-lg flex flex-col justify-between">
        <span className="text-[10px] text-[#4db6ac] uppercase tracking-wider font-bold">Throughput</span>
        <div className="flex items-baseline gap-1 mt-1">
          <span className="text-xl font-bold text-[#00ffcc]">{metrics.throughput.toLocaleString()}</span>
          <span className="text-[9px] text-[#4db6ac]">EPS</span>
        </div>
      </div>

      {/* Latency */}
      <div className="bg-[#121a19] border border-[#1e2d2b] p-3 rounded-lg flex flex-col justify-between">
        <span className="text-[10px] text-[#4db6ac] uppercase tracking-wider font-bold">P99 Latency</span>
        <div className="flex items-baseline gap-1 mt-1">
          <span className="text-xl font-bold text-[#00ffcc]">{metrics.latency.toFixed(2)}</span>
          <span className="text-[9px] text-[#4db6ac]">μs</span>
        </div>
      </div>

      {/* Critical Alerts */}
      <div className={`bg-[#121a19] border ${metrics.alerts > 0 ? 'border-[#ff3366]' : 'border-[#1e2d2b]'} p-3 rounded-lg flex flex-col justify-between transition-colors duration-300`}>
        <span className="text-[10px] text-[#ff3366] uppercase tracking-wider font-bold">Critical Alerts</span>
        <div className="flex items-baseline gap-1 mt-1">
          <span className={`text-xl font-bold ${metrics.alerts > 0 ? 'text-[#ff3366] animate-pulse' : 'text-[#e0f2f1]'}`}>
            {metrics.alerts}
          </span>
          <span className="text-[9px] text-[#4db6ac]">TRG</span>
        </div>
      </div>

      {/* Active Monitors */}
      <div className="bg-[#121a19] border border-[#1e2d2b] p-3 rounded-lg flex flex-col justify-between">
        <span className="text-[10px] text-[#4db6ac] uppercase tracking-wider font-bold">Nodes</span>
        <div className="flex items-baseline gap-1 mt-1">
          <span className="text-xl font-bold text-[#00ffcc]">{metrics.active_monitors}</span>
          <span className="text-[9px] text-[#4db6ac]">ACTIVE</span>
        </div>
      </div>
    </div>
  );
};

export default TelemetryCluster;
