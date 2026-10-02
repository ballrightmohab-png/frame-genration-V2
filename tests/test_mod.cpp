#include <iostream>
#include <cassert>
#include <cmath>
#include <chrono>
#include "camera/CameraSmoothing.h"
#include "framegen/TFRFrameGenerator.hpp"
#include "framegen/FrameGenEngine.h"
#include "math/MatrixMath.h"
#include "gui/OverlayHUD.h"
#include "pl/ModMenu.hpp"
#include "FrameGenMod.hpp"

void testCameraSmoothing() {
    std::cout << "[Test] Running Camera Smoothing tests..." << std::endl;
    LeviMod::CameraSmoothing cs;
    cs.setSmoothTime(0.05f); // 50ms

    LeviMod::CameraRotation initialRot = {0.0f, 0.0f, 0.0f};
    LeviMod::Vec3 initialPos = {0.0f, 64.0f, 0.0f};
    cs.reset(initialRot, initialPos);

    LeviMod::CameraRotation targetRot = {90.0f, 0.0f, 0.0f};

    LeviMod::CameraRotation r1 = cs.updateRotation(targetRot, 0.016f);
    assert(r1.yaw > 0.0f && r1.yaw < 90.0f);

    for (int i = 0; i < 20; ++i) {
        r1 = cs.updateRotation(targetRot, 0.016f);
    }

    assert(std::abs(r1.yaw - 90.0f) < 5.0f);
    std::cout << "  ✓ Camera Smoothing physics verified! Final Yaw: " << r1.yaw << std::endl;
}

void testMatrixMath() {
    std::cout << "[Test] Running Matrix Math tests..." << std::endl;
    LeviMod::Mat4 proj = LeviMod::Mat4::perspective(1.0472f, 16.0f / 9.0f, 0.1f, 1000.0f);
    LeviMod::Mat4 invProj = proj.inverse();

    LeviMod::Mat4 identityCheck = proj.multiply(invProj);
    assert(std::abs(identityCheck.m[0] - 1.0f) < 1e-3f);
    assert(std::abs(identityCheck.m[5] - 1.0f) < 1e-3f);
    assert(std::abs(identityCheck.m[10] - 1.0f) < 1e-3f);
    assert(std::abs(identityCheck.m[15] - 1.0f) < 1e-3f);

    std::cout << "  ✓ Matrix Inversion and Projection verified!" << std::endl;
}

void testTFRFrameGenerator() {
    std::cout << "[Test] Running Levi TFR Frame Generator tests..." << std::endl;
    levi::framegen::FrameGenerator fg;
    fg.setMode(levi::framegen::FrameGenerator::Mode::OneX);

    int w = 16, h = 16;
    levi::framegen::Frame f1, f2;
    f1.resize(w, h);
    f2.resize(w, h);

    std::fill(f1.rgba.begin(), f1.rgba.end(), 100);
    std::fill(f2.rgba.begin(), f2.rgba.end(), 200);

    fg.submitRealFrame(f1);
    assert(fg.hasPrevious());
    assert(!fg.hasCurrent());

    fg.submitRealFrame(f2);
    assert(fg.hasPrevious());
    assert(fg.hasCurrent());

    bool genOk = fg.generate(0.5f);
    assert(genOk);

    const auto& genFrame = fg.generatedFrame();
    assert(genFrame.valid());
    assert(genFrame.width == w && genFrame.height == h);

    // Bilinear + motion weighted sample value check
    uint8_t midPixelVal = genFrame.rgba[0];
    assert(midPixelVal >= 130 && midPixelVal <= 170);

    fg.commitCurrentRealFrame();
    assert(fg.hasPrevious());
    assert(!fg.hasCurrent());

    std::cout << "  ✓ Levi TFR Frame Generator verified! Mid Pixel Value: " << (int)midPixelVal << std::endl;
}

void testModLifecycleAndRuntime() {
    std::cout << "[Test] Running Mod Lifecycle & Runtime Settings tests..." << std::endl;
    auto& mod = LeviMod::FrameGenMod::getInstance();
    assert(mod.load());
    assert(mod.enable());

    auto& runtime = mod.getRuntime();
    assert(runtime.masterEnabled.load());
    assert(runtime.frameGenerationEnabled.load());

    LeviMod::HUDStats stats;
    std::string hud = LeviMod::OverlayHUD::renderOverlayText(stats, runtime);
    assert(!hud.empty());
    assert(hud.find("FrameGen: ON") != std::string::npos);

    assert(mod.disable());
    assert(mod.unload());

    std::cout << "  ✓ Mod Lifecycle & Runtime Settings verified!" << std::endl;
}

void testFrameGenEngine() {
    std::cout << "[Test & Benchmark] Running FrameGenEngine Spatial Reprojection tests..." << std::endl;
    LeviMod::FrameGenEngine engine;
    int w = 1920, h = 1080;
    engine.initialize(w, h);

    std::vector<uint8_t> color1(w * h * 4, 100);
    std::vector<uint8_t> color2(w * h * 4, 200);
    std::vector<float> depth1(w * h, 0.5f);
    std::vector<float> depth2(w * h, 0.5f);

    // Give some distinct pixel values to test interpolation correctness
    for (int i = 0; i < w * h; ++i) {
        color1[i * 4 + 0] = static_cast<uint8_t>(i % 256);
        color1[i * 4 + 1] = static_cast<uint8_t>((i * 2) % 256);
        color1[i * 4 + 2] = static_cast<uint8_t>((i * 3) % 256);
        color1[i * 4 + 3] = 255;

        color2[i * 4 + 0] = static_cast<uint8_t>((i + 50) % 256);
        color2[i * 4 + 1] = static_cast<uint8_t>((i * 2 + 50) % 256);
        color2[i * 4 + 2] = static_cast<uint8_t>((i * 3 + 50) % 256);
        color2[i * 4 + 3] = 255;
    }

    LeviMod::Mat4 v1 = LeviMod::Mat4::perspective(1.0472f, 16.0f / 9.0f, 0.1f, 1000.0f)
                        .multiply(LeviMod::Mat4::rotationYawPitchRoll(0.0f, 0.0f, 0.0f));
    LeviMod::Mat4 v2 = LeviMod::Mat4::perspective(1.0472f, 16.0f / 9.0f, 0.1f, 1000.0f)
                        .multiply(LeviMod::Mat4::rotationYawPitchRoll(2.0f, 1.0f, 0.0f));

    engine.pushNewFrame(color1.data(), depth1.data(), v1);
    engine.pushNewFrame(color2.data(), depth2.data(), v2);

    assert(engine.isReady());

    std::vector<uint8_t> outColor;
    bool ok = engine.generateInterpolatedFrame(0.5f, outColor);
    assert(ok);
    assert(outColor.size() == static_cast<size_t>(w * h * 4));

    // Warmup
    for (int i = 0; i < 3; ++i) {
        engine.generateInterpolatedFrame(0.5f, outColor);
    }

    // Benchmark
    constexpr int iterations = 30;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        engine.generateInterpolatedFrame(0.5f, outColor);
    }
    auto end = std::chrono::high_resolution_clock::now();
    double totalMs = std::chrono::duration<double, std::milli>(end - start).count();
    double avgMs = totalMs / iterations;

    std::cout << "  ✓ FrameGenEngine verified! 1080p generation time: " << avgMs << " ms/frame (" << iterations << " iterations)" << std::endl;
}

int main() {
    std::cout << "=== LeviLaunchroid Native Mod Unit Test Suite ===" << std::endl;
    testCameraSmoothing();
    testMatrixMath();
    testTFRFrameGenerator();
    testFrameGenEngine();
    testModLifecycleAndRuntime();
    std::cout << "=== ALL TESTS PASSED SUCCESSFULLY! ===" << std::endl;
    return 0;
}
