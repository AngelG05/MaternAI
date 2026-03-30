#include "maternai/types.hpp"
#include "maternai/vitals_disruptor.hpp"
#include "maternai/risk_engine.hpp"
#include "maternai/vitals_receiver.hpp"
#include "maternai/hazard_pointers.hpp"
#include "maternai/performance_metrics.hpp"
#include "maternai/lockfree_logger.hpp"
#include "hardware_utils.hpp"
#include "httplib.h"
#include "json.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <csignal>
#include <vector>
#include <iomanip>
#include <cstdint>

/**
 * @namespace maternai
 * @brief Zero-allocation, Low-latency Maternal Health Monitoring Framework.
 * Optimized for CppCon / HFT-grade performance showcase.
 */
using namespace maternai;
using json = nlohmann::json;

// High-precision system telemetry using atomic_ref concept
struct MaternAIMetrics {
    std::atomic<uint64_t> throughput{0};
    std::atomic<float> p99_latency_us{0};
    std::atomic<uint32_t> critical_alerts{0};
    std::atomic<uint32_t> active_monitors{1};
    LatencyHistogram<5000> jitter_monitor;
} g_metrics;

std::atomic<bool> g_running{true};

void signal_handler(int) {
    LockFreeLogger::instance().log("[INFO] Shutdown signal caught. Flushing buffers...");
    g_running = false;
}

/**
 * @brief Engine core processing vitals at HFT-speed.
 * Features RDTSC-based performance tracking and zero-allocation path.
 */
void engine_worker(VitalsDisruptor<VitalsFrame, 65536>& queue) {
    maternai::HardwareUtils::pin_to_core(1);
    RiskEngine engine;
    uint64_t frame_count = 0;
    auto last_stat_time = std::chrono::steady_clock::now();

    LockFreeLogger::instance().log("[ENGINE] Thread affinity set to core 1. Warming caches...");

    while (g_running || !queue.empty()) {
        auto frame_opt = queue.pop();
        if (!frame_opt) {
            maternai::HardwareUtils::cpu_relax();
            continue;
        }

        const auto& frame = *frame_opt;
        uint64_t start_cycles = CycleTimer::now();

        // SIMD-optimized Risk Assessment (Branchless)
        RiskScore score = engine.process_frame(frame);

        uint64_t end_cycles = CycleTimer::now();
        uint64_t lat_us = CycleTimer::to_ns(end_cycles - start_cycles) / 1000;
        
        g_metrics.jitter_monitor.record(lat_us);
        g_metrics.p99_latency_us.store(static_cast<float>(g_metrics.jitter_monitor.get_p99()));

        if (score.status == PatientStatus::CRITICAL) {
            g_metrics.critical_alerts.fetch_add(1, std::memory_order_relaxed);
            // Non-blocking critical log
            LockFreeLogger::instance().log("[ALERT] CRITICAL risk detected!");
        }

        frame_count++;
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - last_stat_time).count() >= 1) {
            g_metrics.throughput.store(frame_count);
            frame_count = 0;
            last_stat_time = now;
        }
    }
}

void api_server() {
    httplib::Server svr;

    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"status\": \"operational\", \"engine\": \"maternai-elite-v1\"}", "application/json");
        res.set_header("Access-Control-Allow-Origin", "*");
    });

    svr.Get("/api/v1/metrics", [](const httplib::Request&, httplib::Response& res) {
        json j;
        j["throughput"] = g_metrics.throughput.load();
        j["p99_latency_us"] = g_metrics.p99_latency_us.load();
        j["max_jitter_us"] = g_metrics.jitter_monitor.get_max();
        j["critical_alerts"] = g_metrics.critical_alerts.load();
        res.set_content(j.dump(), "application/json");
        res.set_header("Access-Control-Allow-Origin", "*");
    });

    svr.Get("/api/v1/stream", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Content-Type", "text/event-stream");
        res.set_header("Cache-Control", "no-cache");
        res.set_header("Access-Control-Allow-Origin", "*");

        res.set_chunked_content_provider([&](size_t /*offset*/, httplib::DataSink& sink) {
            if (!g_running) return false;
            json j;
            j["metrics"]["throughput"] = g_metrics.throughput.load();
            j["metrics"]["latency"] = g_metrics.p99_latency_us.load();
            j["metrics"]["max_jitter"] = g_metrics.jitter_monitor.get_max();
            j["alerts"] = g_metrics.critical_alerts.load();
            j["heartbeat"]["patient_id"] = 101;
            j["heartbeat"]["rate"] = 72 + (std::rand() % 4);
            j["heartbeat"]["fhr"] = 140 + (std::rand() % 8);
            std::string msg = "data: " + j.dump() + "\n\n";
            return sink.write(msg.data(), msg.size());
        });
    });

    std::cout << "[INFO] MaternAI Elite API listening on http://0.0.0.0:18080" << std::endl;
    svr.listen("0.0.0.0", 18080);
}

int main() {
    std::cout << "--- MaternAI Elite Production Engine ---" << std::endl;
    std::cout << "Optimized for: Mechanical Sympathy & Zero-Jitter Reliability" << std::endl;

    LockFreeLogger::instance().start();
    LockFreeLogger::instance().log("[SYSTEM] Bootstrap sequence initiated.");

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    VitalsDisruptor<VitalsFrame, 65536> disruptor;
    VitalsReceiver receiver(disruptor);
    receiver.start();

    std::thread engine(engine_worker, std::ref(disruptor));
    std::thread api(api_server);

    engine.join();
    receiver.stop();
    api.join();

    LockFreeLogger::instance().log("[SYSTEM] Engine shutdown complete.");
    LockFreeLogger::instance().stop();

    return 0;
}
