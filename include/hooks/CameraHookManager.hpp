#ifndef CAMERA_HOOK_MANAGER_HPP
#define CAMERA_HOOK_MANAGER_HPP

#include "camera/CameraSmoothing.h"
#include "RuntimeSettings.hpp"
#include <iostream>

namespace LeviMod {

    struct CameraState {
        Vec3 position{0.0f, 0.0f, 0.0f};
        CameraRotation rotation{0.0f, 0.0f, 0.0f};
    };

    class CameraHookManager {
    public:
        static CameraHookManager& getInstance() {
            static CameraHookManager instance;
            return instance;
        }

        bool install() {
            if (m_installed) return true;
            std::cout << "[CameraHooks] Installing Bedrock camera transform hooks..." << std::endl;
            m_installed = true;
            return true;
        }

        void uninstall() {
            if (!m_installed) return;
            std::cout << "[CameraHooks] Uninstalling camera transform hooks..." << std::endl;
            m_installed = false;
        }

        // Frame-rate independent delta-time camera smoothing hook
        CameraState onCameraTransformUpdate(const CameraState& targetCamera, float deltaTime, const RuntimeSettings& runtime) {
            if (!runtime.cameraSmoothingEnabled.load() || !runtime.masterEnabled.load()) {
                m_previousCamera = targetCamera;
                m_smoothedCamera = targetCamera;
                return targetCamera;
            }

            float strength = runtime.cameraSmoothingStrength.load(); // 0.0 to 1.0
            if (strength <= 0.001f) {
                m_previousCamera = targetCamera;
                m_smoothedCamera = targetCamera;
                return targetCamera;
            }

            // Convert strength [0, 1] to frame-rate independent time window smoothTime (10ms to 100ms)
            float smoothTime = 0.010f + (1.0f - strength) * 0.090f;
            m_cameraSmoother.setSmoothTime(smoothTime);

            m_smoothedCamera.rotation = m_cameraSmoother.updateRotation(targetCamera.rotation, deltaTime);
            m_smoothedCamera.position = m_cameraSmoother.updatePosition(targetCamera.position, deltaTime);

            m_previousCamera = m_smoothedCamera;
            return m_smoothedCamera;
        }

        const CameraState& getSmoothedCamera() const { return m_smoothedCamera; }
        const CameraState& getPreviousCamera() const { return m_previousCamera; }

    private:
        CameraHookManager() = default;

        CameraSmoothing m_cameraSmoother;
        CameraState m_previousCamera;
        CameraState m_smoothedCamera;
        bool m_installed = false;
    };

} // namespace LeviMod

#endif // CAMERA_HOOK_MANAGER_HPP
