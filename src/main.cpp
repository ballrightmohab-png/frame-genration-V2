#include "FrameGenMod.hpp"
#include "hooks/CameraHookManager.hpp"
#include "hooks/RenderHooks.hpp"
#include "gui/OverlayHUD.h"
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
    std::cout << "========================================================\n";
    std::cout << "  🚀 LAUNCHING LEVILAUNCHROID TFR FRAME GEN MOD 🚀       \n";
    std::cout << "========================================================\n";

    auto& mod = LeviMod::FrameGenMod::getInstance();
    mod.load();
    mod.enable();

    auto& runtime = mod.getRuntime();
    auto& cameraHooks = LeviMod::CameraHookManager::getInstance();
    auto& renderHooks = LeviMod::RenderHooks::getInstance();

    int w = 64, h = 64;
    std::vector<uint8_t> realFrameA(w * h * 4, 100);
    std::vector<uint8_t> realFrameB(w * h * 4, 200);

    LeviMod::CameraState targetCamera;
    targetCamera.position = {0.0f, 64.0f, 0.0f};

    std::vector<uint8_t> generatedBuffer;

    std::cout << "\n--- Submitting Real Frame A ---" << std::endl;
    bool gen1 = renderHooks.processTFRFrame(realFrameA.data(), w, h, runtime, generatedBuffer);
    std::cout << "Step 1 Output: " << (gen1 ? "⚡ [GENERATED]" : "🎥 [REAL (Establishing History)]") << std::endl;

    std::cout << "\n--- Submitting Real Frame B (TFR Generation Trigger) ---" << std::endl;
    bool gen2 = renderHooks.processTFRFrame(realFrameB.data(), w, h, runtime, generatedBuffer);
    std::cout << "Step 2 Output: " << (gen2 ? "⚡ [GENERATED FRAME A→B]" : "🎥 [REAL]") << std::endl;

    std::cout << "\n--- Debug Telemetry HUD Output ---" << std::endl;
    LeviMod::HUDStats stats;
    stats.realFPS = 60.0f;
    stats.outputFPS = 120.0f; // Real 60 FPS doubled to 120 FPS
    stats.frameTimeMs = 8.33f;
    stats.frameType = (gen2 ? "GENERATED" : "REAL");

    std::cout << LeviMod::OverlayHUD::renderOverlayText(stats, runtime) << std::endl;

    mod.disable();
    mod.unload();

    std::cout << "✅ Levi TFR Frame Generator Execution Completed Successfully!\n";
    return 0;
}
