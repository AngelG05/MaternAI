#pragma once

#include "vitals_disruptor.hpp"
#include <string>
#include <thread>
#include <fstream>
#include <iostream>
#include <atomic>

namespace maternai {

/**
 * @brief Lock-free Logger for high-performance telemetry.
 * Offloads all I/O and string formatting to a background thread.
 */
class LockFreeLogger {
public:
    struct LogEntry {
        uint64_t timestamp_ns;
        char message[128]; // Fixed-size buffer to avoid heap alloc
        uint8_t level;
    };

    static LockFreeLogger& instance() {
        static LockFreeLogger logger;
        return logger;
    }

    void start(const std::string& filename = "maternai_edge.log") {
        if (running_.exchange(true)) return;
        
        log_file_.open(filename, std::ios::out | std::ios::app);
        backend_thread_ = std::thread([this]() {
            while (running_.load(std::memory_order_relaxed) || !log_queue_.empty()) {
                auto entry_opt = log_queue_.pop();
                if (entry_opt) {
                    const auto& entry = *entry_opt;
                    if (log_file_.is_open()) {
                        log_file_ << "[" << entry.timestamp_ns << "] " 
                                  << entry.message << "\n";
                    }
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        });
    }

    void stop() {
        running_.store(false, std::memory_order_relaxed);
        if (backend_thread_.joinable()) backend_thread_.join();
        if (log_file_.is_open()) log_file_.close();
    }

    /**
     * @brief Non-blocking log call. 
     * Transfers the message to the background thread via SPSC queue.
     */
    void log(const char* msg, uint8_t level = 0) noexcept {
        LogEntry entry;
        entry.timestamp_ns = CycleTimer::now();
        std::strncpy(entry.message, msg, sizeof(entry.message) - 1);
        entry.message[sizeof(entry.message) - 1] = '\0';
        entry.level = level;
        
        log_queue_.push(entry); // Wait-free if queue not full
    }

private:
    LockFreeLogger() : running_(false) {}
    ~LockFreeLogger() { stop(); }

    std::atomic<bool> running_;
    std::thread backend_thread_;
    std::ofstream log_file_;
    VitalsDisruptor<LogEntry, 16384> log_queue_;
};

} // namespace maternai
