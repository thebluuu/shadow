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
// add get current camera type and set current camera type

RBX::Instance RBX::Instance::GetWorkspace() {
	return globals::game.GetService(("Workspace"));
}
RBX::Instance RBX::Instance::GetWorkspaceByOffset() {
	return driver::read<RBX::Instance>(globals::game.address + 0x150);
}
RBX::Instance RBX::Instance::GetCurrentCamera() {
	return driver::read<RBX::Instance>(globals::camera.address + 0x248);
}
