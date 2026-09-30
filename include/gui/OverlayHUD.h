#ifndef OVERLAY_HUD_H
#define OVERLAY_HUD_H

#include <string>

namespace LeviMod {

    struct HUDStats {
        float baseFPS = 60.0f;
        float generatedFPS = 120.0f;
        bool frameGenActive = true;
        bool cameraSmoothingActive = true;
        float smoothTime = 0.04f;
    };

    class OverlayHUD {
    public:
        static std::string renderOverlayText(const HUDStats& stats) {
            std::string text = "--- [Levi FrameGen & Camera Smooth] ---\n";
            text += "Base Game FPS: " + std::to_string(static_cast<int>(stats.baseFPS)) + "\n";
            text += "Display FPS (FrameGen): " + std::to_string(static_cast<int>(stats.generatedFPS)) + " FPS\n";
            text += "Frame Generation: " + std::string(stats.frameGenActive ? "ON (2x Motion Reprojection)" : "OFF") + "\n";
            text += "Camera Damping: " + std::string(stats.cameraSmoothingActive ? "ON (" + std::to_string(static_cast<int>(stats.smoothTime * 1000)) + "ms)" : "OFF") + "\n";
            return text;
        }
    };

} // namespace LeviMod

#endif // OVERLAY_HUD_H
