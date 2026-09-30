#ifndef RENDER_HOOKS_HPP
#define RENDER_HOOKS_HPP

#include "framegen/FrameGenEngine.h"
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

        // Render Present Hook Pipeline State Machine: REAL -> GENERATED -> REAL -> GENERATED
        bool onRenderPresent(const uint8_t* realColorPixels, const float* depthPixels,
                             const Mat4& viewProjMatrix, int width, int height,
                             RuntimeSettings& runtime, std::vector<uint8_t>& outOutputPixels) {

            if (!runtime.masterEnabled.load() || !runtime.frameGenerationEnabled.load() || runtime.frameGenerationMode.load() == 0) {
                // Fallback: Present Real Frame directly
                runtime.realFrameCount.fetch_add(1, std::memory_order_relaxed);
                m_lastFrameType = FrameType::REAL;
                return false; // Display Real Frame
            }

            if (m_engine.getWidth() != width || m_engine.getHeight() != height) {
                m_engine.initialize(width, height);
            }

            // Always capture fresh REAL frame into temporal history
            m_engine.pushNewFrame(realColorPixels, depthPixels, viewProjMatrix);
            runtime.realFrameCount.fetch_add(1, std::memory_order_relaxed);

            // Check history depth: need at least 2 real frames (previousReal + currentReal)
            if (!m_engine.isReady()) {
                m_lastFrameType = FrameType::REAL;
                return false; // Fall back to real frame until history is full
            }

            // State Machine Check: Alternate Real -> Generated -> Real -> Generated
            if (m_lastFrameType == FrameType::REAL) {
                // Generate Intermediate Frame (t = 0.5) anchored to fresh real frames
                float strength = runtime.generatedFrameStrength.load();
                if (strength <= 0.01f) {
                    m_lastFrameType = FrameType::REAL;
                    return false;
                }

                bool genOk = m_engine.generateInterpolatedFrame(0.5f, outOutputPixels);
                if (genOk) {
                    runtime.generatedFrameCount.fetch_add(1, std::memory_order_relaxed);
                    m_lastFrameType = FrameType::GENERATED;
                    return true; // Present GENERATED Frame
                } else {
                    runtime.droppedFrameCount.fetch_add(1, std::memory_order_relaxed);
                    m_lastFrameType = FrameType::REAL;
                    return false; // Fallback to Real Frame on error
                }
            } else {
                // Next step in pattern: Present REAL Frame
                m_lastFrameType = FrameType::REAL;
                return false; // Display Real Frame
            }
        }

        FrameType getLastFrameType() const { return m_lastFrameType; }
        FrameGenEngine& getEngine() { return m_engine; }

    private:
        RenderHooks() = default;

        FrameGenEngine m_engine;
        FrameType m_lastFrameType = FrameType::REAL;
        bool m_installed = false;
    };

} // namespace LeviMod

#endif // RENDER_HOOKS_HPP
