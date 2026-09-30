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
        std::cout << "[LeviFrameGenSmooth] Registering module and settings in pl::modmenu..." << std::endl;

        pl::modmenu::ModuleBuilder builder("framegen.core", "Frame Generator & V-Sync");

        builder.description("Enhances game performance with frame multiplication (2x/3x FPS) and V-Sync disabling for uncapped framerates.")
               .modId("framegen.core")
               .defaultEnabled(m_config.frameGenEnabled)
               .hideInHudEditor(true);

        // Add Config entries to Mod Menu
        builder.config("frameMultiplier", "Frame Multiplier", pl::modmenu::ConfigType::INT, "2", "2", "3", std::to_string(m_config.frameMultiplier));
        builder.config("cameraSmoothingEnabled", "Camera Smoothing", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.cameraSmoothingEnabled ? "true" : "false");
        builder.config("smoothTime", "Camera Damping Time (s)", pl::modmenu::ConfigType::FLOAT, "0.040", "0.010", "0.100", std::to_string(m_config.smoothTime));
        builder.config("motionVectorStrength", "Motion Vector Strength", pl::modmenu::ConfigType::FLOAT, "1.0", "0.1", "2.0", std::to_string(m_config.motionVectorStrength));
        builder.config("uiMaskingEnabled", "UI Masking Protection", pl::modmenu::ConfigType::BOOLEAN, "true", "", "", m_config.uiMaskingEnabled ? "true" : "false");

        // Set callbacks
        builder.onToggle([this](std::string_view modId, bool enabled) {
            std::cout << "[pl::modmenu Callback] Module Toggle " << modId << ": " << (enabled ? "ENABLED" : "DISABLED") << std::endl;
            m_config.frameGenEnabled = enabled;
            LeviHookManager::getInstance().syncConfigToEngine();
        });

        builder.onConfigChanged([this](std::string_view key, std::string_view value, std::string_view oldValue) {
            std::cout << "[pl::modmenu Callback] Setting Changed " << key << " = " << value << " (was " << oldValue << ")" << std::endl;

            if (key == "frameMultiplier") {
                m_config.frameMultiplier = std::stoi(std::string(value));
            } else if (key == "cameraSmoothingEnabled") {
                m_config.cameraSmoothingEnabled = (value == "true" || value == "1");
            } else if (key == "smoothTime") {
                m_config.smoothTime = std::stof(std::string(value));
            } else if (key == "motionVectorStrength") {
                m_config.motionVectorStrength = std::stof(std::string(value));
            } else if (key == "uiMaskingEnabled") {
                m_config.uiMaskingEnabled = (value == "true" || value == "1");
            }

            LeviHookManager::getInstance().syncConfigToEngine();
            saveConfig("config.ini");
        });

        builder.registerModule();
        std::cout << "[LeviFrameGenSmooth] Registered pl::modmenu settings successfully!" << std::endl;
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

        // Register with pl::modmenu so all settings appear inside the in-game mod menu
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
