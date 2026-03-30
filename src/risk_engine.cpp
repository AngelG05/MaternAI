#include "maternai/risk_engine.hpp"
#include <algorithm>
#include <cmath>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

namespace maternai {

RiskScore RiskEngine::process_frame(const VitalsFrame& frame) noexcept {
    float score = 0.0f;

    // Use branchless logic: (condition) * penalty
    score += (frame.heart_rate > HR_UPPER_CRITICAL) * 3.0f;
    score += (frame.spo2 < SPO2_LOWER_CRITICAL) * 3.0f;
    score += (frame.systolic_bp > BP_SYSTOLIC_UPPER_CRITICAL) * 3.0f;
    score += (frame.fetal_heart_rate < 110.0f || frame.fetal_heart_rate > 160.0f) * 2.0f;

    RiskScore rs;
    rs.patient_id = frame.patient_id;
    rs.cumulative_risk = std::clamp(score / 10.0f, 0.0f, 1.0f);
    
    if (rs.cumulative_risk > 0.8f) rs.status = PatientStatus::CRITICAL;
    else if (rs.cumulative_risk > 0.5f) rs.status = PatientStatus::URGENT;
    else if (rs.cumulative_risk > 0.2f) rs.status = PatientStatus::OBSERVATION;
    else rs.status = PatientStatus::STABLE;

    rs.alert_id = frame.timestamp_ns; // Simplified alert tracking
    return rs;
}

void RiskEngine::process_batch(const VitalsFrame* frames, std::size_t count, RiskScore* results) noexcept {
#if defined(__AVX2__)
    // AVX2 Vectorized batch processing (simplified example for HR scoring)
    const __m256 hr_threshold = _mm256_set1_ps(HR_UPPER_CRITICAL);
    const __m256 three = _mm256_set1_ps(3.0f);

    size_t i = 0;
    for (; i + 7 < count; i += 8) {
        // Load HR values from frames (this is simplified as frames aren't SOA)
        // In a real HFT engine, we'd use Struct-of-Arrays (SoA) for this.
        float hrs[8];
        for (int j = 0; j < 8; ++j) hrs[j] = frames[i+j].heart_rate;
        
        __m256 v_hrs = _mm256_loadu_ps(hrs);
        __m256 v_mask = _mm256_cmp_ps(v_hrs, hr_threshold, _CMP_GT_OQ);
        __m256 v_scores = _mm256_and_ps(v_mask, three);
        
        // Results storage (truncated output for this demo)
        _mm256_storeu_ps(&results[i].cumulative_risk, v_scores); 
        
        // Finalize non-vectorized fields
        for (int j = 0; j < 8; ++j) {
            results[i+j] = process_frame(frames[i+j]);
        }
    }
#endif
    // Fallback or remaining frames
    for (size_t i = 0; i < count; ++i) {
        results[i] = process_frame(frames[i]);
    }
}

} // namespace maternai
