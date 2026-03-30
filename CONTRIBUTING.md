# Contributing to MaternAI

Thank you for your interest in contributing to **MaternAI**! We are building a high-performance system where every microsecond saved can improve clinical outcomes.

## 🎯 Our Philosophy

MaternAI isn't just a medical application; it's a piece of systems engineering. We believe in:
1.  **Mechanical Sympathy:** Understanding the underlying hardware (cache lines, NUMA, SIMD).
2.  **Zero-Allocation Paths:** No heap allocations (malloc/new) are allowed in the "hot path" (vitals ingestion -> risk scoring -> alert dispatch).
3.  **Lock-Free Progress:** Coordination between threads should happen via atomics and wait-free data structures wherever possible.

## 🛠️ Code Standards

### C++ Guidelines
- **Standard:** C++20.
- **Namespaces:** All core logic must reside in the `maternai` namespace.
- **Headers:** Prefer `#pragma once`. Use angle brackets for external libraries and quotes for internal headers.
- **Latency:** Avoid RTTI, exceptions in the hot path, and unnecessary virtual function calls.
- **SIMD:** When implementing signal processing, ensure a fallback path is provided for non-AVX systems.

### Git Workflow
1.  Fork the repository.
2.  Create a feature branch (`git checkout -b feature/dynamic-thresholds`).
3.  Ensure all latency benchmarks pass (`./test_engine`).
4.  Submit a Pull Request with detailed performance impact analysis.

## 🧪 Testing

We value **Deterministic Correctness**. If you add a new risk signal:
1.  Add a unit test in `src/test_engine.cpp`.
2.  Run the latency profiler to ensure no regression in P99 timing.
3.  Verify thread-safety with `ThreadSanitizer`.

## 🎨 UI/UX

The dashboard should follow the **Edge Control** aesthetic:
-   Maintain the dark, high-contrast teal/red palette.
-   Ensure all telemetry updates at 60fps where possible.

---
*MaternAI: Engineering for the most important heartbeats of all.*
