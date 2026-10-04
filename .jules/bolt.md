## 2025-05-18 - Precomputing Luma Maps in Motion Estimation

**Learning:** Block-matching motion estimation algorithms sample luma repeatedly for every candidate offset in search windows (e.g., $17 \times 17 = 289$ candidates). On-the-fly luma conversion causes millions of redundant float multiplications and clamping operations.

**Action:** Always precompute luma arrays once per frame prior to running search loops to convert repeated computations into $O(1)$ memory lookups.
