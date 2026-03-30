#pragma once

#include <cstdint>
#include <chrono>
#include <string>

namespace maternai {

enum class PatientStatus : uint8_t {
    STABLE = 0,
    OBSERVATION = 1,
    URGENT = 2,
    CRITICAL = 3
};

// Represents a single "tick" of physiological data
// Packed to 16 bytes for cache-line efficiency
struct alignas(16) VitalsFrame {
    uint64_t timestamp_ns;  // Tick timestamp
    uint32_t patient_id;    // Unique patient identifier
    float heart_rate;       // Beats per minute
    float spo2;            // Oxygen saturation (0-100)
    float systolic_bp;     // Systolic blood pressure
    float diastolic_bp;    // Diastolic blood pressure
    float fetal_heart_rate; // The "Alpha" signal
    uint16_t sensor_id;     // IoT device ID
    
    // Status flags (bitfield for efficiency)
    uint8_t flags; 
};

struct RiskScore {
    uint32_t patient_id;
    float cumulative_risk;  // Normalized 0.0 - 1.0
    PatientStatus status;
    uint64_t alert_id;
};

} // namespace maternai
