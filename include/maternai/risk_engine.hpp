#pragma once

#include "types.hpp"
#include <vector>

namespace maternai {

/**
 * @brief High-frequency quantitative risk engine.
 * Applies physiological "Alpha" signals to detect early warning signs of pre-eclampsia.
 */
class RiskEngine {
public:
    RiskEngine() = default;

    /**
     * @brief Computes a risk score for a single frame.
     * Uses branchless logic where possible for deterministic performance.
     */
    [[nodiscard]] RiskScore process_frame(const VitalsFrame& frame) noexcept;

    /**
     * @brief Vectorized risk computation for a batch of frames.
     * Leveraging SIMD instructions (AVX/AVX2/AVX-512) for high-throughput patient monitoring.
     */
    void process_batch(const VitalsFrame* frames, std::size_t count, RiskScore* results) noexcept;

private:
    // Scoring thresholds based on MEOWS (Modified Early Obstetric Warning Score)
    static constexpr float HR_UPPER_CRITICAL = 120.0f;
    static constexpr float SPO2_LOWER_CRITICAL = 92.0f;
    static constexpr float BP_SYSTOLIC_UPPER_CRITICAL = 160.0f;
};

} // namespace maternai
