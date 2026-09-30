#include "camera/CameraSmoothing.h"
#include <cmath>
#include <algorithm>

namespace LeviMod {

    // Safely normalize angle to [-180, 180] degrees (NaN-safe & O(1))
    static float normalizeAngle(float angle) {
        if (std::isnan(angle) || std::isinf(angle)) return 0.0f;
        float a = std::fmod(angle + 180.0f, 360.0f);
        if (a < 0.0f) a += 360.0f;
        return a - 180.0f;
    }

    CameraSmoothing::CameraSmoothing() {
        m_rotVelocity = {0.0f, 0.0f, 0.0f};
        m_posVelocity = {0.0f, 0.0f, 0.0f};
    }

    void CameraSmoothing::reset(const CameraRotation& currentRot, const Vec3& currentPos) {
        m_currentRot = currentRot;
        m_currentPos = currentPos;
        m_rotVelocity = {0.0f, 0.0f, 0.0f};
        m_posVelocity = {0.0f, 0.0f, 0.0f};
        m_initialized = true;
    }

    float CameraSmoothing::smoothDampAngle(float current, float target, float& currentVelocity,
                                           float smoothTime, float maxSpeed, float deltaTime) {
        if (deltaTime <= 0.0f) return current;

        // Calculate shortest angle delta
        float deltaAngle = normalizeAngle(target - current);
        target = current + deltaAngle;

        return smoothDampVal(current, target, currentVelocity, smoothTime, maxSpeed, deltaTime);
    }

    float CameraSmoothing::smoothDampVal(float current, float target, float& currentVelocity,
                                         float smoothTime, float maxSpeed, float deltaTime) {
        if (deltaTime <= 0.0f) return current;

        smoothTime = std::max(0.0001f, smoothTime);
        float omega = 2.0f / smoothTime;

        float x = omega * deltaTime;
        float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

        float change = current - target;
        float originalTarget = target;

        // Clamp maximum speed
        float maxChange = maxSpeed * smoothTime;
        change = std::clamp(change, -maxChange, maxChange);
        target = current - change;

        float temp = (currentVelocity + omega * change) * deltaTime;
        currentVelocity = (currentVelocity - omega * temp) * exp;
        float output = target + (change + temp) * exp;

        // Prevent overshooting target
        if ((originalTarget - current > 0.0f) == (output > originalTarget)) {
            output = originalTarget;
            currentVelocity = 0.0f;
        }

        return output;
    }

    CameraRotation CameraSmoothing::updateRotation(const CameraRotation& targetRot, float deltaTime) {
        if (!m_initialized) {
            reset(targetRot, m_currentPos);
            return m_currentRot;
        }

        m_currentRot.yaw = smoothDampAngle(m_currentRot.yaw, targetRot.yaw, m_rotVelocity.yaw,
                                           m_smoothTime, m_maxSpeed, deltaTime);
        m_currentRot.pitch = smoothDampAngle(m_currentRot.pitch, targetRot.pitch, m_rotVelocity.pitch,
                                             m_smoothTime, m_maxSpeed, deltaTime);
        m_currentRot.roll = smoothDampAngle(m_currentRot.roll, targetRot.roll, m_rotVelocity.roll,
                                            m_smoothTime, m_maxSpeed, deltaTime);

        return m_currentRot;
    }

    Vec3 CameraSmoothing::updatePosition(const Vec3& targetPos, float deltaTime) {
        if (!m_initialized) {
            reset(m_currentRot, targetPos);
            return m_currentPos;
        }

        m_currentPos.x = smoothDampVal(m_currentPos.x, targetPos.x, m_posVelocity.x,
                                       m_smoothTime, m_maxSpeed, deltaTime);
        m_currentPos.y = smoothDampVal(m_currentPos.y, targetPos.y, m_posVelocity.y,
                                       m_smoothTime, m_maxSpeed, deltaTime);
        m_currentPos.z = smoothDampVal(m_currentPos.z, targetPos.z, m_posVelocity.z,
                                       m_smoothTime, m_maxSpeed, deltaTime);

        return m_currentPos;
    }

} // namespace LeviMod
