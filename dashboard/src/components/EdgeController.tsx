import React from 'react';

const EdgeController: React.FC = () => {
  return (
    <div className="bg-[#121a19] border border-[#1e2d2b] rounded-lg p-4 flex flex-col gap-4 shadow-[0_0_15px_rgba(0,0,0,0.3)]">
      <div className="flex justify-between items-center border-b border-[#1e2d2b] pb-2">
        <h3 className="text-[#00ffcc] text-xs font-bold uppercase tracking-widest">Edge Node Control</h3>
        <span className="text-[9px] text-[#4db6ac] font-mono">NODE_77_ALPHA</span>
      </div>

      <div className="grid grid-cols-2 gap-2">
        <button className="flex items-center justify-center gap-2 bg-[#1e2d2b] hover:bg-[#2c4240] text-[#00ffcc] py-2 px-3 rounded text-[10px] font-bold transition-all uppercase border border-[#2c4240]">
          <span className="w-1.5 h-1.5 bg-[#00ffcc] rounded-full"></span>
          Reset Engine
        </button>
        <button className="flex items-center justify-center gap-2 bg-[#1e2d2b] hover:bg-[#2c4240] text-[#ff3366] py-2 px-3 rounded text-[10px] font-bold transition-all uppercase border border-[#2c4240]">
           <span className="w-1.5 h-1.5 bg-[#ff3366] rounded-full"></span>
           Purge Cache
        </button>
      </div>

      <div className="flex flex-col gap-2 bg-[#0c1211] p-3 rounded border border-[#1e2d2b]">
        <div className="flex justify-between items-center">
            <span className="text-[9px] text-[#4db6ac] uppercase">SIMD Filtering</span>
            <div className="w-8 h-4 bg-[#00ffcc] rounded-full relative cursor-pointer">
              <div className="w-3 h-3 bg-white rounded-full absolute top-0.5 right-0.5"/>
            </div>
        </div>
        <div className="flex justify-between items-center">
            <span className="text-[9px] text-[#4db6ac] uppercase">Auto-Reclamation</span>
            <div className="w-8 h-4 bg-[#00ffcc] rounded-full relative cursor-pointer">
              <div className="w-3 h-3 bg-white rounded-full absolute top-0.5 right-0.5"/>
            </div>
        </div>
        <div className="flex justify-between items-center">
            <span className="text-[9px] text-[#4db6ac] uppercase">Heartbeat Sync</span>
            <div className="w-8 h-4 bg-[#1e2d2b] rounded-full relative cursor-pointer">
              <div className="w-3 h-3 bg-white rounded-full absolute top-0.5 left-0.5"/>
            </div>
        </div>
      </div>

      <div className="text-[8px] text-[#4db6ac] font-mono flex items-center justify-between opacity-50">
        <span>FW_VER: 2.1.0_LTS</span>
        <span>UPTIME: 14D_02H_55M</span>
      </div>
    </div>
  );
};

export default EdgeController;
