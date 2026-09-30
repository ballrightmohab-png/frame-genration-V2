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
                        // Trim whitespace
                        key.erase(0, key.find_first_not_of(" \t\r\n"));
                        key.erase(key.find_last_not_of(" \t\r\n") + 1);
                        value.erase(0, value.find_first_not_of(" \t\r\n"));
                        value.erase(value.find_last_not_of(" \t\r\n") + 1);

                        if (key == "cameraSmoothingEnabled") config.cameraSmoothingEnabled = (value == "true" || value == "1");
                        else if (key == "smoothTime") config.smoothTime = std::stof(value);
                        else if (key == "maxSpeed") config.maxSpeed = std::stof(value);
                        else if (key == "frameGenEnabled") config.frameGenEnabled = (value == "true" || value == "1");
                        else if (key == "frameMultiplier") config.frameMultiplier = std::stoi(value);
                        else if (key == "motionVectorStrength") config.motionVectorStrength = std::stof(value);
                        else if (key == "uiMaskingEnabled") config.uiMaskingEnabled = (value == "true" || value == "1");
                        else if (key == "disocclusionProtection") config.disocclusionProtection = (value == "true" || value == "1");
                    }
                }
            }
            return true;
        }

        static bool saveToFile(const std::string& filepath, const ModConfig& config) {
            std::ofstream file(filepath);
            if (!file.is_open()) return false;

            file << "# LeviLaunchroid Frame Gen & Camera Smoothing Config\n";
            file << "cameraSmoothingEnabled=" << (config.cameraSmoothingEnabled ? "true" : "false") << "\n";
            file << "smoothTime=" << config.smoothTime << "\n";
            file << "maxSpeed=" << config.maxSpeed << "\n";
            file << "frameGenEnabled=" << (config.frameGenEnabled ? "true" : "false") << "\n";
            file << "frameMultiplier=" << config.frameMultiplier << "\n";
            file << "motionVectorStrength=" << config.motionVectorStrength << "\n";
            file << "uiMaskingEnabled=" << (config.uiMaskingEnabled ? "true" : "false") << "\n";
            file << "disocclusionProtection=" << (config.disocclusionProtection ? "true" : "false") << "\n";

            return true;
        }
    };

} // namespace LeviMod

#endif // CONFIG_MANAGER_H
