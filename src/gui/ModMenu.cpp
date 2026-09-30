#include "gui/ModMenu.h"
#include <iostream>
#include <iomanip>

namespace LeviMod {

    ModMenu::ModMenu() {
        setupMenuItems();
    }

    void ModMenu::setupMenuItems() {
        auto& config = PluginMain::getInstance().getConfig();

        m_items = {
            {
                "Frame Generation",
                "Toggle motion reprojection frame generation",
                [&config]() { return config.frameGenEnabled ? "[ ON ]" : "[ OFF ]"; },
                [&config]() { config.frameGenEnabled = !config.frameGenEnabled; }
            },
            {
                "Frame Multiplier",
                "Target FPS multiplier ratio",
                [&config]() { return std::to_string(config.frameMultiplier) + "x"; },
                [&config]() {
                    config.frameMultiplier = (config.frameMultiplier >= 3) ? 2 : config.frameMultiplier + 1;
                }
            },
            {
                "Camera Smoothing",
                "Critically damped spring-damper camera physics",
                [&config]() { return config.cameraSmoothingEnabled ? "[ ON ]" : "[ OFF ]"; },
                [&config]() { config.cameraSmoothingEnabled = !config.cameraSmoothingEnabled; }
            },
            {
                "Camera Damping Time",
                "Smooth response window in milliseconds",
                [&config]() { return std::to_string(static_cast<int>(config.smoothTime * 1000.0f)) + " ms"; },
                [&config]() {
                    config.smoothTime += 0.010f;
                    if (config.smoothTime > 0.100f) config.smoothTime = 0.010f;
                }
            },
            {
                "UI / HUD Masking",
                "Prevent crosshair / UI distortion during interpolation",
                [&config]() { return config.uiMaskingEnabled ? "[ ON ]" : "[ OFF ]"; },
                [&config]() { config.uiMaskingEnabled = !config.uiMaskingEnabled; }
            },
            {
                "Disocclusion Protection",
                "Mask newly revealed surface boundaries",
                [&config]() { return config.disocclusionProtection ? "[ ON ]" : "[ OFF ]"; },
                [&config]() { config.disocclusionProtection = !config.disocclusionProtection; }
            }
        };
    }

    void ModMenu::selectNext() {
        if (m_items.empty()) return;
        m_selectedIndex = (m_selectedIndex + 1) % static_cast<int>(m_items.size());
    }

    void ModMenu::selectPrevious() {
        if (m_items.empty()) return;
        m_selectedIndex = (m_selectedIndex - 1 + static_cast<int>(m_items.size())) % static_cast<int>(m_items.size());
    }

    void ModMenu::toggleSelected() {
        if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(m_items.size())) {
            m_items[m_selectedIndex].toggleOrAdjust();
        }
    }

    void ModMenu::renderMenu() const {
        if (!m_visible) return;

        std::cout << "\n======================================================\n";
        std::cout << "     🎮 LEVILAUNCHROID MOD MENU - FRAME GEN & SMOOTH   \n";
        std::cout << "======================================================\n";

        for (size_t i = 0; i < m_items.size(); ++i) {
            bool isSelected = (static_cast<int>(i) == m_selectedIndex);
            std::cout << (isSelected ? "  👉 " : "     ");
            std::cout << std::left << std::setw(25) << m_items[i].label;
            std::cout << ": " << m_items[i].getValue() << "\n";
            if (isSelected) {
                std::cout << "        └─> " << m_items[i].description << "\n";
            }
        }
        std::cout << "======================================================\n";
        std::cout << "Controls: [N]ext | [P]revious | [T]oggle | [H]ide Menu\n";
        std::cout << "======================================================\n";
    }

} // namespace LeviMod
