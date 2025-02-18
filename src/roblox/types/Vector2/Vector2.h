#pragma once
#include <cmath>

namespace RBX {

    struct Vector2 final {
        float x, y;

        Vector2 operator-(const Vector2& other) const noexcept {
            return { x - other.x, y - other.y };
        }

        Vector2 operator+(const Vector2& other) const noexcept {
            return { x + other.x, y + other.y };
        }

        Vector2 operator/(float factor) const noexcept {
            return { x / factor, y / factor };
        }

        Vector2 operator/(const Vector2& factor) const noexcept {
            return { x / factor.x, y / factor.y };
        }

        Vector2 operator*(float factor) const noexcept {
            return { x * factor, y * factor };
        }

        Vector2 operator*(const Vector2& factor) const noexcept {
            return { x * factor.x, y * factor.y };
        }

        float getMagnitude() const noexcept {
            return std::sqrt(x * x + y * y);
        }
    };

}