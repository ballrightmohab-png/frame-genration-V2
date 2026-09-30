#ifndef OVERLAY_HUD_H
#define OVERLAY_HUD_H

#include <string>
#include "LeviMod.h"

namespace LeviMod {

    struct HUDStats {
        float baseFPS = 60.0f;
        float generatedFPS = 120.0f;
        float frameTimeMs = 16.6f;
        float inputLatencyMs = 8.2f;
        std::string currentFrameType = "REAL"; // REAL or GENERATED
    };

    class OverlayHUD {
    public:
        static std::string renderOverlayText(const HUDStats& stats, const ModConfig& config) {
            std::string text = "--- [Levi FrameGen & Camera Smooth] ---\n";

            if (config.showRealFPS) {
                text += "Base Real FPS: " + std::to_string(static_cast<int>(stats.baseFPS)) + " FPS\n";
            }
            if (config.showGeneratedFPS) {
                text += "Display FPS (FG): " + std::to_string(static_cast<int>(stats.generatedFPS)) + " FPS\n";
            }
            if (config.showFrameTime) {
                text += "Frame Time: " + std::to_string(stats.frameTimeMs).substr(0, 5) + " ms\n";
            }
            if (config.showInputLatency) {
                text += "Input Latency: " + std::to_string(stats.inputLatencyMs).substr(0, 4) + " ms\n";
            }
            if (config.showFrameType) {
                text += "Frame Type: [" + stats.currentFrameType + "]\n";
            }

            text += "Frame Gen Mode: " + std::string(config.frameGenEnabled ? "ON (" + config.frameGenPriority + " / " + config.motionEstimationQuality + " Quality)" : "OFF") + "\n";
            text += "Camera Response: " + config.cameraResponse + " (" + std::to_string(static_cast<int>(config.smoothingStrength * 100.0f)) + "% Strength)\n";

            if (config.motionVectorDebug) {
                text += " [DEBUG] Motion Vectors: Active | Range: " + std::to_string(config.motionSearchRange) + "px\n";
            }
            if (config.frameHistoryDebug) {
                text += " [DEBUG] Queue Length: " + std::to_string(config.frameQueueLength) + " frames\n";
            }

            return text;
        }
    };

} // namespace LeviMod

#endif // OVERLAY_HUD_H
