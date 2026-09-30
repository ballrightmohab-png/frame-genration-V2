#include "LeviMod.h"
#include "config/ConfigManager.h"
#include "hooks/LeviHooks.h"
#include "pl/ModMenu.hpp"
#include <iostream>

namespace LeviMod {

    PluginMain& PluginMain::getInstance() {
        static PluginMain instance;
        return instance;
    }

    void PluginMain::registerModMenuModule() {
        std::cout << "[LeviFrameGenSmooth] Registering expanded module and settings in pl::modmenu..." << std::endl;

        pl::modmenu::ModuleBuilder builder("framegen.core", "Frame Generator & V-Sync");

        builder.description("Enhances game performance with frame multiplication (2x/3x FPS) and V-Sync disabling for uncapped framerates.")
               .modId("framegen.core")
               .defaultEnabled(m_config.frameGenEnabled)
               .hideInHudEditor(true);

        // --- 🎬 Frame Generation & Motion Estimation ---
        builder.config("frameGenEnabled", "Frame Generation", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.frameGenEnabled ? "true" : "false");
        builder.config("tfrAlternatingFrames", "TFR / Alternating Frames", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.tfrAlternatingFrames ? "true" : "false");
        builder.config("usePreviousRealFrame", "Use Previous Real Frame", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.usePreviousRealFrame ? "true" : "false");
        builder.config("frameQueueLength", "Frame Queue Length", pl::modmenu::ConfigType::INT, "2", "1", "3", std::to_string(m_config.frameQueueLength));
        builder.config("generatedFrameStrength", "Generated Frame Strength", pl::modmenu::ConfigType::FLOAT, "1.0", "0.0", "1.0", std::to_string(m_config.generatedFrameStrength));
        builder.config("motionEstimationQuality", "Motion Estimation Quality", pl::modmenu::ConfigType::CHOICE, "Medium", "Low", "High", m_config.motionEstimationQuality);
        builder.config("motionSearchRange", "Motion Search Range", pl::modmenu::ConfigType::INT, "16", "8", "32", std::to_string(m_config.motionSearchRange));
        builder.config("sceneChangeDetection", "Scene Change Detection", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.sceneChangeDetection ? "true" : "false");
        builder.config("fastCameraMovementProtection", "Fast Camera-Movement Protection", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.fastCameraMovementProtection ? "true" : "false");

        // --- 🎥 Camera Smoothing ---
        builder.config("cameraSmoothingEnabled", "Camera Smoothing", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.cameraSmoothingEnabled ? "true" : "false");
        builder.config("smoothingStrength", "Smoothing Strength", pl::modmenu::ConfigType::FLOAT, "80", "0", "100", std::to_string(static_cast<int>(m_config.smoothingStrength * 100.0f)));
        builder.config("cameraResponse", "Camera Response", pl::modmenu::ConfigType::CHOICE, "Smooth", "Instant", "Cinematic", m_config.cameraResponse);
        builder.config("mouseTouchSmoothing", "Mouse/Touch Smoothing", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.mouseTouchSmoothing ? "true" : "false");
        builder.config("rotationPrediction", "Rotation Prediction", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.rotationPrediction ? "true" : "false");
        builder.config("adaptiveSmoothing", "Adaptive Smoothing", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.adaptiveSmoothing ? "true" : "false");
        builder.config("combatSmoothing", "Combat Smoothing", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.combatSmoothing ? "true" : "false");
        builder.config("disableWhileAttacking", "Disable While Attacking", pl::modmenu::ConfigType::BOOLEAN, "false", "", "", m_config.disableWhileAttacking ? "true" : "false");
        builder.config("disableWhileInventoryOpen", "Disable While Inventory Open", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.disableWhileInventoryOpen ? "true" : "false");

        // --- ⚡ Performance / Latency ---
        builder.config("lowLatencyMode", "Low Latency Mode", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.lowLatencyMode ? "true" : "false");
        builder.config("frameGenPriority", "Frame Generation Priority", pl::modmenu::ConfigType::CHOICE, "Balanced", "Performance", "Quality", m_config.frameGenPriority);
        builder.config("adaptiveFrameGeneration", "Adaptive Frame Generation", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.adaptiveFrameGeneration ? "true" : "false");
        builder.config("thermalProtection", "Thermal Protection", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.thermalProtection ? "true" : "false");
        builder.config("disableFgWhenFpsDrops", "Disable FG When FPS Drops", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.disableFgWhenFpsDrops ? "true" : "false");
        builder.config("targetRealFPS", "Target Real FPS", pl::modmenu::ConfigType::CHOICE, "60", "30", "Auto", m_config.targetRealFPS);

        // --- 🧪 Debug ---
        builder.config("showRealFPS", "Show Real FPS", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.showRealFPS ? "true" : "false");
        builder.config("showGeneratedFPS", "Show Generated FPS", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.showGeneratedFPS ? "true" : "false");
        builder.config("showFrameTime", "Show Frame Time", pl::modmenu::ConfigType::BOOLEAN, "false", "", "", m_config.showFrameTime ? "true" : "false");
        builder.config("showInputLatency", "Show Input Latency", pl::modmenu::ConfigType::BOOLEAN, "false", "", "", m_config.showInputLatency ? "true" : "false");
        builder.config("showFrameType", "Show Frame Type (REAL/GENERATED)", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.showFrameType ? "true" : "false");
        builder.config("motionVectorDebug", "Motion Vector Debug", pl::modmenu::ConfigType::BOOLEAN, "false", "", "", m_config.motionVectorDebug ? "true" : "false");
        builder.config("frameHistoryDebug", "Frame History Debug", pl::modmenu::ConfigType::BOOLEAN, "false", "", "", m_config.frameHistoryDebug ? "true" : "false");

        // Event callbacks
        builder.onToggle([this](std::string_view modId, bool enabled) {
            std::cout << "[pl::modmenu Callback] Module Toggle " << modId << ": " << (enabled ? "ENABLED" : "DISABLED") << std::endl;
            m_config.frameGenEnabled = enabled;
            LeviHookManager::getInstance().syncConfigToEngine();
        });

        builder.onConfigChanged([this](std::string_view key, std::string_view value, std::string_view oldValue) {
            std::cout << "[pl::modmenu Callback] Setting Changed " << key << " = " << value << " (was " << oldValue << ")" << std::endl;

            std::string sVal(value);
            bool bVal = (value == "true" || value == "1");

            if (key == "frameGenEnabled") m_config.frameGenEnabled = bVal;
            else if (key == "tfrAlternatingFrames") m_config.tfrAlternatingFrames = bVal;
            else if (key == "usePreviousRealFrame") m_config.usePreviousRealFrame = bVal;
            else if (key == "frameQueueLength") m_config.frameQueueLength = std::stoi(sVal);
            else if (key == "generatedFrameStrength") m_config.generatedFrameStrength = std::stof(sVal);
            else if (key == "motionEstimationQuality") m_config.motionEstimationQuality = sVal;
            else if (key == "motionSearchRange") m_config.motionSearchRange = std::stoi(sVal);
            else if (key == "sceneChangeDetection") m_config.sceneChangeDetection = bVal;
            else if (key == "fastCameraMovementProtection") m_config.fastCameraMovementProtection = bVal;

            else if (key == "cameraSmoothingEnabled") m_config.cameraSmoothingEnabled = bVal;
            else if (key == "smoothingStrength") {
                float str = std::stof(sVal);
                m_config.smoothingStrength = (str > 1.0f) ? str / 100.0f : str;
            }
            else if (key == "cameraResponse") m_config.cameraResponse = sVal;
            else if (key == "mouseTouchSmoothing") m_config.mouseTouchSmoothing = bVal;
            else if (key == "rotationPrediction") m_config.rotationPrediction = bVal;
            else if (key == "adaptiveSmoothing") m_config.adaptiveSmoothing = bVal;
            else if (key == "combatSmoothing") m_config.combatSmoothing = bVal;
            else if (key == "disableWhileAttacking") m_config.disableWhileAttacking = bVal;
            else if (key == "disableWhileInventoryOpen") m_config.disableWhileInventoryOpen = bVal;

            else if (key == "lowLatencyMode") m_config.lowLatencyMode = bVal;
            else if (key == "frameGenPriority") m_config.frameGenPriority = sVal;
            else if (key == "adaptiveFrameGeneration") m_config.adaptiveFrameGeneration = bVal;
            else if (key == "thermalProtection") m_config.thermalProtection = bVal;
            else if (key == "disableFgWhenFpsDrops") m_config.disableFgWhenFpsDrops = bVal;
            else if (key == "targetRealFPS") m_config.targetRealFPS = sVal;

            else if (key == "showRealFPS") m_config.showRealFPS = bVal;
            else if (key == "showGeneratedFPS") m_config.showGeneratedFPS = bVal;
            else if (key == "showFrameTime") m_config.showFrameTime = bVal;
            else if (key == "showInputLatency") m_config.showInputLatency = bVal;
            else if (key == "showFrameType") m_config.showFrameType = bVal;
            else if (key == "motionVectorDebug") m_config.motionVectorDebug = bVal;
            else if (key == "frameHistoryDebug") m_config.frameHistoryDebug = bVal;

            LeviHookManager::getInstance().syncConfigToEngine();
            saveConfig("config.ini");
        });

        builder.registerModule();
        std::cout << "[LeviFrameGenSmooth] Registered all 30+ pl::modmenu settings successfully!" << std::endl;
    }

    bool PluginMain::loadConfig(const std::string& path) {
        m_configPath = path;
        if (ConfigManager::loadFromFile(m_configPath, m_config)) {
            std::cout << "[LeviFrameGenSmooth] Loaded settings from " << m_configPath << std::endl;
            LeviHookManager::getInstance().syncConfigToEngine();
            return true;
        } else {
            std::cout << "[LeviFrameGenSmooth] Creating default config file: " << m_configPath << std::endl;
            saveConfig(m_configPath);
            return false;
        }
    }

    bool PluginMain::saveConfig(const std::string& path) {
        std::string targetPath = path.empty() ? m_configPath : path;
        if (ConfigManager::saveToFile(targetPath, m_config)) {
            std::cout << "[LeviFrameGenSmooth] Saved settings to " << targetPath << std::endl;
            LeviHookManager::getInstance().syncConfigToEngine();
            return true;
        }
        return false;
    }

    bool PluginMain::initialize() {
        if (m_initialized) return true;

        std::cout << "[LeviFrameGenSmooth] Initializing Frame Generation & Camera Smoothing Mod for Bedrock/LeviLaunchroid..." << std::endl;
        loadConfig("config.ini");

        // Register all mod settings in pl::modmenu
        registerModMenuModule();

        if (LeviHookManager::getInstance().installHooks()) {
            std::cout << "[LeviFrameGenSmooth] Native LeviLaunchroid hooks installed successfully!" << std::endl;
        } else {
            std::cerr << "[LeviFrameGenSmooth] Failed to install native hooks!" << std::endl;
            return false;
        }

        m_initialized = true;
        std::cout << "[LeviFrameGenSmooth] Plugin initialized successfully!" << std::endl;
        return true;
    }

    void PluginMain::shutdown() {
        if (!m_initialized) return;
        std::cout << "[LeviFrameGenSmooth] Saving config and unregistering pl::modmenu..." << std::endl;
        pl::modmenu::unregisterModule("framegen.core");
        saveConfig(m_configPath);
        LeviHookManager::getInstance().uninstallHooks();
        m_initialized = false;
    }

} // namespace LeviMod

extern "C" LEVI_API void LeviMod_OnLoad() {
    LeviMod::PluginMain::getInstance().initialize();
}

extern "C" LEVI_API void LeviMod_OnUnload() {
    LeviMod::PluginMain::getInstance().shutdown();
}
