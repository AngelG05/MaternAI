#pragma once

#include "maternai/types.hpp"
#include "maternai/vitals_disruptor.hpp"
#include <iostream>
#include <thread>
#include <atomic>
#include <random>

namespace maternai {

/**
 * @brief High-throughput vitals ingestion layer.
 * Simulated UDP receiver for IoT vitals monitors.
 */
class VitalsReceiver {
public:
    VitalsReceiver(VitalsDisruptor<VitalsFrame, 65536>& queue)
        : queue_(queue), running_(false) {}

    /**
     * @brief Starts the ingestion loop on a pinned thread.
     */
    void start() {
        running_ = true;
        worker_thread_ = std::thread([this]() {
            // In a production HFT system, we'd pin this thread to an isolated core
            // and use io_uring to read UDP frames directly into the ring buffer.
            this->ingest_loop();
        });
    }

    void stop() {
        running_ = false;
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
    }

private:
    void ingest_loop() {
        std::mt19937 rng(1337);
        std::uniform_real_distribution<float> hr_dist(60.0f, 130.0f);
        std::uniform_real_distribution<float> spo2_dist(88.0f, 100.0f);
        std::uniform_real_distribution<float> bp_dist(90.0f, 170.0f);
        std::uniform_real_distribution<float> fhr_dist(105.0f, 170.0f);

        uint64_t frame_count = 0;

        while (running_) {
            VitalsFrame frame;
            frame.timestamp_ns = static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());
            frame.patient_id = 101; // Simulation: fixed patient
            frame.heart_rate = hr_dist(rng);
            frame.spo2 = spo2_dist(rng);
            frame.systolic_bp = bp_dist(rng);
            frame.diastolic_bp = frame.systolic_bp * 0.7f;
            frame.fetal_heart_rate = fhr_dist(rng);
            frame.sensor_id = 42;
            frame.flags = 0;

            // Non-blocking push into the disruptor
            while (!queue_.push(frame) && running_) {
                // Busy wait or yield depending on latency profile
                std::this_thread::yield();
            }

            frame_count++;
            // Simulate 100Hz feed for 1000 patients or high-speed hardware
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }

        std::cout << "[INGEST] Signal feed stopped. Total frames: " << frame_count << std::endl;
    }

    VitalsDisruptor<VitalsFrame, 65536>& queue_;
    std::atomic<bool> running_;
    std::thread worker_thread_;
};

} // namespace maternai
