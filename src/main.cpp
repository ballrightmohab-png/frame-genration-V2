#include "LeviMod.h"
#include "hooks/LeviHooks.h"
#include "gui/ModMenu.h"
#include "gui/OverlayHUD.h"
#include <iostream>
#include <vector>

int main(int argc, char** argv) {
    std::cout << "========================================================\n";
    std::cout << "  🚀 LAUNCHING LEVILAUNCHROID ADVANCED MOD RUNNER 🚀  \n";
    std::cout << "========================================================\n";

    if (!LeviMod::PluginMain::getInstance().initialize()) {
        std::cerr << "Failed to initialize mod!" << std::endl;
        return 1;
    }

    auto& hookManager = LeviMod::LeviHookManager::getInstance();
    auto& config = LeviMod::PluginMain::getInstance().getConfig();

    std::cout << "\n--- Displaying Overlay HUD Telemetry ---" << std::endl;
    LeviMod::HUDStats stats;
    stats.baseFPS = 60.0f;
    stats.generatedFPS = 120.0f;
    stats.frameTimeMs = 16.6f;
    stats.inputLatencyMs = 8.2f;
    stats.currentFrameType = "GENERATED";

    std::cout << LeviMod::OverlayHUD::renderOverlayText(stats, config) << std::endl;

    LeviMod::PluginMain::getInstance().shutdown();
    std::cout << "✅ Advanced Mod Runner Test Completed Successfully!\n";

    return 0;
}
