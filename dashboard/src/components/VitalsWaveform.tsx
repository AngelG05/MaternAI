import React, { useEffect, useRef, useState } from 'react';

interface Props {
  color?: string;
  points?: number;
}

const VitalsWaveform: React.FC<Props> = ({ color = '#00ffcc', points = 200 }) => {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const data = useRef<number[]>(new Array(points).fill(0));
  const offset = useRef(0);

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    if (!ctx) return;

    let animationId: number;

    const render = () => {
      // Clear canvas
      ctx.clearRect(0, 0, canvas.width, canvas.height);
      
      // Update data with sine-wave simulation + noise
      offset.current += 0.05;
      const newValue = Math.sin(offset.current) * 20 + (Math.random() * 5);
      data.current.push(newValue);
      if (data.current.length > points) data.current.shift();

      // Draw grid
      ctx.strokeStyle = '#1e2d2b';
      ctx.setLineDash([5, 5]);
      ctx.beginPath();
      for (let i = 0; i < canvas.width; i += 40) {
        ctx.moveTo(i, 0);
        ctx.lineTo(i, canvas.height);
      }
      for (let i = 0; i < canvas.height; i += 40) {
        ctx.moveTo(0, i);
        ctx.lineTo(canvas.width, i);
      }
      ctx.stroke();
      ctx.setLineDash([]);

      // Draw waveform
      ctx.beginPath();
      ctx.lineWidth = 2;
      ctx.strokeStyle = color;
      ctx.shadowBlur = 10;
      ctx.shadowColor = color;

      const step = canvas.width / (points - 1);
      const midY = canvas.height / 2;

      data.current.forEach((val, i) => {
        const x = i * step;
        const y = midY - val;
        if (i === 0) ctx.moveTo(x, y);
        else ctx.lineTo(x, y);
      });

      ctx.stroke();
      ctx.shadowBlur = 0; // Reset for performance

      animationId = requestAnimationFrame(render);
    };

    render();
    return () => cancelAnimationFrame(animationId);
  }, [color, points]);

  return (
    <div className="w-full h-full min-h-[120px] bg-[#0c1211] rounded overflow-hidden">
      <canvas 
        ref={canvasRef} 
        width={800} 
        height={200}
        className="w-full h-full block"
      />
    </div>
  );
};

export default VitalsWaveform;
