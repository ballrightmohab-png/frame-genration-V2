#include "FrameGenMod.hpp"
#include "hooks/CameraHookManager.hpp"
#include "hooks/RenderHooks.hpp"
#include "gui/OverlayHUD.h"
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
    std::cout << "========================================================\n";
    std::cout << "  🚀 LAUNCHING LEVILAUNCHROID NATIVE MOD RUNNER 🚀      \n";
    std::cout << "========================================================\n";

    auto& mod = LeviMod::FrameGenMod::getInstance();
    mod.load();
    mod.enable();

    auto& runtime = mod.getRuntime();
    auto& cameraHooks = LeviMod::CameraHookManager::getInstance();
    auto& renderHooks = LeviMod::RenderHooks::getInstance();

    std::cout << "\n--- Testing Frame Generation Pipeline (REAL -> GENERATED) ---" << std::endl;

    int w = 640, h = 360;
    std::vector<uint8_t> realPixels(w * h * 4, 100);
    std::vector<float> depthPixels(w * h, 0.5f);
    LeviMod::Mat4 viewProj = LeviMod::Mat4::identity();

    LeviMod::CameraState targetCamera;
    targetCamera.position = {0.0f, 64.0f, 0.0f};

    std::vector<uint8_t> generatedPixels;

    for (int step = 1; step <= 8; ++step) {
        // Update camera position
        targetCamera.rotation.yaw += 10.0f;
        LeviMod::CameraState smoothedCamera = cameraHooks.onCameraTransformUpdate(targetCamera, 0.016f, runtime);

        // Process Render Present
        bool isGenerated = renderHooks.onRenderPresent(realPixels.data(), depthPixels.data(), viewProj, w, h, runtime, generatedPixels);

        std::cout << "Step #" << step
                  << " | Display Frame: " << (isGenerated ? "⚡ [GENERATED]" : "🎥 [REAL]")
                  << " | Smoothed Yaw: " << smoothedCamera.rotation.yaw << "°\n";
    }

    std::cout << "\n--- Debug Telemetry HUD Output ---" << std::endl;
    LeviMod::HUDStats stats;
    stats.realFPS = 60.0f;
    stats.outputFPS = 120.0f;
    stats.frameTimeMs = 8.33f;
    stats.frameType = (renderHooks.getLastFrameType() == LeviMod::FrameType::GENERATED ? "GENERATED" : "REAL");

    std::cout << LeviMod::OverlayHUD::renderOverlayText(stats, runtime) << std::endl;

    mod.disable();
    mod.unload();

    std::cout << "✅ Native LeviLaunchroid Mod Execution Completed Successfully!\n";
    return 0;
}
