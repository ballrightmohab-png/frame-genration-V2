#ifndef MATRIX_MATH_H
#define MATRIX_MATH_H

#include <cmath>
#include <array>
#include "camera/CameraSmoothing.h"

namespace LeviMod {

    struct Mat4 {
        std::array<float, 16> m{};

        Mat4() {
            m.fill(0.0f);
            m[0] = m[5] = m[10] = m[15] = 1.0f; // Identity
        }

        static Mat4 identity() {
            return Mat4();
        }

        static Mat4 perspective(float fovRadians, float aspect, float nearZ, float farZ) {
            Mat4 res;
            res.m.fill(0.0f);
            float tanHalfFov = std::tan(fovRadians / 2.0f);
            res.m[0] = 1.0f / (aspect * tanHalfFov);
            res.m[5] = 1.0f / tanHalfFov;
            res.m[10] = -(farZ + nearZ) / (farZ - nearZ);
            res.m[11] = -1.0f;
            res.m[14] = -(2.0f * farZ * nearZ) / (farZ - nearZ);
            return res;
        }

        static Mat4 rotationYawPitchRoll(float yawDeg, float pitchDeg, float rollDeg) {
            float yaw = yawDeg * (3.14159265f / 180.0f);
            float pitch = pitchDeg * (3.14159265f / 180.0f);
            float roll = rollDeg * (3.14159265f / 180.0f);

            float cy = std::cos(yaw), sy = std::sin(yaw);
            float cp = std::cos(pitch), sp = std::sin(pitch);
            float cr = std::cos(roll), sr = std::sin(roll);

            Mat4 res;
            res.m[0] = cy * cr + sy * sp * sr;
            res.m[1] = sr * cp;
            res.m[2] = -sy * cr + cy * sp * sr;
            res.m[3] = 0.0f;

            res.m[4] = -cy * sr + sy * sp * cr;
            res.m[5] = cr * cp;
            res.m[6] = sr * sy + cy * sp * cr;
            res.m[7] = 0.0f;

            res.m[8] = sy * cp;
            res.m[9] = -sp;
            res.m[10] = cy * cp;
            res.m[11] = 0.0f;

            res.m[12] = 0.0f;
            res.m[13] = 0.0f;
            res.m[14] = 0.0f;
            res.m[15] = 1.0f;

            return res;
        }

        static Mat4 lookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
            Vec3 f = (target - eye);
            float lenF = std::sqrt(f.x * f.x + f.y * f.y + f.z * f.z);
            if (lenF > 1e-6f) f = f / lenF;

            Vec3 s = {f.y * up.z - f.z * up.y, f.z * up.x - f.x * up.z, f.x * up.y - f.y * up.x};
            float lenS = std::sqrt(s.x * s.x + s.y * s.y + s.z * s.z);
            if (lenS > 1e-6f) s = s / lenS;

            Vec3 u = {s.y * f.z - s.z * f.y, s.z * f.x - s.x * f.z, s.x * f.y - s.y * f.x};

            Mat4 res;
            res.m[0] = s.x;  res.m[4] = s.y;  res.m[8]  = s.z;  res.m[12] = -(s.x * eye.x + s.y * eye.y + s.z * eye.z);
            res.m[1] = u.x;  res.m[5] = u.y;  res.m[9]  = u.z;  res.m[13] = -(u.x * eye.x + u.y * eye.y + u.z * eye.z);
            res.m[2] = -f.x; res.m[6] = -f.y; res.m[10] = -f.z; res.m[14] = (f.x * eye.x + f.y * eye.y + f.z * eye.z);
            res.m[3] = 0.0f; res.m[7] = 0.0f; res.m[11] = 0.0f; res.m[15] = 1.0f;
            return res;
        }

        Mat4 multiply(const Mat4& o) const {
            Mat4 res;
            for (int r = 0; r < 4; ++r) {
                for (int c = 0; c < 4; ++c) {
                    res.m[r + c * 4] = m[r + 0 * 4] * o.m[0 + c * 4] +
                                       m[r + 1 * 4] * o.m[1 + c * 4] +
                                       m[r + 2 * 4] * o.m[2 + c * 4] +
                                       m[r + 3 * 4] * o.m[3 + c * 4];
                }
            }
            return res;
        }

        Mat4 inverse() const {
            // Standard 4x4 matrix inversion
            Mat4 inv;
            const float* m_in = m.data();
            float* invOut = inv.m.data();

            invOut[0] = m_in[5]  * m_in[10] * m_in[15] -
                        m_in[5]  * m_in[11] * m_in[14] -
                        m_in[9]  * m_in[6]  * m_in[15] +
                        m_in[9]  * m_in[7]  * m_in[14] +
                        m_in[13] * m_in[6]  * m_in[11] -
                        m_in[13] * m_in[7]  * m_in[10];

            invOut[4] = -m_in[4]  * m_in[10] * m_in[15] +
                         m_in[4]  * m_in[11] * m_in[14] +
                         m_in[8]  * m_in[6]  * m_in[15] -
                         m_in[8]  * m_in[7]  * m_in[14] -
                         m_in[12] * m_in[6]  * m_in[11] +
                         m_in[12] * m_in[7]  * m_in[10];

            invOut[8] = m_in[4]  * m_in[9]  * m_in[15] -
                        m_in[4]  * m_in[11] * m_in[13] -
                        m_in[8]  * m_in[5]  * m_in[15] +
                        m_in[8]  * m_in[7]  * m_in[13] +
                        m_in[12] * m_in[5]  * m_in[11] -
                        m_in[12] * m_in[7]  * m_in[9];

            invOut[12] = -m_in[4]  * m_in[9]  * m_in[14] +
                          m_in[4]  * m_in[10] * m_in[13] +
                          m_in[8]  * m_in[5]  * m_in[14] -
                          m_in[8]  * m_in[6]  * m_in[13] -
                          m_in[12] * m_in[5]  * m_in[10] +
                          m_in[12] * m_in[6]  * m_in[9];

            invOut[1] = -m_in[1]  * m_in[10] * m_in[15] +
                         m_in[1]  * m_in[11] * m_in[14] +
                         m_in[9]  * m_in[2]  * m_in[15] -
                         m_in[9]  * m_in[3]  * m_in[14] -
                         m_in[13] * m_in[2]  * m_in[11] +
                         m_in[13] * m_in[3]  * m_in[10];

            invOut[5] = m_in[0]  * m_in[10] * m_in[15] -
                        m_in[0]  * m_in[11] * m_in[14] -
                        m_in[8]  * m_in[2]  * m_in[15] +
                        m_in[8]  * m_in[3]  * m_in[14] +
                        m_in[12] * m_in[2]  * m_in[11] -
                        m_in[12] * m_in[3]  * m_in[10];

            invOut[9] = -m_in[0]  * m_in[9]  * m_in[15] +
                         m_in[0]  * m_in[11] * m_in[13] +
                         m_in[8]  * m_in[1]  * m_in[15] -
                         m_in[8]  * m_in[3]  * m_in[13] -
                         m_in[12] * m_in[1]  * m_in[11] +
                         m_in[12] * m_in[3]  * m_in[9];

            invOut[13] = m_in[0]  * m_in[9]  * m_in[14] -
                         m_in[0]  * m_in[10] * m_in[13] -
                         m_in[8]  * m_in[1]  * m_in[14] +
                         m_in[8]  * m_in[2]  * m_in[13] +
                         m_in[12] * m_in[1]  * m_in[10] -
                         m_in[12] * m_in[2]  * m_in[9];

            invOut[2] = m_in[1]  * m_in[6]  * m_in[15] -
                        m_in[1]  * m_in[7]  * m_in[14] -
                        m_in[5]  * m_in[2]  * m_in[15] +
                        m_in[5]  * m_in[3]  * m_in[14] +
                        m_in[13] * m_in[2]  * m_in[7] -
                        m_in[13] * m_in[3]  * m_in[6];

            invOut[6] = -m_in[0]  * m_in[6]  * m_in[15] +
                         m_in[0]  * m_in[7]  * m_in[14] +
                         m_in[4]  * m_in[2]  * m_in[15] -
                         m_in[4]  * m_in[3]  * m_in[14] -
                         m_in[12] * m_in[2]  * m_in[7] +
                         m_in[12] * m_in[3]  * m_in[6];

            invOut[10] = m_in[0]  * m_in[5]  * m_in[15] -
                         m_in[0]  * m_in[7]  * m_in[13] -
                         m_in[4]  * m_in[1]  * m_in[15] +
                         m_in[4]  * m_in[3]  * m_in[13] +
                         m_in[12] * m_in[1]  * m_in[7] -
                         m_in[12] * m_in[3]  * m_in[5];

            invOut[14] = -m_in[0]  * m_in[5]  * m_in[14] +
                          m_in[0]  * m_in[6]  * m_in[13] +
                          m_in[4]  * m_in[1]  * m_in[14] -
                          m_in[4]  * m_in[2]  * m_in[13] -
                          m_in[12] * m_in[1]  * m_in[6] +
                          m_in[12] * m_in[2]  * m_in[5];

            invOut[3] = -m_in[1] * m_in[6] * m_in[11] +
                         m_in[1] * m_in[7] * m_in[10] +
                         m_in[5] * m_in[2] * m_in[11] -
                         m_in[5] * m_in[3] * m_in[10] -
                         m_in[9] * m_in[2] * m_in[7] +
                         m_in[9] * m_in[3] * m_in[6];

            invOut[7] = m_in[0] * m_in[6] * m_in[11] -
                        m_in[0] * m_in[7] * m_in[10] -
                        m_in[4] * m_in[2] * m_in[11] +
                        m_in[4] * m_in[3] * m_in[10] +
                        m_in[8] * m_in[2] * m_in[7] -
                        m_in[8] * m_in[3] * m_in[6];

            invOut[11] = -m_in[0] * m_in[5] * m_in[11] +
                          m_in[0] * m_in[7] * m_in[9] +
                          m_in[4] * m_in[1] * m_in[11] -
                          m_in[4] * m_in[3] * m_in[9] -
                          m_in[8] * m_in[1] * m_in[7] +
                          m_in[8] * m_in[3] * m_in[5];

            invOut[15] = m_in[0] * m_in[5] * m_in[10] -
                         m_in[0] * m_in[6] * m_in[9] -
                         m_in[4] * m_in[1] * m_in[10] +
                         m_in[4] * m_in[2] * m_in[9] +
                         m_in[8] * m_in[1] * m_in[6] -
                         m_in[8] * m_in[2] * m_in[5];

            float det = m_in[0] * invOut[0] + m_in[1] * invOut[4] + m_in[2] * invOut[8] + m_in[3] * invOut[12];
            if (det == 0.0f) return Mat4::identity();

            float invDet = 1.0f / det;
            for (int i = 0; i < 16; i++) {
                inv.m[i] *= invDet;
            }

            return inv;
        }
    };

} // namespace LeviMod

#endif // MATRIX_MATH_H
