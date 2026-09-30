#ifndef FRAME_GEN_ENGINE_H
#define FRAME_GEN_ENGINE_H

#include "math/MatrixMath.h"
#include <vector>
#include <cstdint>

namespace LeviMod {

    struct FrameBufferData {
        int width = 0;
        int height = 0;
        std::vector<uint8_t> colorBuffer;
        std::vector<float> depthBuffer;
        Mat4 viewProjMatrix;
    };

    class FrameGenEngine {
    public:
        FrameGenEngine() = default;
        ~FrameGenEngine() = default;

        void initialize(int width, int height);
        void resize(int width, int height);

        // Push a newly rendered frame from the game engine
        void pushNewFrame(const uint8_t* colorPixels, const float* depthPixels, const Mat4& viewProjMatrix);

        // Generate an interpolated intermediate frame at sub-step factor t (e.g. t = 0.5)
        bool generateInterpolatedFrame(float t, std::vector<uint8_t>& outputColorBuffer);

        bool isReady() const { return m_hasPrevFrame && m_hasCurrFrame; }
        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }

    private:
        int m_width = 1920;
        int m_height = 1080;

        FrameBufferData m_prevFrame;
        FrameBufferData m_currFrame;

        bool m_hasPrevFrame = false;
        bool m_hasCurrFrame = false;
    };

} // namespace LeviMod

#endif // FRAME_GEN_ENGINE_H
