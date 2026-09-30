#ifndef TFR_FRAME_GENERATOR_HPP
#define TFR_FRAME_GENERATOR_HPP

/*
 * Levi Frame Generation - TFR prototype
 *
 * Pipeline:
 *   REAL FRAME A
 *        ↓
 *   GENERATED FRAME A→B
 *        ↓
 *   REAL FRAME B
 *        ↓
 *   GENERATED FRAME B→C
 *        ↓
 *   REAL FRAME C
 */

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <vector>

namespace levi::framegen {

struct Frame {
    int width = 0;
    int height = 0;
    std::vector<uint8_t> rgba;

    bool valid() const {
        return width > 0 && height > 0 &&
               rgba.size() == static_cast<size_t>(width) *
                              static_cast<size_t>(height) * 4;
    }

    void resize(int w, int h) {
        width = w;
        height = h;
        rgba.resize(static_cast<size_t>(w) *
                    static_cast<size_t>(h) * 4);
    }
};

struct Motion {
    float dx = 0.0f;
    float dy = 0.0f;
};

class FrameGenerator {
public:
    enum class Mode {
        Off,
        OneX
    };

    void setMode(Mode mode) {
        mode_ = mode;
    }

    void setBlend(float blend) {
        blend_ = std::clamp(blend, 0.0f, 1.0f);
    }

    void setBlockSize(int size) {
        blockSize_ = std::clamp(size, 4, 32);
    }

    void setSearchRadius(int radius) {
        searchRadius_ = std::clamp(radius, 1, 32);
    }

    /*
     * Submit a REAL frame.
     * The first real frame only establishes history.
     * Every following real frame becomes the next temporal anchor.
     */
    void submitRealFrame(const Frame& frame) {
        if (!frame.valid())
            return;

        if (!previous_.valid() ||
            previous_.width != frame.width ||
            previous_.height != frame.height) {
            previous_ = frame;
            generated_.resize(frame.width, frame.height);
            return;
        }

        current_ = frame;
    }

    bool hasPrevious() const {
        return previous_.valid();
    }

    bool hasCurrent() const {
        return current_.valid();
    }

    /*
     * Generates the in-between frame:
     * previous REAL  --->  GENERATED  --->  current REAL
     * alpha = 0.5 gives a midpoint frame.
     */
    bool generate(float alpha = 0.5f) {
        if (mode_ == Mode::Off)
            return false;

        if (!previous_.valid() || !current_.valid())
            return false;

        if (previous_.width != current_.width ||
            previous_.height != current_.height)
            return false;

        alpha = std::clamp(alpha, 0.0f, 1.0f);

        generated_.resize(previous_.width, previous_.height);

        const int w = previous_.width;
        const int h = previous_.height;

        // Estimate motion from REAL(previous) -> REAL(current).
        const std::vector<Motion> motion =
            estimateMotion(previous_, current_);

        /*
         * Warp both real frames toward the midpoint.
         * Previous frame moves forward by alpha.
         * Current frame moves backward by (1-alpha).
         */
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                const size_t index =
                    (static_cast<size_t>(y) * w + x) * 4;

                const Motion& m =
                    motion[(static_cast<size_t>(y) * w + x)];

                const float oldX =
                    static_cast<float>(x) - m.dx * alpha;
                const float oldY =
                    static_cast<float>(y) - m.dy * alpha;

                const float newX =
                    static_cast<float>(x) + m.dx * (1.0f - alpha);
                const float newY =
                    static_cast<float>(y) + m.dy * (1.0f - alpha);

                uint8_t a[4];
                uint8_t b[4];

                sampleBilinear(previous_, oldX, oldY, a);
                sampleBilinear(current_, newX, newY, b);

                const float speed =
                    std::sqrt(m.dx * m.dx + m.dy * m.dy);

                float confidence = 1.0f;
                if (speed > 1.0f) {
                    confidence =
                        std::clamp(1.0f - speed / 64.0f,
                                   0.15f, 1.0f);
                }

                const float wa = (1.0f - alpha) * confidence;
                const float wb = alpha * confidence;

                const float fallbackA = 1.0f - alpha;
                const float fallbackB = alpha;

                const float sum = wa + wb;

                const float finalA =
                    sum > 0.001f ? wa / sum : fallbackA;
                const float finalB =
                    sum > 0.001f ? wb / sum : fallbackB;

                for (int c = 0; c < 4; ++c) {
                    const float value =
                        static_cast<float>(a[c]) * finalA +
                        static_cast<float>(b[c]) * finalB;

                    generated_.rgba[index + c] =
                        static_cast<uint8_t>(
                            std::clamp(value, 0.0f, 255.0f));
                }
            }
        }

        return true;
    }

    /*
     * Promote current REAL frame to temporal history.
     */
    void commitCurrentRealFrame() {
        if (!current_.valid())
            return;

        previous_ = std::move(current_);
        current_ = Frame{};
    }

    const Frame& generatedFrame() const {
        return generated_;
    }

    const Frame& currentRealFrame() const {
        return current_;
    }

private:
    std::vector<Motion> estimateMotion(
        const Frame& a,
        const Frame& b) const
    {
        const int w = a.width;
        const int h = a.height;

        std::vector<Motion> result(
            static_cast<size_t>(w) *
            static_cast<size_t>(h));

        const int bs = blockSize_;

        for (int by = 0; by < h; by += bs) {
            for (int bx = 0; bx < w; bx += bs) {

                float bestError =
                    std::numeric_limits<float>::max();

                int bestDx = 0;
                int bestDy = 0;

                for (int dy = -searchRadius_;
                     dy <= searchRadius_; ++dy) {

                    for (int dx = -searchRadius_;
                         dx <= searchRadius_; ++dx) {

                        float error = 0.0f;
                        int samples = 0;

                        const int yEnd =
                            std::min(by + bs, h);
                        const int xEnd =
                            std::min(bx + bs, w);

                        for (int y = by;
                             y < yEnd;
                             y += 2) {

                            const int yy = y + dy;

                            if (yy < 0 || yy >= h)
                                continue;

                            for (int x = bx;
                                 x < xEnd;
                                 x += 2) {

                                const int xx = x + dx;

                                if (xx < 0 || xx >= w)
                                    continue;

                                const float lumaA =
                                    luma(a, x, y);

                                const float lumaB =
                                    luma(b, xx, yy);

                                error +=
                                    std::abs(lumaA - lumaB);

                                ++samples;
                            }
                        }

                        if (samples > 0)
                            error /= static_cast<float>(samples);

                        if (error < bestError) {
                            bestError = error;
                            bestDx = dx;
                            bestDy = dy;
                        }
                    }
                }

                for (int y = by;
                     y < std::min(by + bs, h);
                     ++y) {

                    for (int x = bx;
                         x < std::min(bx + bs, w);
                         ++x) {

                        result[
                            static_cast<size_t>(y) * w + x
                        ] = Motion{
                            static_cast<float>(bestDx),
                            static_cast<float>(bestDy)
                        };
                    }
                }
            }
        }

        return result;
    }

    static float luma(
        const Frame& frame,
        int x,
        int y)
    {
        x = std::clamp(x, 0, frame.width - 1);
        y = std::clamp(y, 0, frame.height - 1);

        const size_t i =
            (static_cast<size_t>(y) * frame.width + x) * 4;

        const float r = frame.rgba[i + 0];
        const float g = frame.rgba[i + 1];
        const float b = frame.rgba[i + 2];

        return 0.2126f * r +
               0.7152f * g +
               0.0722f * b;
    }

    static void sampleBilinear(
        const Frame& frame,
        float x,
        float y,
        uint8_t out[4])
    {
        x = std::clamp(x, 0.0f,
                       static_cast<float>(frame.width - 1));

        y = std::clamp(y, 0.0f,
                       static_cast<float>(frame.height - 1));

        const int x0 = static_cast<int>(std::floor(x));
        const int y0 = static_cast<int>(std::floor(y));

        const int x1 =
            std::min(x0 + 1, frame.width - 1);

        const int y1 =
            std::min(y0 + 1, frame.height - 1);

        const float fx = x - x0;
        const float fy = y - y0;

        for (int c = 0; c < 4; ++c) {
            const float p00 =
                pixel(frame, x0, y0, c);

            const float p10 =
                pixel(frame, x1, y0, c);

            const float p01 =
                pixel(frame, x0, y1, c);

            const float p11 =
                pixel(frame, x1, y1, c);

            const float top =
                p00 + (p10 - p00) * fx;

            const float bottom =
                p01 + (p11 - p01) * fx;

            const float value =
                top + (bottom - top) * fy;

            out[c] =
                static_cast<uint8_t>(
                    std::clamp(value, 0.0f, 255.0f));
        }
    }

    static float pixel(
        const Frame& frame,
        int x,
        int y,
        int channel)
    {
        const size_t i =
            (static_cast<size_t>(y) * frame.width + x) * 4;

        return static_cast<float>(
            frame.rgba[i + channel]);
    }

private:
    Mode mode_ = Mode::OneX;
    float blend_ = 0.5f;
    int blockSize_ = 8;
    int searchRadius_ = 8;

    Frame previous_;
    Frame current_;
    Frame generated_;
};

} // namespace levi::framegen

#endif // TFR_FRAME_GENERATOR_HPP
