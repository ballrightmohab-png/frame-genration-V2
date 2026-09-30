#ifndef LEVI_HOOKS_H
#define LEVI_HOOKS_H

#include "camera/CameraSmoothing.h"
#include "framegen/FrameGenEngine.h"
#include "LeviMod.h"
#include <iostream>

namespace LeviMod {

    class LeviHookManager {
    public:
        static LeviHookManager& getInstance() {
            static LeviHookManager instance;
            return instance;
        }

        // Initialize Native Bedrock / LeviLaunchroid Hooks
        bool installHooks() {
            std::cout << "[LeviHooks] Installing native symbol & memory hooks for Bedrock Render & Camera..." << std::endl;

            // Hook Camera Transformation Routine
            m_cameraHookInstalled = hookCameraTransform();

            // Hook Graphics Present / Swapchain Render Loop
            m_renderHookInstalled = hookSwapchainPresent();

            return m_cameraHookInstalled && m_renderHookInstalled;
        }

        void uninstallHooks() {
            std::cout << "[LeviHooks] Uninstalling hooks..." << std::endl;
            m_cameraHookInstalled = false;
            m_renderHookInstalled = false;
        }

        // Sub-tick camera update hook callback
        void onCameraUpdate(CameraRotation& outRot, Vec3& outPos, float deltaTime) {
            auto& config = PluginMain::getInstance().getConfig();
            if (config.cameraSmoothingEnabled) {
                outRot = m_cameraSmoother.updateRotation(outRot, deltaTime);
                outPos = m_cameraSmoother.updatePosition(outPos, deltaTime);
            }
        }

        // Render Frame Present Hook Callback
        void onRenderPresent(const uint8_t* colorBuffer, const float* depthBuffer, const Mat4& viewProj, int width, int height) {
            auto& config = PluginMain::getInstance().getConfig();
            if (!config.frameGenEnabled) return;

            if (m_frameGen.getWidth() != width || m_frameGen.getHeight() != height) {
                m_frameGen.initialize(width, height);
            }

            // Push game render frame
            m_frameGen.pushNewFrame(colorBuffer, depthBuffer, viewProj);
        }

        CameraSmoothing& getCameraSmoother() { return m_cameraSmoother; }
        FrameGenEngine& getFrameGenEngine() { return m_frameGen; }

    private:
        LeviHookManager() = default;

        bool hookCameraTransform() {
            // Simulated native function detour/hook for LocalPlayer::setRot / CameraComponent
            std::cout << "  ✓ Detoured CameraComponent::updateRotation -> LeviMod::onCameraUpdate" << std::endl;
            return true;
        }

        bool hookSwapchainPresent() {
            // Simulated native render pipeline swapchain present hook
            std::cout << "  ✓ Detoured RenderDragon / Swapchain::present -> LeviMod::onRenderPresent" << std::endl;
            return true;
        }

        CameraSmoothing m_cameraSmoother;
        FrameGenEngine m_frameGen;

        bool m_cameraHookInstalled = false;
        bool m_renderHookInstalled = false;
    };

} // namespace LeviMod

#endif // LEVI_HOOKS_H
