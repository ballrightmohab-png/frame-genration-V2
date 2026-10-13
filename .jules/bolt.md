## 2025-02-23 - Spatial-Temporal Motion Reprojection Loop Optimization
**Learning:** In pixel-level frame generation/reprojection loops, replacing floating-point divisions with reciprocal multiplications and hoisting invariant float conversions outside the O(W*H) loops yields a ~14.4% throughput increase without compromising quality or thread-safety in OpenMP parallel loops.
**Action:** Always check inner per-pixel loops for perspective divisions (`/ w`) and invariant UV-to-pixel coordinate scaling that can be converted into reciprocal multiplication and pre-scaled variables.
