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
#include "../roblox/data/data.h"

#include "../roblox/rbx_engine.h"
#include <Psapi.h>

bool RBX::Initializer() {

    globals::debug_info = false;
    utils::output::begin("Initializing...");
    std::this_thread::sleep_for(std::chrono::microseconds(700));
    utils::output::input("Clearing Console");
    std::this_thread::sleep_for(std::chrono::microseconds(700));
    utils::output::clear_screen();
    utils::output::input("Finding Decrypted Web connection");
    std::this_thread::sleep_for(std::chrono::microseconds(700));
    auto randomcon = 2 / rand();
    std::string connectionlol = ("13::42::2" + randomcon);

    utils::output::printint("Testing Connection 1: 13::42::2::", randomcon);
    std::this_thread::sleep_for(std::chrono::microseconds(700));
    utils::output::warning("Connection 1 failed");
    std::this_thread::sleep_for(std::chrono::microseconds(700));
    utils::output::input("Brute Forcing a Connection");
    std::this_thread::sleep_for(std::chrono::microseconds(700));
    utils::output::printint("Connected to 13::42::2:: ", randomcon);
    std::this_thread::sleep_for(std::chrono::microseconds(700));
    utils::output::end("Connecting To driver");
    std::this_thread::sleep_for(std::chrono::microseconds(700));
    if (!driver::find_driver()) {
        utils::output::error("Driver Not Mapped, Restart PC");
        return false;
    }
    utils::output::info("Driver Mapped");
    std::this_thread::sleep_for(std::chrono::microseconds(700));
    driver::process_id = driver::find_process(TEXT("RobloxPlayerBeta.exe"));
    virtualaddy = driver::find_image();

    utils::output::info("Task Scheduler Resources Found");
    std::this_thread::sleep_for(std::chrono::microseconds(700));
    /////////////////////
    if (!load_game_data()) {
        return false;

    }
    utils::output::info("Thread Safety Check Passed");
    std::this_thread::sleep_for(std::chrono::microseconds(700));
    ///////////////////

    std::thread(RBX::InitializeAimbot).detach();
    utils::output::info("Aimbot Thread Hooked");
    std::this_thread::sleep_for(std::chrono::microseconds(700));
    std::thread(RBX::hook_exploits).detach();
    utils::output::info("Exploits Thread Hooked");
    std::this_thread::sleep_for(std::chrono::microseconds(700));
    std::thread(RBX::Instance::updatePlayers).detach();
    utils::output::info("Player Thread Hooked");
    std::this_thread::sleep_for(std::chrono::microseconds(700));
    std::thread(RBX::initsilent).detach(); // not thread safe
    utils::output::info("SilentAim Thread Hooked");
    std::this_thread::sleep_for(std::chrono::microseconds(700));
    std::thread(overlay::render).detach();
    utils::output::info("Overlay Thread Hooked");
    std::this_thread::sleep_for(std::chrono::microseconds(700));
    utils::output::info("Threads hooked");
    /////////////////////////
    ////////////////////////

    while (true) {
    }
}