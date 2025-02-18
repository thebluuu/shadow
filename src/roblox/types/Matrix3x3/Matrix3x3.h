#pragma once
#include <string>
#include <vector>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <queue>
#include "../../rbx_engine.h"
#include "../Vector3/Vector3.h"
#include <algorithm>

#define M_PI 3.14159265358979323846

namespace RBX {

    struct Matrix3x3 final {
        float data[9];

        Vector3 MatrixToEulerAngles() const {
            Vector3 angles;

            angles.y = std::asin(std::clamp(data[6], -1.0f, 1.0f));

            if (std::abs(data[6]) < 0.9999f) {
                angles.x = std::atan2(-data[7], data[8]);
                angles.z = std::atan2(-data[3], data[0]);
            }
            else {
                angles.x = 0.0f;
                angles.z = std::atan2(data[1], data[4]);
            }

            angles.x = angles.x * (180.0f / M_PI);
            angles.y = angles.y * (180.0f / M_PI);
            angles.z = angles.z * (180.0f / M_PI);

            return angles;
        }

        Matrix3x3 EulerAnglesToMatrix(const Vector3& angles) const {
            Matrix3x3 rotationMatrix;

            float pitch = angles.x * (M_PI / 180.0f);
            float yaw = angles.y * (M_PI / 180.0f);
            float roll = angles.z * (M_PI / 180.0f);

            float cosPitch = std::cos(pitch), sinPitch = std::sin(pitch);
            float cosYaw = std::cos(yaw), sinYaw = std::sin(yaw);
            float cosRoll = std::cos(roll), sinRoll = std::sin(roll);

            rotationMatrix.data[0] = cosYaw * cosRoll;
            rotationMatrix.data[1] = cosYaw * sinRoll;
            rotationMatrix.data[2] = -sinYaw;

            rotationMatrix.data[3] = sinPitch * sinYaw * cosRoll - cosPitch * sinRoll;
            rotationMatrix.data[4] = sinPitch * sinYaw * sinRoll + cosPitch * cosRoll;
            rotationMatrix.data[5] = sinPitch * cosYaw;

            rotationMatrix.data[6] = cosPitch * sinYaw * cosRoll + sinPitch * sinRoll;
            rotationMatrix.data[7] = cosPitch * sinYaw * sinRoll - sinPitch * cosRoll;
            rotationMatrix.data[8] = cosPitch * cosYaw;

            return rotationMatrix;
        }

        Vector3 GetForwardVector() const { return { data[2], data[5], data[8] }; }
        Vector3 GetRightVector() const { return { data[0], data[3], data[6] }; }
        Vector3 GetUpVector() const { return { data[1], data[4], data[7] }; }

        Matrix3x3 Transpose() const {
            Matrix3x3 result;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    result.data[i + j * 3] = data[j + i * 3];
            return result;
        }

        Vector3 multiplyVector(const Vector3& vec) const {
            return {
                data[0] * vec.x + data[1] * vec.y + data[2] * vec.z,
                data[3] * vec.x + data[4] * vec.y + data[5] * vec.z,
                data[6] * vec.x + data[7] * vec.y + data[8] * vec.z
            };
        }

        Matrix3x3 operator*(const Matrix3x3& other) const {
            Matrix3x3 result;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    result.data[i + j * 3] = data[i * 3 + 0] * other.data[0 + j] +
                    data[i * 3 + 1] * other.data[1 + j] +
                    data[i * 3 + 2] * other.data[2 + j];
            return result;
        }

        Matrix3x3 operator+(const Matrix3x3& other) const {
            Matrix3x3 result;
            for (int i = 0; i < 9; ++i)
                result.data[i] = data[i] + other.data[i];
            return result;
        }

        Matrix3x3 operator-(const Matrix3x3& other) const {
            Matrix3x3 result;
            for (int i = 0; i < 9; ++i)
                result.data[i] = data[i] - other.data[i];
            return result;
        }

        Matrix3x3 operator/(float scalar) const {
            Matrix3x3 result;
            for (int i = 0; i < 9; ++i)
                result.data[i] = data[i] / scalar;
            return result;
        }

        Vector3 getColumn(int index) const { return { data[index], data[index + 3], data[index + 6] }; }
    };

    static Vector3 lookvec(const Matrix3x3& rotationMatrix) { return rotationMatrix.getColumn(2); }
    static Vector3 rightvec(const Matrix3x3& rotationMatrix) { return rotationMatrix.getColumn(0); }

}