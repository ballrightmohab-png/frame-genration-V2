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
        // Camera Smoothing Settings
        bool cameraSmoothingEnabled = true;
        float smoothTime = 0.040f; // Seconds to reach target angle (40ms)
        float maxSpeed = 3600.0f;  // Max rotation speed deg/sec

        // Frame Generation Settings
        bool frameGenEnabled = true;
        int frameMultiplier = 2; // 2x FPS multiplier
        float motionVectorStrength = 1.0f;
        int targetFPSCap = 240;
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

        ModConfig m_config;
        std::string m_configPath = "config.ini";
        bool m_initialized = false;
    };

} // namespace LeviMod

extern "C" LEVI_API void LeviMod_OnLoad();
extern "C" LEVI_API void LeviMod_OnUnload();

#endif // LEVI_MOD_FRAMEGEN_SMOOTH_H
