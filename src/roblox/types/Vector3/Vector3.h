#pragma once
#include <cmath>

namespace RBX {

    struct Vector3 final {
        float x, y, z;

        Vector3() noexcept : x(0), y(0), z(0) {}

        Vector3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}

        inline const float& operator[](int i) const noexcept {
            return reinterpret_cast<const float*>(this)[i];
        }

        inline float& operator[](int i) noexcept {
            return reinterpret_cast<float*>(this)[i];
        }

        Vector3 operator/(float s) const noexcept {
            return *this * (1.0f / s);
        }

        float dot(const Vector3& vec) const noexcept {
            return x * vec.x + y * vec.y + z * vec.z;
        }

        float distance(const Vector3& vector) const noexcept {
            return std::sqrt((vector.x - x) * (vector.x - x) +
                (vector.y - y) * (vector.y - y) +
                (vector.z - z) * (vector.z - z));
        }

        Vector3 operator*(float value) const noexcept {
            return { x * value, y * value, z * value };
        }

        bool operator!=(const Vector3& other) const noexcept {
            return x != other.x || y != other.y || z != other.z;
        }

        float squared() const noexcept {
            return x * x + y * y + z * z;
        }

        Vector3 normalize() const noexcept {
            float mag = magnitude();
            if (mag == 0) return { 0, 0, 0 };
            return { x / mag, y / mag, z / mag };
        }

        Vector3 direction() const noexcept {
            return normalize();
        }

        static const Vector3& one() noexcept {
            static const Vector3 v(1, 1, 1);
            return v;
        }

        static const Vector3& unitX() noexcept {
            static const Vector3 v(1, 0, 0);
            return v;
        }

        static const Vector3& unitY() noexcept {
            static const Vector3 v(0, 1, 0);
            return v;
        }

        static const Vector3& unitZ() noexcept {
            static const Vector3 v(0, 0, 1);
            return v;
        }

        Vector3 cross(const Vector3& b) const noexcept {
            return { y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x };
        }

        Vector3 operator+(const Vector3& vec) const noexcept {
            return { x + vec.x, y + vec.y, z + vec.z };
        }

        Vector3 operator-(const Vector3& vec) const noexcept {
            return { x - vec.x, y - vec.y, z - vec.z };
        }

        Vector3 operator*(const Vector3& vec) const noexcept {
            return { x * vec.x, y * vec.y, z * vec.z };
        }

        Vector3 operator/(const Vector3& vec) const noexcept {
            return { x / vec.x, y / vec.y, z / vec.z };
        }

        Vector3& operator+=(const Vector3& vec) noexcept {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            return *this;
        }

        Vector3& operator-=(const Vector3& vec) noexcept {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
            return *this;
        }

        Vector3& operator*=(float fScalar) noexcept {
            x *= fScalar;
            y *= fScalar;
            z *= fScalar;
            return *this;
        }

        Vector3& operator/=(const Vector3& other) noexcept {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            return *this;
        }

        bool operator==(const Vector3& other) const noexcept {
            return x == other.x && y == other.y && z == other.z;
        }

        float magnitude() const noexcept {
            return std::sqrt(x * x + y * y + z * z);
        }
    };

}