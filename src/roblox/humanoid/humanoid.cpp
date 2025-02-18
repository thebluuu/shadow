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
#include "../../Graphics/overlay/overlay.hpp"

union convertion
{
    std::uint64_t hex;
    float f;
} conv;

float RBX::Instance::GetHealth() const {
    auto one = driver::read<std::uint64_t>(this->address + Offsets::Health);
    auto two = driver::read<std::uint64_t>(driver::read<std::uint64_t>(this->address + Offsets::Health));
    conv.hex = one ^ two;
    return conv.f;
}

std::string  RBX::Instance::getPlayerDisplayName() {
    uint64_t name = driver::read<uint64_t>(this->address + 0xC8);
   if (!name) {
       return "Nigga";
   }
   else {
       return driver::ReadString(name);
   }
}
void RBX::Instance::SetHumanoidWalkSpeed(float WalkSpeed)
{
    driver::write<float>(this->address + Offsets::WalkSpeed1, WalkSpeed);
    driver::write<float>(this->address + Offsets::WalkSpeed2, WalkSpeed);
    return;
}
void RBX::Instance::SetHumanoidJumpPower(float JumpPower)
{
    driver::write<float>(this->address + Offsets::JumpPower1, JumpPower);
    return;
}
float RBX::Instance::GetMaxHealth() const {
    auto one = driver::read<std::uint64_t>(this->address + Offsets::Max_Health);
    auto two = driver::read<std::uint64_t>(driver::read<std::uint64_t>(this->address + Offsets::Max_Health));

    conv.hex = one ^ two;
    return conv.f;
}

int RBX::Instance::getHumanoidState() const {
    return driver::read<int>(this->address + 0x850);
}

bool RBX::Instance::isHumanoidInIdleState() const {
    return getHumanoidState() == 0;
}

bool RBX::Instance::isHumanoidInRunningState() const {
    return getHumanoidState() == 1;
}

bool RBX::Instance::isHumanoidInSwimmingState() const {
    return getHumanoidState() == 2;
}
