#ifndef OVERLAY_HUD_H
#define OVERLAY_HUD_H

#include <string>
#include "RuntimeSettings.hpp"
#include "hooks/RenderHooks.hpp"

namespace LeviMod {

    struct HUDStats {
        float realFPS = 60.0f;
        float outputFPS = 120.0f;
        float frameTimeMs = 16.6f;
        std::string frameType = "REAL";
    };

    class OverlayHUD {
    public:
        static std::string renderOverlayText(const HUDStats& stats, const RuntimeSettings& runtime) {
            if (!runtime.debugOverlayEnabled.load()) return "";

            std::string text = "--- [Levi FrameGen + Camera Smooth] ---\n";
            text += "FrameGen: " + std::string(runtime.frameGenerationEnabled.load() ? "ON" : "OFF") + "\n";

            int mode = runtime.frameGenerationMode.load();
            text += "Mode: " + std::string(mode == 0 ? "Off" : (mode == 1 ? "1x" : "2x")) + "\n";

            text += "Real FPS: " + std::to_string(static_cast<int>(stats.realFPS)) + "\n";
            text += "Output FPS: " + std::to_string(static_cast<int>(stats.outputFPS)) + "\n";
            text += "Frame Time: " + std::to_string(stats.frameTimeMs).substr(0, 4) + " ms\n";
            text += "Generated Frame: " + std::string(stats.frameType == "GENERATED" ? "YES" : "NO") + "\n";
            text += "Camera Smooth: " + std::string(runtime.cameraSmoothingEnabled.load() ? "ON" : "OFF") + "\n";

            int preset = runtime.preset.load();
            std::string presetStr = (preset == 0) ? "LOW LATENCY" : ((preset == 2) ? "SMOOTHNESS" : "BALANCED");
            text += "Latency Mode: " + presetStr + "\n";

            text += "Real Frames: " + std::to_string(runtime.realFrameCount.load()) + "\n";
            text += "Generated Frames: " + std::to_string(runtime.generatedFrameCount.load()) + "\n";
            text += "Dropped FG Count: " + std::to_string(runtime.droppedFrameCount.load()) + "\n";

            return text;
        }
    };

} // namespace LeviMod

#endif // OVERLAY_HUD_H
