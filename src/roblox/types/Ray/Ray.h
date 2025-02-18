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

namespace RBX {
    struct RaycastResult {
        bool hit;
        Vector3 hitPosition;
        Vector3 normal;
        float distance;
        void* hitObject;
    };

    class Ray   {
    public:
        static RaycastResult cast_ray(Vector3 origin, Vector3 direction, float maxDistance, const std::vector<Vector3>& objects) {
            RaycastResult result{ false, Vector3(), Vector3(), maxDistance, nullptr };
            for (const auto& obj : objects) {
                Vector3 toObject = obj - origin;
                float projLength = toObject.dot(direction.normalize());
                if (projLength < 0 || projLength > maxDistance) continue;
                Vector3 projectedPoint = origin + direction.normalize() * projLength;
                if ((projectedPoint - obj).magnitude() < 1.0f) {
                    result.hit = true;
                    result.hitPosition = projectedPoint;
                    result.normal = (projectedPoint - obj).normalize();
                    result.distance = projLength;
                    result.hitObject = (void*)&obj;
                    break;
                }
            }
            return result;
        }

        static std::vector<RaycastResult> cast_ray_multi(Vector3 origin, Vector3 direction, float maxDistance, const std::vector<Vector3>& objects) {
            std::vector<RaycastResult> results;
            for (const auto& obj : objects) {
                Vector3 toObject = obj - origin;
                float projLength = toObject.dot(direction.normalize());
                if (projLength < 0 || projLength > maxDistance) continue;
                Vector3 projectedPoint = origin + direction.normalize() * projLength;
                if ((projectedPoint - obj).magnitude() < 1.0f) {
                    results.push_back({ true, projectedPoint, (projectedPoint - obj).normalize(), projLength, (void*)&obj });
                }
            }
            return results;
        }

        static bool is_point_inside_object(Vector3 point, const std::vector<Vector3>& objects) {
            for (const auto& obj : objects) {
                if ((point - obj).magnitude() < 1.0f) {
                    return true;
                }
            }
            return false;
        }
    };
}
// used G3D to help me with some of this