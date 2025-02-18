#include <windows.h>
#include <string>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <fstream>
#include <wininet.h>
#include <ShlObj.h>
#include "../../misc/output/output.hpp"
#include "../rbx_engine.h"
#include "../../driver/driver_impl.hpp"
#include "../offsets/Offsets.hpp"
#include "../../misc/globals/globals.hpp"

namespace RBX {

    int Instance::getPlayerArmor() {
        for (const auto& player : globals::cached_players) {
            return driver::read<int>(player.armor_obj.address + Offsets::Value);
        }
        return 0;
    }

    RBX::Instance Instance::GetPlayersService() {
        return globals::game.GetService("Players");
    }

    RBX::Instance Instance::GetLocalPlayer() const {
        return driver::read<RBX::Instance>(this->address + Offsets::Local_Entity);
    }

    RBX::Instance Instance::GetModelInstance() const {
        return driver::read<RBX::Instance>(this->address + Offsets::Model_Instance);
    }

    std::int32_t Instance::GetRigType() {
        return driver::read<std::uint8_t>(this->address + Offsets::Humanoid_RigType);
    }

    RBX::Instance Instance::GetModelPrimaryPart() {
        return driver::read<RBX::Instance>(this->address + Offsets::Primary_Part);
    } // returns hrp

    RBX::Instance Instance::GetTeam() const {
        return driver::read<RBX::Instance>(this->address + Offsets::Team);
    }

    void Instance::SetPartPos(RBX::Vector3 position) {
        static Vector3 cached_position = { 0.0f, 0.0f, 0.0f };

        const float threshold = 0.001f;

        if ((position - cached_position).magnitude() > threshold) {
            auto primitive = driver::read<std::uint64_t>(this->address + Offsets::Part_Primitive);

            for (int i = 0; i < 10000; i++) {
                driver::write<Vector3>(primitive + Offsets::Part_Position, position);
            }

          cached_position = position;
        }
    }

    void Instance::writePositionMultipleTimes(std::uint64_t primitive, RBX::Vector3 position) {
    
        driver::write<RBX::Vector3>(primitive + Offsets::Part_Position, position);
        driver::write<RBX::Vector3>(primitive + Offsets::Part_Position, position);
        driver::write<RBX::Vector3>(primitive + Offsets::Part_Position, position);
        driver::write<RBX::Vector3>(primitive + Offsets::Part_Position, position);
        driver::write<RBX::Vector3>(primitive + Offsets::Part_Position, position);
        driver::write<RBX::Vector3>(primitive + Offsets::Part_Position, position);
        driver::write<RBX::Vector3>(primitive + Offsets::Part_Position, position);
        driver::write<RBX::Vector3>(primitive + Offsets::Part_Position, position);
        driver::write<RBX::Vector3>(primitive + Offsets::Part_Position, position);
        driver::write<RBX::Vector3>(primitive + Offsets::Part_Position, position);
    }



    void Instance::SetPartVelocity(RBX::Vector3 velocity) {
        static Vector3 cached_velocity = { 0.0f, 0.0f, 0.0f };
        auto primitive = driver::read<std::uint64_t>(this->address + Offsets::Part_Primitive);
        const float threshold = 0.001f;

        if ((velocity - cached_velocity).magnitude() > threshold) {
            auto start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < 1000; i++) {
                driver::write<Vector3>(primitive + Offsets::Part_Velocity, velocity);

                auto elapsed = std::chrono::high_resolution_clock::now() - start;
                if (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() >= 1) {
                    break;
                }
            }

            cached_velocity = velocity;
        }

        driver::write<Vector3>(primitive + Offsets::Part_Velocity, velocity);
    }


} // namespace RBX
