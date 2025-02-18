#include "../rbx_engine.h"
#include "../../Graphics/overlay/overlay.hpp"




#include "../../driver/driver_impl.hpp"
#include "../../misc/output/output.hpp"
#include "../offsets/Offsets.hpp"
#include "../../misc/globals/globals.hpp"




#include <windows.h> // For Windows API functions like OpenProcess, GetModuleHandleEx, etc.
#include <TlHelp32.h> // For process and module enumeration (e.g., PROCESSENTRY32, MODULEENTRY32)
#include <string> // For working with strings
#include <iostream> // For debugging with std::cout (optional)
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
namespace RBX {
RBX::Vector2 RBX::Instance::GetDimensions()
{
    return driver::read<RBX::Vector2>(this->address + Offsets::Dimensions);
}

RBX::Matrix4x4 RBX::Instance::GetViewMatrix()
{
    return driver::read<RBX::Matrix4x4>(this->address + Offsets::View_Matrix);
}
RBX::Vector2 WorldToScreen(RBX::Vector3 world, RBX::Vector2 dimensions, RBX::Matrix4x4 viewmatrix) {
    RBX::Vector4 clipCoords = {
        world.x * viewmatrix.data[0] + world.y * viewmatrix.data[1] + world.z * viewmatrix.data[2] + viewmatrix.data[3],
        world.x * viewmatrix.data[4] + world.y * viewmatrix.data[5] + world.z * viewmatrix.data[6] + viewmatrix.data[7],
        world.x * viewmatrix.data[8] + world.y * viewmatrix.data[9] + world.z * viewmatrix.data[10] + viewmatrix.data[11],
        world.x * viewmatrix.data[12] + world.y * viewmatrix.data[13] + world.z * viewmatrix.data[14] + viewmatrix.data[15]
    };

    if (clipCoords.w <= 0.5f) return { -1.0f, -1.0f };

    float inv_w = 1.0f / clipCoords.w;
    RBX::Vector3 ndc = { clipCoords.x * inv_w, clipCoords.y * inv_w, clipCoords.z * inv_w };

    return {
        (dimensions.x / 2.0f) * (ndc.x + 1.0f),
        (dimensions.y / 2.0f) * (1.0f - ndc.y)
    };
}


int Instance::getPlayerTeamColor() {
    return driver::read<int>(this->address + 0xC8);
}


bool Instance::isPlayerVisible(Matrix4x4 viewMatrix) {
    Instance Instance;
    Vector3 position = Instance.GetPosition();
    Vector2 screenPosition = WorldToScreen(position, GetDimensions(), viewMatrix);
    return !(screenPosition.x == -1 && screenPosition.y == -1);
}
}