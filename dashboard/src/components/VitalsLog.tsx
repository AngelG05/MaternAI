import React from 'react';

interface VitalsEntry {
  patient_id: number;
  rate: number;
  fhr: number;
}

interface Props {
  vitals: VitalsEntry[];
}

const VitalsLog: React.FC<Props> = ({ vitals }) => {
  return (
    <div className="bg-[#121a19] border border-[#1e2d2b] rounded-lg h-full flex flex-col">
      <div className="p-2 border-b border-[#1e2d2b] bg-[#0c1211] flex justify-between items-baseline">
        <span className="text-[10px] text-[#4db6ac] font-bold uppercase tracking-wider">Vitals Log</span>
        <span className="text-[8px] text-[#1e2d2b] font-mono">RT_STREAM_01</span>
      </div>
      
      <div className="flex-1 overflow-y-auto font-mono text-[11px] p-2 custom-scrollbar">
        <table className="w-full text-left">
          <thead>
            <tr className="text-[#4db6ac] opacity-50 border-b border-[#1e2d2b]">
              <th className="pb-1">PID</th>
              <th className="pb-1">HR</th>
              <th className="pb-1">FHR</th>
              <th className="pb-1 text-right">STATUS</th>
            </tr>
          </thead>
          <tbody>
            {vitals.map((v, i) => (
              <tr key={i} className="border-b border-[#172221] last:border-0 hover:bg-[#1e2d2b] transition-colors">
                <td className="py-1 text-[#00ffcc]">P-{v.patient_id}</td>
                <td className="py-1 text-[#e0f2f1]">{v.rate}</td>
                <td className="py-1 text-[#00ffcc] font-bold">{v.fhr}</td>
                <td className="py-1 text-right">
                  <span className={`px-1 rounded text-[9px] ${v.fhr > 160 || v.fhr < 110 ? 'bg-[#ff3366] text-white' : 'bg-[#00ffcc]/10 text-[#00ffcc]'}`}>
                    {v.fhr > 160 || v.fhr < 110 ? 'CRIT' : 'NOM'}
                  </span>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  );
};

export default VitalsLog;
