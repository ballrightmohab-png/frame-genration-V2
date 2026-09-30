#ifndef LEVI_MOD_FRAMEGEN_SMOOTH_H
#define LEVI_MOD_FRAMEGEN_SMOOTH_H

#include <iostream>
#include <string>
#include <memory>

#ifdef _WIN32
    #ifdef LEVI_MOD_EXPORTS
        #define LEVI_API __declspec(dllexport)
    #else
        #define LEVI_API __declspec(dllimport)
    #endif
#else
    #define LEVI_API __attribute__((visibility("default")))
#endif

namespace LeviMod {

    struct ModConfig {
        // --- 🎬 Frame Generation & Motion Estimation ---
        bool frameGenEnabled = true;
        int frameMultiplier = 2; // 2x or 3x
        bool tfrAlternatingFrames = true; // Real -> Generated -> Real -> Generated
        bool usePreviousRealFrame = true;
        int frameQueueLength = 2; // 1, 2, 3 (experimental)
        float generatedFrameStrength = 1.0f; // 0.0 - 1.0
        std::string motionEstimationQuality = "Medium"; // Low, Medium, High
        int motionSearchRange = 16;
        bool sceneChangeDetection = true;
        bool fastCameraMovementProtection = true;

        // --- 🎥 Camera Smoothing Physics ---
        bool cameraSmoothingEnabled = true;
        float smoothTime = 0.040f; // Seconds to reach target angle
        float maxSpeed = 3600.0f;  // Max rotation speed deg/sec
        float smoothingStrength = 0.80f; // 0 - 100% (0.0 to 1.0)
        std::string cameraResponse = "Smooth"; // Instant, Smooth, Cinematic
        bool mouseTouchSmoothing = true;
        bool rotationPrediction = true;
        bool adaptiveSmoothing = true;
        bool combatSmoothing = true;
        bool disableWhileAttacking = false;
        bool disableWhileInventoryOpen = true;

        // --- ⚡ Performance & Latency ---
        bool lowLatencyMode = true;
        std::string frameGenPriority = "Balanced"; // Quality, Balanced, Performance
        bool adaptiveFrameGeneration = true;
        bool thermalProtection = true;
        bool disableFgWhenFpsDrops = true;
        std::string targetRealFPS = "60"; // 30, 45, 60, 90, Auto

        // --- 🧪 Debug & Telemetry Overlays ---
        bool showRealFPS = true;
        bool showGeneratedFPS = true;
        bool showFrameTime = false;
        bool showInputLatency = false;
        bool showFrameType = true; // REAL / GENERATED
        bool motionVectorDebug = false;
        bool frameHistoryDebug = false;

        // Legacy compatibility options
        bool uiMaskingEnabled = true;
        bool disocclusionProtection = true;
        float disocclusionThreshold = 0.05f;
    };

    class LEVI_API PluginMain {
    public:
        static PluginMain& getInstance();

        bool initialize();
        void shutdown();

        bool loadConfig(const std::string& path = "config.ini");
        bool saveConfig(const std::string& path = "config.ini");

        ModConfig& getConfig() { return m_config; }

    private:
        PluginMain() = default;
        ~PluginMain() = default;

        void registerModMenuModule();

        ModConfig m_config;
        std::string m_configPath = "config.ini";
        bool m_initialized = false;
    };

} // namespace LeviMod

extern "C" LEVI_API void LeviMod_OnLoad();
extern "C" LEVI_API void LeviMod_OnUnload();

#endif // LEVI_MOD_FRAMEGEN_SMOOTH_H
