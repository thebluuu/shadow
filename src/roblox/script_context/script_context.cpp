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
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <fstream>
#include <wininet.h>
#include <ShlObj.h>

uint64_t RBX::Instance::GetScriptContext()
{
    auto HyrbidScriptsJob = RBX::TaskScheduler{}.GetJobByName("WaitingHybridScriptsJob");
    return driver::read<uint64_t>(HyrbidScriptsJob + 0x1F8);
}

uint64_t RBX::Instance::GetDataModelViaScriptContext()
{
    return driver::read<uint64_t>(GetScriptContext() + 0x50);
}
