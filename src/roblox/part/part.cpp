#include <windows.h>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <fstream>
#include <wininet.h>
#include <ShlObj.h>
#include "../rbx_engine.h"
#include "../../misc/output/output.hpp"
#include "../../driver/driver_impl.hpp"
#include "../offsets/Offsets.hpp"
#include "../../misc/globals/globals.hpp"
#include "../../Graphics/overlay/overlay.hpp"
#include "../enums/enum.h"

namespace RBX {
    void Instance::SetColor(const std::string& part_name, RBX::Vector3 color) {
        auto player = globals::players.GetLocalPlayer();
        auto character = player.GetModelInstance();
        auto part = character.FindFirstChild(part_name, true, {});

        std::uint64_t primitive = part.GetPart();
        if (primitive) {
            driver::write<RBX::Vector3>(primitive + 0x190, color);
        }
    }

    Instance Instance::SetAllColor(RBX::Vector3 color) {
        auto player = globals::players.GetLocalPlayer();
        auto character = player.GetModelInstance();
        for (auto part : character.GetChildren(false)) {
            SetColor(part.GetName(), color);
        }
        return *this;
    }
    void Instance::SetMaterial(const std::string& part_name) {
        auto player = globals::players.GetLocalPlayer();
        auto character = player.GetModelInstance();
        auto part = character.FindFirstChild(part_name, true, {});

        std::uint64_t primitive = part.GetPart();
        if (primitive) {
            driver::write<int16_t>(primitive + 0x2F0, static_cast<int16_t>(utils::enums::material::ForceField));

        }
    }

    Instance Instance::SetAllMaterial() {
        auto player = globals::players.GetLocalPlayer();
        auto character = player.GetModelInstance();
        for (auto part : character.GetChildren(false)) {
            SetMaterial(part.GetName());
        }
        return *this;
    }

    std::uint64_t Instance::GetPart() const {
        return driver::read<std::uint64_t>(this->address + Offsets::Part_Primitive);
    }

    int Instance::GetPartMaterial() const {
        return driver::read<int>(this->address + 0x2F0);
    }

    bool Instance::setCanCollide(bool value) {
        return driver::write<bool>(this->address + Offsets::CanCollide, value);
    }

    RBX::Vector3 Instance::GetPosition() const {
        return driver::read<RBX::Vector3>(GetPart() + Offsets::Part_Position);
    }

    RBX::Vector3 Instance::GetVelocity() const {
        return driver::read<RBX::Vector3>(GetPart() + Offsets::Part_Velocity);
    }

    RBX::Matrix3x3 Instance::GetRotation() const {
        return driver::read<RBX::Matrix3x3>(GetPart() + Offsets::Part_Rotation);
    }

    RBX::Vector3 Instance::GetSize() const {
        return driver::read<RBX::Vector3>(GetPart() + Offsets::Part_Size);
    }
    Vector3 Instance::SetSize(Vector3 size) const {
        return driver::write<Vector3>(GetPart() + Offsets::Part_Size, size);
    }
    RBX::CFrame Instance::GetCframe() {
        RBX::CFrame res{};
        std::uint64_t primitive = GetPart();
        if (primitive) {
            res = driver::read<RBX::CFrame>(primitive + Offsets::CFrame);
        }
        return res;
    }

} // namespace RBX
