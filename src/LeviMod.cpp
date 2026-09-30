#include "LeviMod.h"
#include "config/ConfigManager.h"
#include "hooks/LeviHooks.h"
#include <iostream>

namespace LeviMod {

    PluginMain& PluginMain::getInstance() {
        static PluginMain instance;
        return instance;
    }

    bool PluginMain::loadConfig(const std::string& path) {
        m_configPath = path;
        if (ConfigManager::loadFromFile(m_configPath, m_config)) {
            std::cout << "[LeviFrameGenSmooth] Loaded settings from " << m_configPath << std::endl;
            // Sync settings to hooks & physics
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

        // Load configuration
        loadConfig("config.ini");

        // Install LeviLaunchroid Bedrock hooks
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
        std::cout << "[LeviFrameGenSmooth] Saving config and shutting down plugin..." << std::endl;
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
