#include "LeviMod.h"
#include "hooks/LeviHooks.h"
#include "gui/ModMenu.h"
#include "gui/OverlayHUD.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>

int main(int argc, char** argv) {
    std::cout << "========================================================\n";
    std::cout << "  🚀 LAUNCHING LEVILAUNCHROID FRAME GEN & SMOOTH MOD 🚀 \n";
    std::cout << "========================================================\n";

    // 1. Initialize LeviLaunchroid Mod Core
    if (!LeviMod::PluginMain::getInstance().initialize()) {
        std::cerr << "Failed to initialize mod!" << std::endl;
        return 1;
    }

    auto& hookManager = LeviMod::LeviHookManager::getInstance();
    LeviMod::ModMenu modMenu;

    // 2. Interactive Mod Menu Controls Demo
    std::cout << "\n--- Initializing Mod Menu Controls ---" << std::endl;
    modMenu.renderMenu();

    std::cout << "\n> Toggling Frame Generation setting in Mod Menu..." << std::endl;
    modMenu.toggleSelected(); // Toggle Frame Gen
    modMenu.renderMenu();

    std::cout << "\n> Navigating Mod Menu to Camera Damping..." << std::endl;
    modMenu.selectNext(); // Frame Multiplier
    modMenu.selectNext(); // Camera Smoothing
    modMenu.selectNext(); // Camera Damping Time
    modMenu.toggleSelected(); // Adjust damping time
    modMenu.renderMenu();

    // 3. Live Bedrock Game Simulation with Frame Generation & Camera Damping
    std::cout << "\n========================================================\n";
    std::cout << "  🎥 STARTING LIVE GAME SIMULATION & FRAME GENERATION   \n";
    std::cout << "========================================================\n";

    int renderWidth = 1280;
    int renderHeight = 720;
    hookManager.getFrameGenEngine().initialize(renderWidth, renderHeight);

    LeviMod::CameraRotation targetRot = {0.0f, 0.0f, 0.0f};
    LeviMod::Vec3 targetPos = {0.0f, 64.0f, 0.0f};
    hookManager.getCameraSmoother().reset(targetRot, targetPos);

    // Simulated Game Loop
    float baseGameFPS = 60.0f;
    float dt = 1.0f / baseGameFPS;

    std::vector<uint8_t> framePixels(renderWidth * renderHeight * 4, 120);
    std::vector<float> depthPixels(renderWidth * renderHeight, 0.5f);

    for (int frameIndex = 1; frameIndex <= 10; ++frameIndex) {
        // Player moves mouse fast (sweeping yaw towards 90 deg)
        targetRot.yaw += 15.0f;
        targetPos.x += 1.2f;

        // Apply sub-tick camera damping hook
        LeviMod::CameraRotation smoothedRot = targetRot;
        LeviMod::Vec3 smoothedPos = targetPos;
        hookManager.onCameraUpdate(smoothedRot, smoothedPos, dt);

        // Generate View-Projection matrix
        LeviMod::Mat4 rotMat = LeviMod::Mat4::rotationYawPitchRoll(smoothedRot.yaw, smoothedRot.pitch, smoothedRot.roll);
        LeviMod::Mat4 projMat = LeviMod::Mat4::perspective(1.0472f, 16.0f / 9.0f, 0.1f, 1000.0f);
        LeviMod::Mat4 viewProj = projMat.multiply(rotMat);

        // Push real game frame to hook
        hookManager.onRenderPresent(framePixels.data(), depthPixels.data(), viewProj, renderWidth, renderHeight);

        std::cout << "Frame #" << frameIndex
                  << " | Target Yaw: " << targetRot.yaw << "°"
                  << " | Smooth Damped Yaw: " << smoothedRot.yaw << "°";

        // Generate Interpolated Frame
        std::vector<uint8_t> interpolatedBuffer;
        if (hookManager.getFrameGenEngine().generateInterpolatedFrame(0.5f, interpolatedBuffer)) {
            std::cout << " | ⚡ FrameGen: Success (Generated Mid-Frame # " << frameIndex << ".5)";
        }
        std::cout << "\n";
    }

    // Render Live HUD Telemetry
    LeviMod::HUDStats stats;
    stats.baseFPS = 60.0f;
    stats.generatedFPS = 120.0f;
    stats.frameGenActive = LeviMod::PluginMain::getInstance().getConfig().frameGenEnabled;
    stats.cameraSmoothingActive = LeviMod::PluginMain::getInstance().getConfig().cameraSmoothingEnabled;
    stats.smoothTime = LeviMod::PluginMain::getInstance().getConfig().smoothTime;

    std::cout << "\n" << LeviMod::OverlayHUD::renderOverlayText(stats) << std::endl;

    // Shutdown
    LeviMod::PluginMain::getInstance().shutdown();
    std::cout << "\n✅ LeviLaunchroid Mod Execution Completed Successfully!\n";

    return 0;
}
