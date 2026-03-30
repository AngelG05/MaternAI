import React from 'react';

const RiskGrid: React.FC = () => {
  // Simulated patient data for the grid
  const patients = [
    { id: 101, status: 'NOMINAL', hr: 72, fhr: 142, bp: '120/80', risk: 0.12 },
    { id: 102, status: 'STABLE', hr: 68, fhr: 138, bp: '115/75', risk: 0.08 },
    { id: 103, status: 'URGENT', hr: 95, fhr: 165, bp: '145/95', risk: 0.65 },
    { id: 104, status: 'NOMINAL', hr: 80, fhr: 145, bp: '122/82', risk: 0.15 },
    { id: 105, status: 'STABLE', hr: 74, fhr: 140, bp: '118/78', risk: 0.10 },
    { id: 106, status: 'NOMINAL', hr: 70, fhr: 135, bp: '110/70', risk: 0.05 },
    { id: 107, status: 'OBSERVE', hr: 88, fhr: 155, bp: '135/88', risk: 0.32 },
    { id: 108, status: 'STABLE', hr: 76, fhr: 148, bp: '125/85', risk: 0.18 },
  ];

  return (
    <div className="bg-[#121a19] border border-[#1e2d2b] rounded-lg h-full flex flex-col p-4 shadow-[0_0_10px_rgba(0,0,0,0.2)]">
      <div className="mb-4 flex justify-between items-baseline border-b border-[#1e2d2b] pb-2">
        <h3 className="text-[#00ffcc] text-xs font-bold uppercase tracking-tight">Systematic Risk Surface</h3>
        <span className="text-[10px] text-[#4db6ac] font-mono">ALL_NODES (8/8)</span>
      </div>
      
      <div className="grid grid-cols-4 gap-3 flex-1">
        {patients.map((p) => (
          <div 
            key={p.id} 
            className={`p-3 rounded border bg-[#0c1211] hover:bg-[#151f1e] transition-colors flex flex-col justify-between
              ${p.status === 'URGENT' ? 'border-[#ff3366] shadow-[0_0_10px_rgba(255,51,102,0.1)]' : 'border-[#1e2d2b]'}`}
          >
            <div className="flex justify-between items-start">
              <span className="text-[10px] font-bold text-[#e0f2f1]">P-{p.id}</span>
              <span className={`text-[8px] font-bold px-1 rounded ${
                p.status === 'URGENT' ? 'bg-[#ff3366] text-white' : 
                p.status === 'OBSERVE' ? 'bg-[#ffa726] text-[#000]' : 'text-[#4db6ac]'
              }`}>
                {p.status}
              </span>
            </div>
            
            <div className="mt-2 grid grid-cols-2 gap-1 text-[10px]">
              <div className="text-[#4db6ac]">FHR: <span className="text-[#00ffcc]">{p.fhr}</span></div>
              <div className="text-[#4db6ac]">BP: <span className="text-[#e0f2f1]">{p.bp}</span></div>
            </div>

            <div className="mt-2 h-1 bg-[#1e2d2b] rounded-full overflow-hidden">
               <div 
                 className={`h-full ${p.risk > 0.5 ? 'bg-[#ff3366]' : 'bg-[#00ffcc]'}`} 
                 style={{ width: `${p.risk * 100}%` }}
               />
            </div>
          </div>
        ))}
      </div>
    </div>
  );
};

export default RiskGrid;
