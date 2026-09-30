#ifndef RENDER_HOOKS_HPP
#define RENDER_HOOKS_HPP

#include "framegen/TFRFrameGenerator.hpp"
#include "RuntimeSettings.hpp"
#include "hooks/CameraHookManager.hpp"
#include <iostream>

namespace LeviMod {

    enum class FrameType {
        REAL,
        GENERATED
    };

    class RenderHooks {
    public:
        static RenderHooks& getInstance() {
            static RenderHooks instance;
            return instance;
        }

        bool install() {
            if (m_installed) return true;
            std::cout << "[RenderHooks] Installing Bedrock RenderDragon / Swapchain hooks..." << std::endl;
            m_installed = true;
            return true;
        }

        void uninstall() {
            if (!m_installed) return;
            std::cout << "[RenderHooks] Uninstalling render hooks..." << std::endl;
            m_installed = false;
        }

        // TFR Pipeline State Machine Execution:
        // Every Real Frame render produces 2 Present outputs (Real + Generated = 2x FPS)
        bool processTFRFrame(const uint8_t* realRgba, int width, int height,
                             RuntimeSettings& runtime, std::vector<uint8_t>& outGeneratedBuffer) {

            if (!runtime.masterEnabled.load() || !runtime.frameGenerationEnabled.load() || runtime.frameGenerationMode.load() == 0) {
                m_tfrEngine.setMode(levi::framegen::FrameGenerator::Mode::Off);
                runtime.realFrameCount.fetch_add(1, std::memory_order_relaxed);
                m_lastFrameType = FrameType::REAL;
                return false; // Present Real Frame only
            }

            m_tfrEngine.setMode(levi::framegen::FrameGenerator::Mode::OneX);

            // Construct Real Frame
            levi::framegen::Frame realFrame;
            realFrame.width = width;
            realFrame.height = height;
            realFrame.rgba.assign(realRgba, realRgba + (width * height * 4));

            m_tfrEngine.submitRealFrame(realFrame);
            runtime.realFrameCount.fetch_add(1, std::memory_order_relaxed);

            // If history is ready (previous REAL & current REAL exist)
            if (m_tfrEngine.hasPrevious() && m_tfrEngine.hasCurrent()) {
                if (m_tfrEngine.generate(0.5f)) {
                    outGeneratedBuffer = m_tfrEngine.generatedFrame().rgba;
                    runtime.generatedFrameCount.fetch_add(1, std::memory_order_relaxed);

                    // Commit current REAL to history after generation
                    m_tfrEngine.commitCurrentRealFrame();
                    m_lastFrameType = FrameType::GENERATED;
                    return true; // Successfully generated interpolated midpoint frame
                } else {
                    runtime.droppedFrameCount.fetch_add(1, std::memory_order_relaxed);
                }
            }

            m_lastFrameType = FrameType::REAL;
            return false; // Fallback to Real Frame
        }

        FrameType getLastFrameType() const { return m_lastFrameType; }
        levi::framegen::FrameGenerator& getTFREngine() { return m_tfrEngine; }

    private:
        RenderHooks() = default;

        levi::framegen::FrameGenerator m_tfrEngine;
        FrameType m_lastFrameType = FrameType::REAL;
        bool m_installed = false;
    };

} // namespace LeviMod

#endif // RENDER_HOOKS_HPP
