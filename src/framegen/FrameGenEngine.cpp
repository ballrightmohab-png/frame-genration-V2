#include "framegen/FrameGenEngine.h"
#include <algorithm>
#include <cstring>
#include <cmath>

namespace LeviMod {

    void FrameGenEngine::initialize(int width, int height) {
        resize(width, height);
    }

    void FrameGenEngine::resize(int width, int height) {
        if (m_width == width && m_height == height && m_prevFrame.colorBuffer.size() > 0) return;

        m_width = width;
        m_height = height;

        size_t colorSize = static_cast<size_t>(width * height * 4);
        size_t depthSize = static_cast<size_t>(width * height);

        m_prevFrame.width = width;
        m_prevFrame.height = height;
        m_prevFrame.colorBuffer.resize(colorSize, 0);
        m_prevFrame.depthBuffer.resize(depthSize, 1.0f);

        m_currFrame.width = width;
        m_currFrame.height = height;
        m_currFrame.colorBuffer.resize(colorSize, 0);
        m_currFrame.depthBuffer.resize(depthSize, 1.0f);

        m_hasPrevFrame = false;
        m_hasCurrFrame = false;
    }

    void FrameGenEngine::pushNewFrame(const uint8_t* colorPixels, const float* depthPixels, const Mat4& viewProjMatrix) {
        if (!colorPixels || m_width <= 0 || m_height <= 0) return;

        size_t colorSize = static_cast<size_t>(m_width * m_height * 4);
        size_t depthSize = static_cast<size_t>(m_width * m_height);

        // Swap buffer pointers rather than copy vector allocations for zero-copy efficiency
        if (m_hasCurrFrame) {
            std::swap(m_prevFrame.colorBuffer, m_currFrame.colorBuffer);
            std::swap(m_prevFrame.depthBuffer, m_currFrame.depthBuffer);
            m_prevFrame.viewProjMatrix = m_currFrame.viewProjMatrix;
            m_prevFrame.width = m_currFrame.width;
            m_prevFrame.height = m_currFrame.height;
            m_hasPrevFrame = true;
        }

        m_currFrame.width = m_width;
        m_currFrame.height = m_height;
        if (m_currFrame.colorBuffer.size() != colorSize) {
            m_currFrame.colorBuffer.resize(colorSize);
        }
        std::memcpy(m_currFrame.colorBuffer.data(), colorPixels, colorSize);

        if (depthPixels) {
            if (m_currFrame.depthBuffer.size() != depthSize) {
                m_currFrame.depthBuffer.resize(depthSize);
            }
            std::memcpy(m_currFrame.depthBuffer.data(), depthPixels, depthSize * sizeof(float));
        } else {
            if (m_currFrame.depthBuffer.size() != depthSize) {
                m_currFrame.depthBuffer.resize(depthSize, 0.5f);
            } else {
                std::fill(m_currFrame.depthBuffer.begin(), m_currFrame.depthBuffer.end(), 0.5f);
            }
        }

        m_currFrame.viewProjMatrix = viewProjMatrix;
        m_hasCurrFrame = true;
    }

    bool FrameGenEngine::generateInterpolatedFrame(float t, std::vector<uint8_t>& outputColorBuffer) {
        if (!isReady()) return false;

        size_t colorSize = static_cast<size_t>(m_width * m_height * 4);
        if (outputColorBuffer.size() != colorSize) {
            outputColorBuffer.resize(colorSize);
        }

        // Pre-compute combined reprojection matrix (currViewProj * prevInvViewProj) once per frame.
        // Transforming NDC directly to NDC saves 1 matrix-vector transform, 1 check, and 1 divide per pixel.
        Mat4 reprojMatrix = m_currFrame.viewProjMatrix.multiply(m_prevFrame.viewProjMatrix.inverse());

        // Cache matrix elements in local stack registers to facilitate SIMD vectorization
        const float rm0 = reprojMatrix.m[0],  rm1 = reprojMatrix.m[1],  rm2 = reprojMatrix.m[2],  rm3 = reprojMatrix.m[3];
        const float rm4 = reprojMatrix.m[4],  rm5 = reprojMatrix.m[5],  rm6 = reprojMatrix.m[6],  rm7 = reprojMatrix.m[7];
        const float rm8 = reprojMatrix.m[8],  rm9 = reprojMatrix.m[9],  rm10 = reprojMatrix.m[10], rm11 = reprojMatrix.m[11];
        const float rm12 = reprojMatrix.m[12], rm13 = reprojMatrix.m[13], rm14 = reprojMatrix.m[14], rm15 = reprojMatrix.m[15];

        const uint8_t* prevColor = m_prevFrame.colorBuffer.data();
        const uint8_t* currColor = m_currFrame.colorBuffer.data();
        const float* depthBuf = m_currFrame.depthBuffer.data();

        float invWidth = 1.0f / static_cast<float>(m_width);
        float invHeight = 1.0f / static_cast<float>(m_height);

        // Perform parallelized/optimized spatial-temporal reprojection motion interpolation
        #pragma omp parallel for collapse(2) if(m_width * m_height >= 10000)
        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width; ++x) {
                float v = (static_cast<float>(y) + 0.5f) * invHeight;
                float u = (static_cast<float>(x) + 0.5f) * invWidth;
                int pixelIdx = y * m_width + x;

                float depth = depthBuf[pixelIdx];

                // Reconstruct pixel NDC coordinates
                float ndcX = u * 2.0f - 1.0f;
                float ndcY = v * 2.0f - 1.0f;
                float ndcZ = depth * 2.0f - 1.0f;

                // Directly reproject NDC from previous frame to current frame NDC using pre-composed reprojection matrix
                float rW = rm3 * ndcX + rm7 * ndcY + rm11 * ndcZ + rm15;
                if (std::abs(rW) < 1e-6f) rW = 1.0f;

                float currNdcX = (rm0 * ndcX + rm4 * ndcY + rm8 * ndcZ + rm12) / rW;
                float currNdcY = (rm1 * ndcX + rm5 * ndcY + rm9 * ndcZ + rm13) / rW;

                float currU = currNdcX * 0.5f + 0.5f;
                float currV = currNdcY * 0.5f + 0.5f;

                float motionU = currU - u;
                float motionV = currV - v;

                // Warped sample locations
                float samplePrevU = std::clamp(u + motionU * t, 0.0f, 1.0f);
                float samplePrevV = std::clamp(v + motionV * t, 0.0f, 1.0f);

                float sampleCurrU = std::clamp(u - motionU * (1.0f - t), 0.0f, 1.0f);
                float sampleCurrV = std::clamp(v - motionV * (1.0f - t), 0.0f, 1.0f);

                int prevPxX = std::clamp(static_cast<int>(samplePrevU * m_width), 0, m_width - 1);
                int prevPxY = std::clamp(static_cast<int>(samplePrevV * m_height), 0, m_height - 1);
                int prevPxIdx = (prevPxY * m_width + prevPxX) * 4;

                int currPxX = std::clamp(static_cast<int>(sampleCurrU * m_width), 0, m_width - 1);
                int currPxY = std::clamp(static_cast<int>(sampleCurrV * m_height), 0, m_height - 1);
                int currPxIdx = (currPxY * m_width + currPxX) * 4;

                int outIdx = pixelIdx * 4;
                for (int c = 0; c < 4; ++c) {
                    float valPrev = static_cast<float>(prevColor[prevPxIdx + c]);
                    float valCurr = static_cast<float>(currColor[currPxIdx + c]);
                    float blended = valPrev * (1.0f - t) + valCurr * t;
                    outputColorBuffer[outIdx + c] = static_cast<uint8_t>(std::clamp(blended, 0.0f, 255.0f));
                }
            }
        }

        return true;
    }

} // namespace LeviMod
