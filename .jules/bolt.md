## 2025-05-18 - Precompute Motion Estimation Luma Buffers in TFR Frame Generation

**Learning:** In block-matching motion estimation algorithms, evaluating pixel luma dynamically inside the candidate search loop causes O(Radius^2 * BlockSize^2) redundant RGB-to-Luma floating point calculations and array bounds clamping.
**Action:** Precompute frame luma into a single linear buffer before motion estimation loops so candidate vector searches perform fast O(1) array index lookups.
