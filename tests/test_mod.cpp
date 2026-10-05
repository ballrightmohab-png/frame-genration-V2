#include <iostream>
#include <cassert>
#include <cmath>
#include "camera/CameraSmoothing.h"
#include "framegen/FrameGenEngine.h"
#include "framegen/TFRFrameGenerator.hpp"
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

void testFrameGenEngineReprojection() {
    std::cout << "[Test] Running FrameGenEngine Spatial-Temporal Reprojection tests..." << std::endl;
    LeviMod::FrameGenEngine engine;
    int w = 32, h = 32;
    engine.initialize(w, h);

    std::vector<uint8_t> frame1Color(w * h * 4, 100);
    std::vector<float> frame1Depth(w * h, 0.5f);
    LeviMod::Mat4 viewProj1 = LeviMod::Mat4::identity();

    engine.pushNewFrame(frame1Color.data(), frame1Depth.data(), viewProj1);
    assert(!engine.isReady());

    std::vector<uint8_t> frame2Color(w * h * 4, 200);
    std::vector<float> frame2Depth(w * h, 0.5f);
    LeviMod::Mat4 viewProj2 = LeviMod::Mat4::rotationYawPitchRoll(5.0f, 0.0f, 0.0f);

    engine.pushNewFrame(frame2Color.data(), frame2Depth.data(), viewProj2);
    assert(engine.isReady());

    std::vector<uint8_t> outputColor;
    bool result = engine.generateInterpolatedFrame(0.5f, outputColor);
    assert(result);
    assert(outputColor.size() == static_cast<size_t>(w * h * 4));

    // Verify interpolated frame pixels are valid
    uint8_t pixelVal = outputColor[0];
    assert(pixelVal >= 100 && pixelVal <= 200);

    std::cout << "  ✓ FrameGenEngine Reprojection verified! Sample Pixel: " << static_cast<int>(pixelVal) << std::endl;
}

int main() {
    std::cout << "=== LeviLaunchroid Native Mod Unit Test Suite ===" << std::endl;
    testCameraSmoothing();
    testMatrixMath();
    testTFRFrameGenerator();
    testFrameGenEngineReprojection();
    testModLifecycleAndRuntime();
    std::cout << "=== ALL TESTS PASSED SUCCESSFULLY! ===" << std::endl;
    return 0;
}
