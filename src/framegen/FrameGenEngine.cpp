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

        Mat4 prevInvViewProj = m_prevFrame.viewProjMatrix.inverse();
        const Mat4& currViewProj = m_currFrame.viewProjMatrix;

        // Optimization: Pre-multiply currViewProj * prevInvViewProj to create a single reprojection matrix.
        // This eliminates one matrix transform, world-space coordinate reconstruction, and division per pixel.
        Mat4 reprojMat = currViewProj.multiply(prevInvViewProj);

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

                // Directly reproject NDC from previous frame to current frame NDC using pre-multiplied reprojMat
                float clipW = reprojMat.m[3] * ndcX + reprojMat.m[7] * ndcY + reprojMat.m[11] * ndcZ + reprojMat.m[15];
                if (std::abs(clipW) < 1e-6f) clipW = 1.0f;

                float currNdcX = (reprojMat.m[0] * ndcX + reprojMat.m[4] * ndcY + reprojMat.m[8] * ndcZ + reprojMat.m[12]) / clipW;
                float currNdcY = (reprojMat.m[1] * ndcX + reprojMat.m[5] * ndcY + reprojMat.m[9] * ndcZ + reprojMat.m[13]) / clipW;

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
