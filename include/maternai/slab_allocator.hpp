#pragma once

#include <vector>
#include <cstddef>
#include <atomic>
#include <new>
#include <stdexcept>
#include <memory>

namespace maternai {

/**
 * @brief Thread-Local Slab Allocator for true wait-free performance.
 * Eliminates atomic contention in the hot path by providing each thread with a local pool.
 */
template<typename T, std::size_t PoolSize = 1024>
class SlabAllocator {
public:
    SlabAllocator() : pool_(nullptr), next_free_(0) {
        pool_ = static_cast<T*>(std::aligned_alloc(64, sizeof(T) * PoolSize));
        if (!pool_) throw std::bad_alloc();
        
        // Pool Warming: Pre-touch memory to avoid page faults in hot path
        for (std::size_t i = 0; i < PoolSize; ++i) {
            new (&pool_[i]) T();
        }
    }

    ~SlabAllocator() {
        if (pool_) {
            for (std::size_t i = 0; i < next_free_; ++i) {
                pool_[i].~T();
            }
            std::free(pool_);
        }
    }

    // Prohibit copying
    SlabAllocator(const SlabAllocator&) = delete;
    SlabAllocator& operator=(const SlabAllocator&) = delete;

    /**
     * @brief Perfectly forwarded allocation from the thread-local pool.
     */
    template<typename... Args>
    T* allocate(Args&&... args) noexcept {
        if (next_free_ >= PoolSize) return nullptr;
        T* obj = &pool_[next_free_++];
        new (obj) T(std::forward<Args>(args)...);
        return obj;
    }

    void reset() noexcept {
        next_free_ = 0;
    }

    std::size_t used() const noexcept { return next_free_; }
    std::size_t capacity() const noexcept { return PoolSize; }

private:
    T* pool_;
    std::size_t next_free_;
};

} // namespace maternai
