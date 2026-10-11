# Bolt's Journal - Critical Learnings

## 2025-05-18 - Matrix Reprojection Optimization in Frame Interpolation
**Learning:** Combining consecutive coordinate space transformations (NDC -> World -> NDC) into a single combined matrix multiplication (`currViewProj * prevInvViewProj`) before looping over millions of frame pixels halves floating point math overhead per pixel and enables vectorization.
**Action:** Always check pixel shader or frame generation loops for per-frame constant matrix math that can be composed and hoisted out of the spatial loop.
