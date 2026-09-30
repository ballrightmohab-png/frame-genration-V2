#include "FrameGenMod.hpp"
#include <iostream>

namespace LeviMod {

    FrameGenMod::FrameGenMod()
        : m_configFile("config.ini", FrameGenConfig{}) {}

    bool FrameGenMod::load() {
        std::cout << "[FrameGenMod] Loading module " << ModuleId << "..." << std::endl;
        if (m_configFile.load()) {
            std::cout << "[FrameGenMod] Loaded config settings successfully." << std::endl;
        } else {
            std::cout << "[FrameGenMod] Creating initial default config..." << std::endl;
            m_configFile.save();
        }
        m_configFile.get().syncToRuntime(m_runtime);
        return true;
    }

    bool FrameGenMod::unload() {
        std::cout << "[FrameGenMod] Unloading module..." << std::endl;
        saveConfig();
        return true;
    }

    bool FrameGenMod::enable() {
        if (m_enabled) return true;
        std::cout << "[FrameGenMod] Enabling Frame Generation & Camera Smoothing..." << std::endl;

        registerModMenu();

        if (!LeviHookManager::getInstance().installHooks()) {
            std::cerr << "[FrameGenMod] Failed to install render/camera hooks!" << std::endl;
            unregisterModMenu();
            return false;
        }

        m_enabled = true;
        return true;
    }

    bool FrameGenMod::disable() {
        if (!m_enabled) return true;
        std::cout << "[FrameGenMod] Disabling module and uninstalling hooks..." << std::endl;

        LeviHookManager::getInstance().uninstallHooks();
        unregisterModMenu();
        saveConfig();

        m_enabled = false;
        return true;
    }

    void FrameGenMod::saveConfig() {
        m_configFile.get().syncFromRuntime(m_runtime);
        m_configFile.save();
    }

    void FrameGenMod::registerModMenu() {
        std::cout << "[FrameGenMod] Registering pl::modmenu UI controls..." << std::endl;

        pl::modmenu::ModuleBuilder builder(ModuleId, "Frame Generation + Camera Smoothing");

        builder.description("Frame generation, temporal interpolation and camera smoothing for Bedrock.")
               .modId(ModuleId)
               .defaultEnabled(m_runtime.masterEnabled.load());

        // --- FRAME GENERATION CONTROLS ---
        builder.config("frameGeneration", "Frame Generation", pl::modmenu::ConfigType::BOOLEAN, "true", "", "",
                       m_runtime.frameGenerationEnabled.load() ? "true" : "false");

        builder.config("frameGenerationMode", "Frame Generation Mode (0=Off, 1=1x, 2=2x)", pl::modmenu::ConfigType::CHOICE, "1", "0", "2",
                       std::to_string(m_runtime.frameGenerationMode.load()));

        builder.config("generatedFrameStrength", "Generated Frame Strength", pl::modmenu::ConfigType::FLOAT, "1.0", "0.0", "1.0",
                       std::to_string(m_runtime.generatedFrameStrength.load()));

        builder.config("preset", "Preset (0=Low Latency, 1=Balanced, 2=Smoothness)", pl::modmenu::ConfigType::CHOICE, "1", "0", "2",
                       std::to_string(m_runtime.preset.load()));

        builder.config("adaptiveMode", "Adaptive Mode", pl::modmenu::ConfigType::BOOLEAN, "true", "", "",
                       m_runtime.adaptiveMode.load() ? "true" : "false");

        builder.config("lowLatency", "Low Latency Mode", pl::modmenu::ConfigType::BOOLEAN, "false", "", "",
                       m_runtime.lowLatencyMode.load() ? "true" : "false");

        // --- CAMERA CONTROLS ---
        builder.config("cameraSmoothing", "Camera Smoothing", pl::modmenu::ConfigType::BOOLEAN, "true", "", "",
                       m_runtime.cameraSmoothingEnabled.load() ? "true" : "false");

        builder.config("smoothingStrength", "Smoothing Strength", pl::modmenu::ConfigType::FLOAT, "0.5", "0.0", "1.0",
                       std::to_string(m_runtime.cameraSmoothingStrength.load()));

        // --- DEBUG CONTROLS ---
        builder.config("debugOverlay", "Debug Overlay", pl::modmenu::ConfigType::BOOLEAN, "true", "", "",
                       m_runtime.debugOverlayEnabled.load() ? "true" : "false");

        // Callbacks
        builder.onToggle([this](std::string_view id, bool enabled) {
            std::cout << "[pl::modmenu] Master Toggle " << id << " = " << (enabled ? "ON" : "OFF") << std::endl;
            m_runtime.masterEnabled.store(enabled);
            m_runtime.frameGenerationEnabled.store(enabled);
        });

        builder.onConfigChanged([this](std::string_view key, std::string_view val, std::string_view oldVal) {
            std::cout << "[pl::modmenu] Config Changed: " << key << " = " << val << std::endl;
            std::string sVal(val);
            bool bVal = (val == "true" || val == "1");

            if (key == "frameGeneration") {
                m_runtime.frameGenerationEnabled.store(bVal);
            } else if (key == "frameGenerationMode") {
                m_runtime.frameGenerationMode.store(std::stoi(sVal));
            } else if (key == "generatedFrameStrength") {
                m_runtime.generatedFrameStrength.store(std::stof(sVal));
            } else if (key == "preset") {
                int p = std::stoi(sVal);
                m_runtime.preset.store(p);
                // Apply preset defaults
                if (p == 0) { // Low Latency
                    m_runtime.lowLatencyMode.store(true);
                    m_runtime.cameraSmoothingStrength.store(0.2f);
                } else if (p == 2) { // Smoothness
                    m_runtime.lowLatencyMode.store(false);
                    m_runtime.cameraSmoothingStrength.store(0.8f);
                } else { // Balanced
                    m_runtime.lowLatencyMode.store(false);
                    m_runtime.cameraSmoothingStrength.store(0.5f);
                }
            } else if (key == "adaptiveMode") {
                m_runtime.adaptiveMode.store(bVal);
            } else if (key == "lowLatency") {
                m_runtime.lowLatencyMode.store(bVal);
            } else if (key == "cameraSmoothing") {
                m_runtime.cameraSmoothingEnabled.store(bVal);
            } else if (key == "smoothingStrength") {
                m_runtime.cameraSmoothingStrength.store(std::stof(sVal));
            } else if (key == "debugOverlay") {
                m_runtime.debugOverlayEnabled.store(bVal);
            }

            saveConfig();
        });

        builder.registerModule();
    }

    void FrameGenMod::unregisterModMenu() {
        std::cout << "[FrameGenMod] Unregistering pl::modmenu controls..." << std::endl;
        pl::modmenu::unregisterModule(ModuleId);
    }

} // namespace LeviMod
