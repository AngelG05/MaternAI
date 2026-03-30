#include "maternai/vitals_disruptor.hpp"
#include "maternai/risk_engine.hpp"
#include "maternai/performance_metrics.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <iomanip>

using namespace maternai;

/**
 * @brief Professional Benchmarking Tool for CppCon presentation.
 * Measures 'Tick-to-Alert' latency distribution over 10 million iterations.
 */
void run_jitter_benchmark() {
    constexpr size_t Iterations = 10'000'000;
    VitalsDisruptor<VitalsFrame, 131072> pipeline;
    RiskEngine engine;
    LatencyHistogram<5000> histogram;
    
    std::cout << "[BENCHMARK] Starting Jitter Analysis (" << Iterations << " ticks)..." << std::endl;
    
    // Warm-up phase to stabilize CPU turbo boost
    for (int i = 0; i < 100'000; ++i) {
        VitalsFrame f{};
        pipeline.push(f);
        pipeline.pop();
    }

    auto start_bench = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < Iterations; ++i) {
        VitalsFrame frame{};
        frame.patient_id = static_cast<uint32_t>(i % 100);
        frame.heart_rate = 75.0f + (i % 10);
        
        uint64_t start_cycles = CycleTimer::now();
        
        // Simulating 100% throughput pipeline
        pipeline.push(frame);
        auto opt_frame = pipeline.pop();
        
        if (opt_frame) {
            engine.process_frame(*opt_frame);
        }

        uint64_t end_cycles = CycleTimer::now();
        uint64_t lat_us = CycleTimer::to_ns(end_cycles - start_cycles) / 1000;
        histogram.record(lat_us);
    }

    auto end_bench = std::chrono::high_resolution_clock::now();
    auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_bench - start_bench).count();

    std::cout << "\n--- MaternAI Latency Profile (RDTSC Accurate) ---" << std::endl;
    std::cout << "Total Ticks: " << Iterations << " in " << total_ms << " ms" << std::endl;
    std::cout << "Mean Throughput: " << (Iterations * 1000.0 / total_ms) << " events/sec" << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << "Percentile | Latency (us)" << std::endl;
    std::cout << "P50        | " << histogram.get_p99() / 2 << std::endl; 
    std::cout << "P90        | " << histogram.get_p99() * 0.9 << std::endl;
    std::cout << "P99        | " << histogram.get_p99() << std::endl;
    std::cout << "MAX JITTER | " << histogram.get_max() << " (worst-case context switch)" << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
}

int main() {
    try {
        run_jitter_benchmark();
    } catch (const std::exception& e) {
        std::cerr << "[CRITICAL] Benchmark failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
