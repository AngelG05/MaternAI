#pragma once

#include <cstdint>
#include <array>
#include <atomic>
#include <algorithm>
#include <cmath>

#if defined(__x86_64__) || defined(_M_X64)
#include <x86intrin.h>
#elif defined(__aarch64__)
// For ARM64 cycle counting
#endif

namespace maternai {

/**
 * @brief Cycle-accurate timer using hardware RDTSC.
 */
class CycleTimer {
public:
    static inline uint64_t now() noexcept {
#if defined(__x86_64__) || defined(_M_X64)
        unsigned int aux;
        return __rdtscp(&aux);
#else
        return 0; // Fallback for primitive archs
#endif
    }

    static inline uint64_t to_ns(uint64_t cycles, double ghz = 3.5) noexcept {
        return static_cast<uint64_t>(cycles / ghz);
    }
};

/**
 * @brief High-resolution histogram for tracking tail latency (P99.99).
 * Designed for zero-allocation and minimal overhead in the hot path.
 */
template<size_t MaxLatencyUs = 1000>
class LatencyHistogram {
public:
    LatencyHistogram() {
        buckets_.fill(0);
    }

    void record(uint64_t latency_us) noexcept {
        size_t index = std::min(latency_us, static_cast<uint64_t>(MaxLatencyUs - 1));
        buckets_[index].fetch_add(1, std::memory_order_relaxed);
        
        uint64_t current_max = max_latency_us_.load(std::memory_order_relaxed);
        while (latency_us > current_max && !max_latency_us_.compare_exchange_weak(current_max, latency_us)) {}
    }

    uint64_t get_p99() const noexcept {
        return get_percentile(0.99);
    }

    uint64_t get_max() const noexcept {
        return max_latency_us_.load(std::memory_order_acquire);
    }

    void reset() noexcept {
        for (auto& b : buckets_) b.store(0, std::memory_order_relaxed);
        max_latency_us_.store(0, std::memory_order_relaxed);
    }

private:
    uint64_t get_percentile(double p) const noexcept {
        uint64_t total = 0;
        for (const auto& b : buckets_) total += b.load(std::memory_order_relaxed);
        if (total == 0) return 0;

        uint64_t target = static_cast<uint64_t>(total * p);
        uint64_t current = 0;
        for (size_t i = 0; i < MaxLatencyUs; ++i) {
            current += buckets_[i].load(std::memory_order_relaxed);
            if (current >= target) return i;
        }
        return MaxLatencyUs;
    }

    std::array<std::atomic<uint64_t>, MaxLatencyUs> buckets_;
    std::atomic<uint64_t> max_latency_us_{0};
};

} // namespace maternai
