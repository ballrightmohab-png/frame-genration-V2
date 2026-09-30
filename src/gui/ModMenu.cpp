#include "gui/ModMenu.h"
#include "FrameGenMod.hpp"
#include <iostream>
#include <iomanip>

namespace LeviMod {

    ModMenu::ModMenu() {
        setupMenuItems();
    }

    void ModMenu::setupMenuItems() {
        auto& runtime = FrameGenMod::getInstance().getRuntime();

        m_items = {
            {
                "Frame Generation",
                "Toggle motion reprojection frame generation",
                [&runtime]() { return runtime.frameGenerationEnabled.load() ? "[ ON ]" : "[ OFF ]"; },
                [&runtime]() { runtime.frameGenerationEnabled.store(!runtime.frameGenerationEnabled.load()); }
            },
            {
                "Frame Generation Mode",
                "0 = Off, 1 = 1x (Interpolated), 2 = 2x",
                [&runtime]() { return std::to_string(runtime.frameGenerationMode.load()) + "x"; },
                [&runtime]() {
                    int mode = runtime.frameGenerationMode.load();
                    runtime.frameGenerationMode.store((mode + 1) % 3);
                }
            },
            {
                "Camera Smoothing",
                "Critically damped spring-damper camera physics",
                [&runtime]() { return runtime.cameraSmoothingEnabled.load() ? "[ ON ]" : "[ OFF ]"; },
                [&runtime]() { runtime.cameraSmoothingEnabled.store(!runtime.cameraSmoothingEnabled.load()); }
            },
            {
                "Camera Smoothing Strength",
                "Frame-rate independent damping strength",
                [&runtime]() { return std::to_string(static_cast<int>(runtime.cameraSmoothingStrength.load() * 100.0f)) + "%"; },
                [&runtime]() {
                    float s = runtime.cameraSmoothingStrength.load() + 0.1f;
                    if (s > 1.0f) s = 0.0f;
                    runtime.cameraSmoothingStrength.store(s);
                }
            },
            {
                "Low Latency Mode",
                "Minimize frame queue depth and render buffering",
                [&runtime]() { return runtime.lowLatencyMode.load() ? "[ ON ]" : "[ OFF ]"; },
                [&runtime]() { runtime.lowLatencyMode.store(!runtime.lowLatencyMode.load()); }
            },
            {
                "Debug Overlay",
                "Show real-time HUD telemetry stats",
                [&runtime]() { return runtime.debugOverlayEnabled.load() ? "[ ON ]" : "[ OFF ]"; },
                [&runtime]() { runtime.debugOverlayEnabled.store(!runtime.debugOverlayEnabled.load()); }
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
