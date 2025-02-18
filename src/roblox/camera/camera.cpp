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
RBX::Instance RBX::Instance::GetCameraInstance()
{
    return driver::read<RBX::Instance>(this->address + Offsets::Camera);
}

RBX::Vector3 RBX::Instance::GetCameraPosition()
{
    return driver::read<RBX::Vector3>(this->address + Offsets::Camera_Position);
}
RBX::Matrix3x3 RBX::Instance::GetCameraRotation()
{
    return driver::read<RBX::Matrix3x3>(this->address + Offsets::Camera_Rotation);
}

float RBX::Instance::GetFov()
{
    return driver::read<float>(this->address + Offsets::Camera_FieldOfView);
}

void RBX::Instance::SetCameraRotation(RBX::Matrix3x3 Rotation)
{
    driver::write<RBX::Matrix3x3>(this->address + Offsets::Camera_Rotation, Rotation);
}
 
RBX::Instance RBX::Instance::Spectate(RBX::Instance stringhere) {
    RBX::Instance placeholder;
    driver::write<std::uint64_t>(globals::game.FindFirstChild("Workspace", false, {}).FindFirstChild("Camera", false, {}).address + Offsets::Value, stringhere.address);
    return placeholder;
}

RBX::Instance RBX::Instance::UnSpectate() {
    RBX::Instance placeholder;
    driver::write<std::uint64_t>(globals::game.FindFirstChild("Workspace", false, {}).FindFirstChild("Camera", false, {}).address + Offsets::Value,
        globals::players.GetLocalPlayer().GetModelInstance().FindFirstChild("Humanoid", false, {}).address);
    return placeholder;
}
//"CameraMaxZoomDistance": "0x240",
//""CameraMinZoomDistance" : "0x244",
//""CameraMode" : "0x248"
void RBX::Instance::SetCameraMaxZoom(int zoom) {
    driver::write<int>(globals::camera.address + 0x240, zoom);
}
void RBX::Instance::SetCameraMinZoom(int zoom) {
    driver::write<int>(globals::camera.address + 0x244, zoom);
}