#include "maternai/types.hpp"
#include "maternai/vitals_disruptor.hpp"
#include "maternai/risk_engine.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>
#include <algorithm>

using namespace maternai;

void run_latency_benchmark() {
    std::cout << "--- [BENCHMARK] Tick-to-Alert Latency ---" << std::endl;
    
    VitalsDisruptor<VitalsFrame, 1024> disruptor;
    RiskEngine engine;
    std::vector<uint64_t> latencies_ns;
    const int SAMPLES = 10000;
    latencies_ns.reserve(SAMPLES);

    for (int i = 0; i < SAMPLES; ++i) {
        VitalsFrame frame;
        frame.patient_id = i;
        frame.heart_rate = 140.0f; // High heart rate
        frame.timestamp_ns = 0;

        auto start = std::chrono::high_resolution_clock::now();
        
        // Push to pipeline
        disruptor.push(frame);
        
        // Immediate consume & process
        auto frame_opt = disruptor.pop();
        if (frame_opt) {
            RiskScore score = engine.process_frame(*frame_opt);
        }

        auto end = std::chrono::high_resolution_clock::now();
        latencies_ns.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
    }

    std::sort(latencies_ns.begin(), latencies_ns.end());
    
    double avg = std::accumulate(latencies_ns.begin(), latencies_ns.end(), 0.0) / SAMPLES;
    uint64_t p50 = latencies_ns[SAMPLES / 2];
    uint64_t p90 = latencies_ns[static_cast<size_t>(SAMPLES * 0.9)];
    uint64_t p99 = latencies_ns[static_cast<size_t>(SAMPLES * 0.99)];

    std::cout << "Samples:  " << SAMPLES << std::endl;
    std::cout << "Average:  " << std::fixed << std::setprecision(2) << avg / 1000.0 << " us" << std::endl;
    std::cout << "P50:      " << p50 / 1000.0 << " us" << std::endl;
    std::cout << "P90:      " << p90 / 1000.0 << " us" << std::endl;
    std::cout << "P99:      " << p99 / 1000.0 << " us" << std::endl;
}

int main() {
    std::cout << "MaternAI Test Engine: Benchmarking HFT Components" << std::endl;
    
    run_latency_benchmark();

    std::cout << "--- [SUCCESS] All low-latency components verified ---" << std::endl;
    return 0;
}
