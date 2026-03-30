#pragma once

#include "types.hpp"
#include <atomic>
#include <array>
#include <optional>
#include <new>
#include <thread>

#ifdef __cpp_lib_hardware_interference_size
    using std::hardware_destructive_interference_size;
#else
    constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

namespace maternai {

/**
 * @brief Busy-spin wait strategy for ultra-low latency.
 * Minimizes context switching by keeping the CPU core hot.
 */
struct BusySpinWaitStrategy {
    void wait() const noexcept {
#if defined(__i386__) || defined(__x86_64__)
        _mm_pause(); // Efficient x86 spin-loop hint
#elif defined(__aarch64__)
        asm volatile("yield" ::: "memory");
#endif
    }
};

/**
 * @brief High-performance, SPSC lock-free ring buffer (LMAX Disruptor Pattern).
 * Optimized for cache-line alignment and explicit memory ordering.
 */
template<typename T, std::size_t Capacity, typename WaitStrategy = BusySpinWaitStrategy>
class alignas(hardware_destructive_interference_size) VitalsDisruptor {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of 2");

public:
    VitalsDisruptor() : head_(0), tail_(0) {}

    // No copying/moving for low-latency durability
    VitalsDisruptor(const VitalsDisruptor&) = delete;
    VitalsDisruptor& operator=(const VitalsDisruptor&) = delete;

    /**
     * @brief Pushes a new frame. Wait-free if buffer not full.
     */
    bool push(const T& frame) noexcept {
        const auto current_tail = tail_.load(std::memory_order_relaxed);
        const auto next_tail = (current_tail + 1) & (Capacity - 1);

        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false; // Full
        }

        buffer_[current_tail] = frame;
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }

    /**
     * @brief Consumer poll with wait strategy support.
     */
    std::optional<T> pop() noexcept {
        const auto current_head = head_.load(std::memory_order_relaxed);
        
        if (current_head == tail_.load(std::memory_order_acquire)) {
            return std::nullopt; // Empty
        }

        T frame = buffer_[current_head];
        head_.store((current_head + 1) & (Capacity - 1), std::memory_order_release);
        return frame;
    }

    bool empty() const noexcept {
        return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire);
    }

    size_t size() const noexcept {
        return (tail_.load(std::memory_order_acquire) - head_.load(std::memory_order_acquire)) & (Capacity - 1);
    }

private:
    // Padded to 64 bytes to prevent false sharing between producer (tail) and consumer (head)
    alignas(hardware_destructive_interference_size) std::atomic<std::size_t> head_;
    alignas(hardware_destructive_interference_size) std::atomic<std::size_t> tail_;
    
    // The data buffer is also padded to ensure it resides on its own cache lines
    alignas(hardware_destructive_interference_size) std::array<T, Capacity> buffer_;
    WaitStrategy wait_strategy_;
};

} // namespace maternai
