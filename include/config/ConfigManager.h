#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "LeviMod.h"

namespace LeviMod {

    class ConfigManager {
    public:
        static bool loadFromFile(const std::string& filepath, ModConfig& config) {
            std::ifstream file(filepath);
            if (!file.is_open()) return false;

            std::string line;
            while (std::getline(file, line)) {
                if (line.empty() || line[0] == '#' || line[0] == ';') continue;
                std::istringstream is_line(line);
                std::string key;
                if (std::getline(is_line, key, '=')) {
                    std::string value;
                    if (std::getline(is_line, value)) {
                        key.erase(0, key.find_first_not_of(" \t\r\n"));
                        key.erase(key.find_last_not_of(" \t\r\n") + 1);
                        value.erase(0, value.find_first_not_of(" \t\r\n"));
                        value.erase(value.find_last_not_of(" \t\r\n") + 1);

                        try {
                            // Frame Gen & Motion Estimation
                            if (key == "frameGenEnabled") config.frameGenEnabled = (value == "true" || value == "1");
                            else if (key == "frameMultiplier") config.frameMultiplier = std::stoi(value);
                            else if (key == "tfrAlternatingFrames") config.tfrAlternatingFrames = (value == "true" || value == "1");
                            else if (key == "usePreviousRealFrame") config.usePreviousRealFrame = (value == "true" || value == "1");
                            else if (key == "frameQueueLength") config.frameQueueLength = std::stoi(value);
                            else if (key == "generatedFrameStrength") config.generatedFrameStrength = std::stof(value);
                            else if (key == "motionEstimationQuality") config.motionEstimationQuality = value;
                            else if (key == "motionSearchRange") config.motionSearchRange = std::stoi(value);
                            else if (key == "sceneChangeDetection") config.sceneChangeDetection = (value == "true" || value == "1");
                            else if (key == "fastCameraMovementProtection") config.fastCameraMovementProtection = (value == "true" || value == "1");

                            // Camera Smoothing
                            else if (key == "cameraSmoothingEnabled") config.cameraSmoothingEnabled = (value == "true" || value == "1");
                            else if (key == "smoothTime") config.smoothTime = std::stof(value);
                            else if (key == "maxSpeed") config.maxSpeed = std::stof(value);
                            else if (key == "smoothingStrength") config.smoothingStrength = std::stof(value);
                            else if (key == "cameraResponse") config.cameraResponse = value;
                            else if (key == "mouseTouchSmoothing") config.mouseTouchSmoothing = (value == "true" || value == "1");
                            else if (key == "rotationPrediction") config.rotationPrediction = (value == "true" || value == "1");
                            else if (key == "adaptiveSmoothing") config.adaptiveSmoothing = (value == "true" || value == "1");
                            else if (key == "combatSmoothing") config.combatSmoothing = (value == "true" || value == "1");
                            else if (key == "disableWhileAttacking") config.disableWhileAttacking = (value == "true" || value == "1");
                            else if (key == "disableWhileInventoryOpen") config.disableWhileInventoryOpen = (value == "true" || value == "1");

                            // Performance & Latency
                            else if (key == "lowLatencyMode") config.lowLatencyMode = (value == "true" || value == "1");
                            else if (key == "frameGenPriority") config.frameGenPriority = value;
                            else if (key == "adaptiveFrameGeneration") config.adaptiveFrameGeneration = (value == "true" || value == "1");
                            else if (key == "thermalProtection") config.thermalProtection = (value == "true" || value == "1");
                            else if (key == "disableFgWhenFpsDrops") config.disableFgWhenFpsDrops = (value == "true" || value == "1");
                            else if (key == "targetRealFPS") config.targetRealFPS = value;

                            // Debug & Telemetry
                            else if (key == "showRealFPS") config.showRealFPS = (value == "true" || value == "1");
                            else if (key == "showGeneratedFPS") config.showGeneratedFPS = (value == "true" || value == "1");
                            else if (key == "showFrameTime") config.showFrameTime = (value == "true" || value == "1");
                            else if (key == "showInputLatency") config.showInputLatency = (value == "true" || value == "1");
                            else if (key == "showFrameType") config.showFrameType = (value == "true" || value == "1");
                            else if (key == "motionVectorDebug") config.motionVectorDebug = (value == "true" || value == "1");
                            else if (key == "frameHistoryDebug") config.frameHistoryDebug = (value == "true" || value == "1");

                            // Legacy
                            else if (key == "uiMaskingEnabled") config.uiMaskingEnabled = (value == "true" || value == "1");
                            else if (key == "disocclusionProtection") config.disocclusionProtection = (value == "true" || value == "1");
                            else if (key == "disocclusionThreshold") config.disocclusionThreshold = std::stof(value);
                        } catch (...) {
                            // Ignore malformed values
                        }
                    }
                }
            }
            return true;
        }

        static bool saveToFile(const std::string& filepath, const ModConfig& config) {
            std::ofstream file(filepath);
            if (!file.is_open()) return false;

            file << "# ======================================================\n";
            file << "# 🎮 LEVILAUNCHROID ADVANCED SETTINGS CONFIGURATION     \n";
            file << "# ======================================================\n\n";

            file << "# --- 🎬 Frame Generation & Motion Estimation ---\n";
            file << "frameGenEnabled=" << (config.frameGenEnabled ? "true" : "false") << "\n";
            file << "frameMultiplier=" << config.frameMultiplier << "\n";
            file << "tfrAlternatingFrames=" << (config.tfrAlternatingFrames ? "true" : "false") << "\n";
            file << "usePreviousRealFrame=" << (config.usePreviousRealFrame ? "true" : "false") << "\n";
            file << "frameQueueLength=" << config.frameQueueLength << "\n";
            file << "generatedFrameStrength=" << config.generatedFrameStrength << "\n";
            file << "motionEstimationQuality=" << config.motionEstimationQuality << "\n";
            file << "motionSearchRange=" << config.motionSearchRange << "\n";
            file << "sceneChangeDetection=" << (config.sceneChangeDetection ? "true" : "false") << "\n";
            file << "fastCameraMovementProtection=" << (config.fastCameraMovementProtection ? "true" : "false") << "\n\n";

            file << "# --- 🎥 Camera Smoothing Physics ---\n";
            file << "cameraSmoothingEnabled=" << (config.cameraSmoothingEnabled ? "true" : "false") << "\n";
            file << "smoothTime=" << config.smoothTime << "\n";
            file << "maxSpeed=" << config.maxSpeed << "\n";
            file << "smoothingStrength=" << config.smoothingStrength << "\n";
            file << "cameraResponse=" << config.cameraResponse << "\n";
            file << "mouseTouchSmoothing=" << (config.mouseTouchSmoothing ? "true" : "false") << "\n";
            file << "rotationPrediction=" << (config.rotationPrediction ? "true" : "false") << "\n";
            file << "adaptiveSmoothing=" << (config.adaptiveSmoothing ? "true" : "false") << "\n";
            file << "combatSmoothing=" << (config.combatSmoothing ? "true" : "false") << "\n";
            file << "disableWhileAttacking=" << (config.disableWhileAttacking ? "true" : "false") << "\n";
            file << "disableWhileInventoryOpen=" << (config.disableWhileInventoryOpen ? "true" : "false") << "\n\n";

            file << "# --- ⚡ Performance & Latency ---\n";
            file << "lowLatencyMode=" << (config.lowLatencyMode ? "true" : "false") << "\n";
            file << "frameGenPriority=" << config.frameGenPriority << "\n";
            file << "adaptiveFrameGeneration=" << (config.adaptiveFrameGeneration ? "true" : "false") << "\n";
            file << "thermalProtection=" << (config.thermalProtection ? "true" : "false") << "\n";
            file << "disableFgWhenFpsDrops=" << (config.disableFgWhenFpsDrops ? "true" : "false") << "\n";
            file << "targetRealFPS=" << config.targetRealFPS << "\n\n";

            file << "# --- 🧪 Debug Overlays ---\n";
            file << "showRealFPS=" << (config.showRealFPS ? "true" : "false") << "\n";
            file << "showGeneratedFPS=" << (config.showGeneratedFPS ? "true" : "false") << "\n";
            file << "showFrameTime=" << (config.showFrameTime ? "true" : "false") << "\n";
            file << "showInputLatency=" << (config.showInputLatency ? "true" : "false") << "\n";
            file << "showFrameType=" << (config.showFrameType ? "true" : "false") << "\n";
            file << "motionVectorDebug=" << (config.motionVectorDebug ? "true" : "false") << "\n";
            file << "frameHistoryDebug=" << (config.frameHistoryDebug ? "true" : "false") << "\n";

            return true;
        }
    };

} // namespace LeviMod

#endif // CONFIG_MANAGER_H
