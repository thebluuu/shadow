#include "../Graphics/overlay/overlay.hpp"
#include "../driver/driver_impl.hpp"
#include "../misc/output/output.hpp"
#include "../roblox/offsets/offsets.hpp"
#include "../misc/globals/globals.hpp"
#include "../cheats/features.h"
#include <windows.h> 
#include <TlHelp32.h> 
#include <string> 
#include <iostream> 
#include "plr.h"
#include "../roblox/rbx_engine.h"
#include <Psapi.h>
#include "debugger.h"
#include "rbx_engine.h"
#include "../misc/log_system/log_system.h"

bool RBX::load_game_data()
{
    RBX::TaskScheduler Instance;
    RBX::Instance Instance2;

    globals::cached_players.clear();
    globals::players.GetChildren(false).clear();
    globals::DataModelPTR = Instance2.GetDataModelPTR();
    globals::game = Instance2.GetDataModel();
    globals::lighting = Instance2.GetLighting();
    globals::visualengine = Instance2.GetVisualEngine();
    globals::workspace = Instance2.GetWorkspaceByOffset();
    globals::mouse_service = globals::game.FindFirstChild("MouseService", false, {}).address;
    globals::players = Instance2.GetPlayersService();
    globals::game_id = globals::game.GetGameId();
    globals::localplayername = globals::players.GetLocalPlayer().GetName();

    std::string logMsg = "Player Count: " + std::to_string(globals::players.GetChildren(false).size());
    RBX::Log_System::Info(logMsg);
    std::string logMsgA = "DataModel Found At: " + std::to_string(globals::game.address);
    RBX::Log_System::Info(logMsgA);
    std::string logMsgB = "DataModelPTR Found At: " + std::to_string(globals::DataModelPTR.address);
    RBX::Log_System::Info(logMsgB);
    std::string logMsg2 = "players Found At: " + std::to_string(globals::players.address);
    RBX::Log_System::Info(logMsg2);
    std::string logMsg1 = "mouse service Found At: " + std::to_string(globals::mouse_service);
    RBX::Log_System::Info(logMsg1);
    //std::cout << logMsg << std::endl;

    plrdata();
    return true;
}

bool RBX::checkdriver() {
    try {
        if (!driver::find_driver()) {
            RBX::Log_System::Error("Driver Not Mapped, Restart PC");
            utils::output::error("Driver Not Mapped, Restart PC");
            return false;
        }

        driver::process_id = driver::find_process(TEXT("RobloxPlayerBeta.exe"));
        if (!driver::process_id) {
            RBX::Log_System::Error("Driver Proc Id Not Found, Restart PC");
            utils::output::error("Driver Proc Id Not Found, Restart PC");
            return false;
        }

        RBX::Log_System::Info("Driver successfully mapped and process ID found.");
        return true;
    }
    catch (const std::exception& e) {
        RBX::Log_System::Error(std::string("Exception occurred in checkdriver: ") + e.what());
        utils::output::error(std::string("Exception occurred in checkdriver: ") + e.what());
        return false;
    }
    catch (...) {
        RBX::Log_System::Error("Unknown exception occurred in checkdriver.");
        utils::output::error("Unknown exception occurred in checkdriver.");
        return false;
    }
}

void RBX::getRobloxId() {
    try {
        virtualaddy = driver::find_image();
        if (virtualaddy == 0) {
            RBX::Log_System::Error("Failed to find image base address.");
            utils::output::error("Failed to find image base address.");
            return;
        }

        std::string logMsg = "Base_Address: " + std::to_string((virtualaddy));
        RBX::Log_System::Info(logMsg);
        std::cout << logMsg << std::endl;
    }
    catch (const std::exception& e) {
        RBX::Log_System::Error(std::string("Exception occurred in getRobloxId: ") + e.what());
        utils::output::error(std::string("Exception occurred in getRobloxId: ") + e.what());
    }
    catch (...) {
        RBX::Log_System::Error("Unknown exception occurred in getRobloxId.");
        utils::output::error("Unknown exception occurred in getRobloxId.");
    }
}
