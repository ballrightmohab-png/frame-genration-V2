#include <iostream>
#include <cassert>
#include <cmath>
#include "camera/CameraSmoothing.h"
#include "framegen/FrameGenEngine.h"
#include "math/MatrixMath.h"
#include "config/ConfigManager.h"
#include "gui/OverlayHUD.h"
#include "gui/ModMenu.h"
#include "LeviMod.h"

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

void testFrameGenEngine() {
    std::cout << "[Test] Running Frame Generation Engine tests..." << std::endl;
    LeviMod::FrameGenEngine engine;
    int w = 64, h = 64;
    engine.initialize(w, h);

    std::vector<uint8_t> frame1(w * h * 4, 100);
    std::vector<uint8_t> frame2(w * h * 4, 200);
    std::vector<float> depth(w * h, 0.5f);

    LeviMod::Mat4 m1 = LeviMod::Mat4::rotationYawPitchRoll(0.0f, 0.0f, 0.0f);
    LeviMod::Mat4 m2 = LeviMod::Mat4::rotationYawPitchRoll(10.0f, 0.0f, 0.0f);

    engine.pushNewFrame(frame1.data(), depth.data(), m1);
    assert(!engine.isReady());

    engine.pushNewFrame(frame2.data(), depth.data(), m2);
    assert(engine.isReady());

    std::vector<uint8_t> interpolatedFrame;
    bool genSuccess = engine.generateInterpolatedFrame(0.5f, interpolatedFrame);
    assert(genSuccess);
    assert(interpolatedFrame.size() == w * h * 4);

    uint8_t midVal = interpolatedFrame[0];
    assert(midVal >= 130 && midVal <= 170);

    std::cout << "  ✓ Frame Generation motion interpolation verified! Mid Pixel Val: " << (int)midVal << std::endl;
}

void testConfigAndModMenu() {
    std::cout << "[Test] Running Config & Mod Menu Integration tests..." << std::endl;

    std::string testConfigPath = "test_config.ini";
    LeviMod::ModConfig config;
    config.smoothTime = 0.045f;
    config.frameMultiplier = 3;

    assert(LeviMod::ConfigManager::saveToFile(testConfigPath, config));

    LeviMod::ModConfig loadedConfig;
    assert(LeviMod::ConfigManager::loadFromFile(testConfigPath, loadedConfig));
    assert(std::abs(loadedConfig.smoothTime - 0.045f) < 1e-4f);
    assert(loadedConfig.frameMultiplier == 3);

    LeviMod::ModMenu menu;
    assert(menu.isVisible());

    std::cout << "  ✓ Config & Mod Menu Integration verified!" << std::endl;
}

int main() {
    std::cout << "=== LeviLaunchroid Mod Unit Test Suite ===" << std::endl;
    testCameraSmoothing();
    testMatrixMath();
    testFrameGenEngine();
    testConfigAndModMenu();
    std::cout << "=== ALL TESTS PASSED SUCCESSFULLY! ===" << std::endl;
    return 0;
}
