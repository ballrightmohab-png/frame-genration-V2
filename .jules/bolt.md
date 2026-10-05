# Bolt's Journal - Critical Learnings

## 2025-05-18 - Matrix Reprojection Homogeneous Coordinate Cancellation
**Learning:** In spatial-temporal frame generation, two-step matrix projections (world un-projection followed by current clip-projection) can be combined into a single matrix product (`reprojectMat = currViewProj * prevInvViewProj`). Homogeneous perspective division factors ($w_P$) cancel out during NDC coordinate calculation, eliminating per-pixel intermediate divisions and matrix transformations.
**Action:** Always combine view-projection matrix chains into a single matrix prior to per-pixel/per-vertex operations.
