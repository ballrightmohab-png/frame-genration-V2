#include "LeviMod.h"
#include "hooks/LeviHooks.h"
#include <iostream>

namespace LeviMod {

    PluginMain& PluginMain::getInstance() {
        static PluginMain instance;
        return instance;
    }

    bool PluginMain::initialize() {
        if (m_initialized) return true;

        std::cout << "[LeviFrameGenSmooth] Initializing Frame Generation & Camera Smoothing Mod for Bedrock/LeviLaunchroid..." << std::endl;

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
        std::cout << "[LeviFrameGenSmooth] Shutting down plugin..." << std::endl;
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
