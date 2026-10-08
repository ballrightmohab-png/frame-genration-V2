# Bolt's Journal - Critical Learnings

## 2025-10-08 - Motion Estimation Luma Precomputation & Parallelization
**Learning:** In frame generation algorithms, calculating luma per search candidate inside block matching inner loops introduces redundant $O(W \times H \times R^2)$ RGB-to-Luma conversions. Precomputing flat luma buffers prior to motion search and parallelizing block matching with OpenMP eliminates redudant math and scales search performance linearly across CPU threads.
**Action:** Always precompute pixel attributes (e.g. luma/gradients) into contiguous contiguous memory arrays before block search iterations.
