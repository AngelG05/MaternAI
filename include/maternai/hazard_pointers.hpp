#pragma once

#include <atomic>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <algorithm>

namespace maternai {

/**
 * @brief Hazard Pointers for safe lock-free memory reclamation.
 * Essential for high-performance concurrent readers without shared pointer overhead.
 */
template<typename T>
class HazardPointerManager {
public:
    static constexpr std::size_t MaxThreads = 64;

    struct alignas(64) HazardPointer {
        std::atomic<T*> ptr{nullptr};
        std::atomic<bool> active{false};
    };

    HazardPointerManager() : hazard_pointers_(MaxThreads) {}

    /**
     * @brief Reserves a hazard pointer for the calling thread.
     */
    HazardPointer* acquire() {
        for (auto& hp : hazard_pointers_) {
            bool expected = false;
            if (hp.active.compare_exchange_strong(expected, true)) {
                return &hp;
            }
        }
        return nullptr; // Should not happen with enough capacity
    }

    /**
     * @brief Releases a hazard pointer.
     */
    void release(HazardPointer* hp) {
        hp->ptr.store(nullptr, std::memory_order_release);
        hp->active.store(false, std::memory_order_release);
    }

    /**
     * @brief Retires a pointer, moving it to a retirement list.
     * Reclaims memory once no hazard pointers are protecting it.
     */
    void retire(std::unique_ptr<T> ptr) {
        std::lock_guard<std::mutex> lock(retirement_mutex_);
        retired_list_.push_back(std::move(ptr));
        reclaim();
    }

private:
    void reclaim() {
        // Collect all currently active hazard pointers
        std::vector<T*> active_ptrs;
        for (const auto& hp : hazard_pointers_) {
            T* p = hp.ptr.load(std::memory_order_acquire);
            if (p) active_ptrs.push_back(p);
        }

        // Remove from retired list if no longer active
        auto it = std::remove_if(retired_list_.begin(), retired_list_.end(),
            [&](const std::unique_ptr<T>& retired) {
                return std::find(active_ptrs.begin(), active_ptrs.end(), retired.get()) == active_ptrs.end();
            });
        retired_list_.erase(it, retired_list_.end());
    }

    std::vector<HazardPointer> hazard_pointers_;
    std::mutex retirement_mutex_;
    std::vector<std::unique_ptr<T>> retired_list_;
};

} // namespace maternai
