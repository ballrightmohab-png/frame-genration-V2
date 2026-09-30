#ifndef LEVI_HOOKS_H
#define LEVI_HOOKS_H

#include "hooks/CameraHookManager.hpp"
#include "hooks/RenderHooks.hpp"
#include "RuntimeSettings.hpp"
#include <iostream>

namespace LeviMod {

    class LeviHookManager {
    public:
        static LeviHookManager& getInstance() {
            static LeviHookManager instance;
            return instance;
        }

        void syncConfigToEngine() {
            // Unused stub kept for interface compatibility
        }

        bool installHooks() {
            std::cout << "[LeviHooks] Installing Bedrock Render & Camera hooks..." << std::endl;
            bool camOk = CameraHookManager::getInstance().install();
            bool renderOk = RenderHooks::getInstance().install();
            return camOk && renderOk;
        }

        void uninstallHooks() {
            std::cout << "[LeviHooks] Uninstalling Bedrock hooks..." << std::endl;
            CameraHookManager::getInstance().uninstall();
            RenderHooks::getInstance().uninstall();
        }

        CameraHookManager& getCameraHooks() { return CameraHookManager::getInstance(); }
        RenderHooks& getRenderHooks() { return RenderHooks::getInstance(); }

    private:
        LeviHookManager() = default;
    };

} // namespace LeviMod

#endif // LEVI_HOOKS_H
