#ifndef RUNTIME_SETTINGS_HPP
#define RUNTIME_SETTINGS_HPP

#include <atomic>
#include <fstream>
#include <string>

namespace LeviMod {

    struct RuntimeSettings {
        // Lock-free atomic state accessed by render and camera hooks
        std::atomic<bool> masterEnabled{true};
        std::atomic<bool> frameGenerationEnabled{true};
        std::atomic<int> frameGenerationMode{1}; // 0 = Off, 1 = 1x, 2 = 2x
        std::atomic<float> generatedFrameStrength{1.0f}; // 0.0 - 1.0
        std::atomic<int> preset{1}; // 0 = Low Latency, 1 = Balanced, 2 = Smoothness
        std::atomic<bool> adaptiveMode{true};
        std::atomic<bool> lowLatencyMode{false};

        std::atomic<bool> cameraSmoothingEnabled{true};
        std::atomic<float> cameraSmoothingStrength{0.5f}; // 0.0 - 1.0

        std::atomic<bool> debugOverlayEnabled{true};

        // Statistics counters for Telemetry
        std::atomic<uint64_t> realFrameCount{0};
        std::atomic<uint64_t> generatedFrameCount{0};
        std::atomic<uint64_t> droppedFrameCount{0};
        std::atomic<float> avgGenerationTimeMs{0.0f};
    };

    struct FrameGenConfig {
        int version = 1;

        bool masterEnabled = true;
        bool frameGeneration = true;
        int frameGenerationMode = 1; // 0 = Off, 1 = 1x, 2 = 2x
        float generatedFrameStrength = 1.0f;
        int preset = 1; // 0 = Low Latency, 1 = Balanced, 2 = Smoothness
        bool adaptiveMode = true;
        bool lowLatency = false;

        bool cameraSmoothing = true;
        float smoothingStrength = 0.5f;

        bool debugOverlay = true;

        void syncToRuntime(RuntimeSettings& runtime) const {
            runtime.masterEnabled.store(masterEnabled);
            runtime.frameGenerationEnabled.store(frameGeneration);
            runtime.frameGenerationMode.store(frameGenerationMode);
            runtime.generatedFrameStrength.store(generatedFrameStrength);
            runtime.preset.store(preset);
            runtime.adaptiveMode.store(adaptiveMode);
            runtime.lowLatencyMode.store(lowLatency);

            runtime.cameraSmoothingEnabled.store(cameraSmoothing);
            runtime.cameraSmoothingStrength.store(smoothingStrength);

            runtime.debugOverlayEnabled.store(debugOverlay);
        }

        void syncFromRuntime(const RuntimeSettings& runtime) {
            masterEnabled = runtime.masterEnabled.load();
            frameGeneration = runtime.frameGenerationEnabled.load();
            frameGenerationMode = runtime.frameGenerationMode.load();
            generatedFrameStrength = runtime.generatedFrameStrength.load();
            preset = runtime.preset.load();
            adaptiveMode = runtime.adaptiveMode.load();
            lowLatency = runtime.lowLatencyMode.load();

            cameraSmoothing = runtime.cameraSmoothingEnabled.load();
            smoothingStrength = runtime.cameraSmoothingStrength.load();

            debugOverlay = runtime.debugOverlayEnabled.load();
        }

        void parseField(const std::string& key, const std::string& val) {
            bool bVal = (val == "true" || val == "1");
            if (key == "masterEnabled") masterEnabled = bVal;
            else if (key == "frameGeneration") frameGeneration = bVal;
            else if (key == "frameGenerationMode") frameGenerationMode = std::stoi(val);
            else if (key == "generatedFrameStrength") generatedFrameStrength = std::stof(val);
            else if (key == "preset") preset = std::stoi(val);
            else if (key == "adaptiveMode") adaptiveMode = bVal;
            else if (key == "lowLatency") lowLatency = bVal;
            else if (key == "cameraSmoothing") cameraSmoothing = bVal;
            else if (key == "smoothingStrength") smoothingStrength = std::stof(val);
            else if (key == "debugOverlay") debugOverlay = bVal;
        }

        void writeFields(std::ofstream& file) const {
            file << "# Levi FrameGen & Camera Smoothing Configuration\n";
            file << "masterEnabled=" << (masterEnabled ? "true" : "false") << "\n";
            file << "frameGeneration=" << (frameGeneration ? "true" : "false") << "\n";
            file << "frameGenerationMode=" << frameGenerationMode << "\n";
            file << "generatedFrameStrength=" << generatedFrameStrength << "\n";
            file << "preset=" << preset << "\n";
            file << "adaptiveMode=" << (adaptiveMode ? "true" : "false") << "\n";
            file << "lowLatency=" << (lowLatency ? "true" : "false") << "\n";
            file << "cameraSmoothing=" << (cameraSmoothing ? "true" : "false") << "\n";
            file << "smoothingStrength=" << smoothingStrength << "\n";
            file << "debugOverlay=" << (debugOverlay ? "true" : "false") << "\n";
        }
    };

} // namespace LeviMod

#endif // RUNTIME_SETTINGS_HPP
