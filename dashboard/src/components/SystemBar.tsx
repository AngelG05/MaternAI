import React from 'react';

interface Props {
  isConnected: boolean;
}

const SystemBar: React.FC<Props> = ({ isConnected }) => {
  return (
    <header className="h-[40px] bg-[#0c1211] border-b border-[#1e2d2b] flex items-center justify-between px-4">
      {/* Branding */}
      <div className="flex items-center gap-3">
        <div className="w-5 h-5 bg-[#00ffcc] flex items-center justify-center rounded-sm">
          <span className="text-[#000] font-black text-xs italic">M</span>
        </div>
        <h1 className="text-[#00ffcc] text-xs font-black uppercase tracking-tighter italic">
          MaternAI <span className="text-[#4db6ac] not-italic font-mono ml-1 opacity-50">EDGE_CONTROL_OS v1.0.0</span>
        </h1>
      </div>

      {/* Center Logic: Clock/Status */}
      <div className="flex items-center gap-6">
        <div className="flex items-center gap-2">
          <span className="text-[10px] text-[#4db6ac] font-bold uppercase tracking-widest">Master Node:</span>
          <span className="text-[10px] text-[#e0f2f1] font-mono">B-ALPHA-99</span>
        </div>
        <div className="flex items-center gap-2">
          <span className="text-[10px] text-[#4db6ac] font-bold uppercase tracking-widest">Protocol:</span>
          <span className="text-[10px] text-[#00ffcc] font-mono">io_uring/UDP</span>
        </div>
      </div>

      {/* Connection Status */}
      <div className="flex items-center gap-4">
        <div className="flex items-center gap-2">
          <span className="text-[9px] text-[#4db6ac] font-bold uppercase tracking-widest">Signal:</span>
          <div className="flex gap-0.5">
            {[1, 2, 3, 4].map(i => (
              <div key={i} className={`w-1 h-3 rounded-full ${i <= 3 ? 'bg-[#00ffcc]' : 'bg-[#1e2d2b]'}`} />
            ))}
          </div>
        </div>

        <div className="flex items-center gap-2 px-3 py-1 bg-[#121a19] border border-[#1e2d2b] rounded-md">
           <div className={`w-2 h-2 rounded-full ${isConnected ? 'bg-[#00ffcc] animate-pulse shadow-[0_0_8px_#00ffcc]' : 'bg-[#ff3366]'}`} />
           <span className="text-[9px] font-black uppercase text-[#e0f2f1] tracking-widest">
             {isConnected ? 'Sync Active' : 'Link Lost'}
           </span>
        </div>
      </div>
    </header>
  );
};

export default SystemBar;
