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

namespace RBX {

    struct CFrame {
        Vector3 right_vector = { 1, 0, 0 };
        Vector3 up_vector = { 0, 1, 0 };
        Vector3 back_vector = { 0, 0, 1 };
        Vector3 position = { 0, 0, 0 };

        CFrame() = default;

        CFrame(Vector3 position) : position{ position } {}

        CFrame(Vector3 right_vector, Vector3 up_vector, Vector3 back_vector, Vector3 position)
            : right_vector{ right_vector }, up_vector{ up_vector }, back_vector{ back_vector }, position{ position } {}

        void look_at_locked(Vector3 point) noexcept {

            Vector3 look_vector = (position - point).normalize();

            right_vector = Vector3{ 0, 1, 0 }.cross(look_vector).normalize();

            up_vector = look_vector.cross(right_vector).normalize();

            back_vector = look_vector * Vector3{ -1, -1, -1 };
        }

        CFrame look_at(Vector3 point) noexcept {

            Vector3 look_vector = (position - point).normalize();

            Vector3 right_vector = Vector3{ 0, 1, 0 }.cross(look_vector).normalize();
            Vector3 up_vector = look_vector.cross(right_vector).normalize();

            return CFrame{ right_vector, up_vector, look_vector, position };
        }

        CFrame operator*(const CFrame& cframe) const noexcept {

            CFrame result;

            result.right_vector = right_vector * cframe.right_vector;
            result.up_vector = up_vector * cframe.up_vector;
            result.back_vector = back_vector * cframe.back_vector;

            result.position = right_vector * cframe.position + position;

            return result;
        }

        Vector3 operator*(const Vector3& vec) const noexcept {

            return {
                right_vector.x * vec.x + right_vector.y * vec.y + right_vector.z * vec.z + position.x,
                up_vector.x * vec.x + up_vector.y * vec.y + up_vector.z * vec.z + position.y,
                back_vector.x * vec.x + back_vector.y * vec.y + back_vector.z * vec.z + position.z
            };
        }
    };

}