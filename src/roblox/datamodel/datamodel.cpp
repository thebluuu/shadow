#include "../rbx_engine.h"
#include "C:\Users\Caoimhin\Desktop\Main src\idk what to name yet lol\src\driver/driver_impl.hpp"
#include "../../misc/output/output.hpp"

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
#include "../offsets/offsets.hpp"

int_fast64_t RBX::Instance::GetGameId() {
    int_fast64_t game_id = driver::read<int_fast64_t>(this->address + Offsets::GameID);
    return (game_id > 0 && game_id <= 9999999999) ? game_id : 0;
}

RBX::Instance RBX::Instance::GetDataModelPTR() {
    RBX::Instance Instance2;
    return static_cast<RBX::Instance>(driver::read<uintptr_t>(Instance2.GetRenderView() + Offsets::DataModel));
}

RBX::Instance RBX::Instance::GetDataModel() {
    RBX::TaskScheduler Instance;
    RBX::Instance Instance2;
    std::uint64_t datamodel = driver::read<uintptr_t>(Instance2.GetRenderView() + Offsets::DataModel);
    return static_cast<RBX::Instance>(driver::read<std::uint64_t>(datamodel + Offsets::Game));
}


int_fast64_t RBX::Instance::GetCurrentPlaceId() {
    return driver::read<int_fast64_t>(this->address + 0x170);
}

std::vector<RBX::Instance> RBX::Instance::GetPlayerList() {
    std::vector<RBX::Instance> players;
    RBX::Instance playerService = this->FindFirstChild("Players", false, {});
    if (playerService.address == 0) return players;

    return playerService.GetChildren(false);
}

bool RBX::Instance::IsInGame() {
    return this->GetPlayersService().GetChildren(false).size()> 1;
    std::cout << "player size" << GetPlayersService().GetChildren(false).size() << std::endl;
    
}


int RBX::Instance::GetPlayerCount() {
    std::vector<RBX::Instance> players = this->GetPlayerList();
    return players.size();
}


std::string RBX::Instance::GetJobId() {
    uint64_t jobIdPtr = driver::read<uint64_t>(this->address + 0x110);
    if (jobIdPtr == 0) return "null id ptr";

    std::vector<char> jobId = driver::read_array<char>(jobIdPtr, 64);

    size_t nullPos = std::find(jobId.begin(), jobId.end(), '\0') - jobId.begin();
    jobId.resize(nullPos);

    return std::string(jobId.begin(), jobId.end());
}

