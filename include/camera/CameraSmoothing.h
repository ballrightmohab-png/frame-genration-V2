#ifndef CAMERA_SMOOTHING_H
#define CAMERA_SMOOTHING_H

#include <cmath>
#include <algorithm>

namespace LeviMod {

    struct Vec3 {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        Vec3() = default;
        Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

        Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
        Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
        Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
        Vec3 operator/(float s) const { return {x / s, y / s, z / s}; }
    };

    struct CameraRotation {
        float yaw = 0.0f;   // horizontal angle in degrees
        float pitch = 0.0f; // vertical angle in degrees
        float roll = 0.0f;  // roll angle in degrees
    };

    class CameraSmoothing {
    public:
        CameraSmoothing();

        // Sets the smoothing responsiveness time constant (in seconds)
        void setSmoothTime(float timeSeconds) { m_smoothTime = std::max(0.001f, timeSeconds); }
        float getSmoothTime() const { return m_smoothTime; }

        void setMaxSpeed(float maxDegPerSec) { m_maxSpeed = maxDegPerSec; }
        float getMaxSpeed() const { return m_maxSpeed; }

        // Teleport camera target without smoothing lag
        void reset(const CameraRotation& currentRot, const Vec3& currentPos);

        // Update rotation with critically damped spring physics
        CameraRotation updateRotation(const CameraRotation& targetRot, float deltaTime);

        // Update position with critically damped spring physics
        Vec3 updatePosition(const Vec3& targetPos, float deltaTime);

        const CameraRotation& getCurrentRotation() const { return m_currentRot; }
        const Vec3& getCurrentPosition() const { return m_currentPos; }

    private:
        // Helper to damp a single float value (or angle)
        static float smoothDampAngle(float current, float target, float& currentVelocity,
                                     float smoothTime, float maxSpeed, float deltaTime);

        static float smoothDampVal(float current, float target, float& currentVelocity,
                                   float smoothTime, float maxSpeed, float deltaTime);

        float m_smoothTime = 0.04f; // Default 40ms smoothing window
        float m_maxSpeed = 3600.0f; // Max rotation speed deg/sec

        CameraRotation m_currentRot;
        CameraRotation m_rotVelocity;

        Vec3 m_currentPos;
        Vec3 m_posVelocity;

        bool m_initialized = false;
    };

} // namespace LeviMod

#endif // CAMERA_SMOOTHING_H
