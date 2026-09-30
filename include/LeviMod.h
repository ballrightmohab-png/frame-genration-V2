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
        // Camera Smoothing
        bool cameraSmoothingEnabled = true;
        float smoothTime = 0.05f; // Seconds to reach target angle
        float maxSpeed = 1000.0f; // Max angular rotation speed deg/sec

        // Frame Generation
        bool frameGenEnabled = true;
        int frameMultiplier = 2; // 2x FPS (1 interpolated frame per real frame)
        float motionVectorStrength = 1.0f;
        bool uiMaskingEnabled = true;
        bool disocclusionProtection = true;
    };

    class LEVI_API PluginMain {
    public:
        static PluginMain& getInstance();

        bool initialize();
        void shutdown();

        ModConfig& getConfig() { return m_config; }

    private:
        PluginMain() = default;
        ~PluginMain() = default;

        ModConfig m_config;
        bool m_initialized = false;
    };

} // namespace LeviMod

extern "C" LEVI_API void LeviMod_OnLoad();
extern "C" LEVI_API void LeviMod_OnUnload();

#endif // LEVI_MOD_FRAMEGEN_SMOOTH_H
