#include "../rbx_engine.h"
#include "../../Graphics/overlay/overlay.hpp"

#include "../../driver/driver_impl.hpp"
#include "../../misc/output/output.hpp"
#include "../offsets/Offsets.hpp"
#include "../../misc/globals/globals.hpp"

#include <windows.h>
#include <TlHelp32.h>
#include <string>
#include <iostream>
#include "../../misc/output/output.hpp"
#include "../rbx_engine.h"
#include <windows.h>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <fstream>
#include <wininet.h>
#include <string>
#include <ShlObj.h>

static inline RBX::Vector3 ConvertColor(RBX::Vector3 color)
{
    // Roblox G3D Color3: if any channel is >1, assume input is in 0-255 range and normalize.
    if (color.x > 1.0f || color.y > 1.0f || color.z > 1.0f)
    {
        color.x /= 255.0f;
        color.y /= 255.0f;
        color.z /= 255.0f;
    }
    return color;
}

static inline RBX::Vector3 ConvertFogColor(RBX::Vector3 color)
{
    // Fog math in Roblox uses a different channel order.
    // If channels are >1, assume 0-255 input. Swap red and blue.
    if (color.x > 1.0f || color.y > 1.0f || color.z > 1.0f)
    {
        float red = color.x;
        float green = color.y;
        float blue = color.z;
        return RBX::Vector3(blue / 255.0f, green / 255.0f, red / 255.0f);
    }
    // Otherwise assume already normalized; swap red and blue.
    return RBX::Vector3(color.z, color.y, color.x);
}

RBX::Instance RBX::Instance::GetLighting() const {
    return globals::game.GetService(("Lighting"));
}

void RBX::Instance::setFogEnd(float value) {
    driver::write<float>(this->address + Offsets::Lighting_FogEnd, value);
}

void RBX::Instance::setFogStart(float value) {
    driver::write<float>(this->address + Offsets::Lighting_FogStart, value);
}

void RBX::Instance::setAmbientColor(RBX::Vector3 color) {
    driver::write<RBX::Vector3>(this->address + Offsets::Lighting_AmbientColor, ConvertColor(color));
}

void RBX::Instance::setOutdoorAmbient(RBX::Vector3 color) {
    driver::write<RBX::Vector3>(this->address + Offsets::Lighting_OutdoorAmbient, ConvertColor(color));
}

void RBX::Instance::setBrightness(float value) {
    driver::write<float>(this->address + Offsets::Lighting_Brightness, value);
}

void RBX::Instance::setGlobalShadows(bool enabled) {
    driver::write<bool>(this->address + 0x134, enabled);
}

void RBX::Instance::setBackDoorAmbienceColor(RBX::Vector3 color) {
    driver::write<RBX::Vector3>(this->address + 0xFC, ConvertColor(color));
}

void RBX::Instance::setColorShiftBottom(RBX::Vector3 color) {
    driver::write<RBX::Vector3>(this->address + 0xDC, ConvertColor(color));
}

void RBX::Instance::setColorShiftTop(RBX::Vector3 color) {
    driver::write<RBX::Vector3>(this->address + 0xE8, ConvertColor(color));
}

void RBX::Instance::setFogColor(RBX::Vector3 color) {
    driver::write<RBX::Vector3>(this->address + 0xF0, ConvertFogColor(color));
}

int RBX::Instance::GetColor3() {
    return driver::read<int>(this->address + 0x1770);
}

void RBX::Instance::setColor3(RBX::Vector3 color) {
    driver::write<RBX::Vector3>(this->address + 0x1770, ConvertColor(color));
}
