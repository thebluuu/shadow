#include <windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <wininet.h>
#include <ShlObj.h>
#include <TlHelp32.h>
#include <mutex>
#include "../../../misc/output/output.hpp"
#include "../../rbx_engine.h"
#include "../../../driver/driver_impl.hpp"
#include "../../../Graphics/overlay/overlay.hpp"
#include "../../offsets/Offsets.hpp"
#include "../../../misc/globals/globals.hpp"

namespace RBX {

    int Instance::fetchPlayer(std::uint64_t address) const {
        return driver::read<int>(address);
    }

    std::atomic<bool> running{ true };
    std::mutex cachedPlayersMutex;

    void syncList(std::vector<RBX::PlayerInstance>& oldList, const std::vector<RBX::PlayerInstance>& newList) {
        ::SetThreadPriority(::GetCurrentThread(), 0x80);
        if (newList.empty()) return;
        std::unordered_map<uintptr_t, RBX::PlayerInstance> newEntitiesMap;
        for (const auto& entity : newList)
            newEntitiesMap[entity.address] = entity;
        oldList.erase(
            std::remove_if(oldList.begin(), oldList.end(),
                [&newEntitiesMap](const RBX::PlayerInstance& item) {
                    return newEntitiesMap.find(item.address) == newEntitiesMap.end();
                }),
            oldList.end()
        );
        for (const auto& entity : newList) {
            auto it = std::find_if(oldList.begin(), oldList.end(),
                [&entity](const RBX::PlayerInstance& item) {
                    return item.address == entity.address;
                });
            if (it != oldList.end())
                *it = entity;
            else
                oldList.push_back(entity);
        }
    }
    bool isOnScreen(const RBX::PlayerInstance& player) {
        auto dimensions = globals::visualengine.GetDimensions();
        auto playerPosition = player.character.GetPosition();
        auto screenPosition = RBX::WorldToScreen(playerPosition, dimensions, globals::visualengine.GetViewMatrix());

        return (screenPosition.x >= 0 && screenPosition.x <= dimensions.x) &&
            (screenPosition.y >= 0 && screenPosition.y <= dimensions.y);
    }


    void Instance::updatePlayers() {
        // ::SetThreadPriority(::GetCurrentThread(), 0x80);
        std::unordered_set<std::uintptr_t> knownAddresses;
        auto& entityPool = globals::players;
        std::mutex updateMutex;

        std::vector<RBX::PlayerInstance> tempCache;
        tempCache.reserve(100);
        globals::camera = globals::workspace.FindFirstChild("Camera", false, {});
        std::this_thread::sleep_for(std::chrono::microseconds(935));

        while (running.load()) {


            if (entityPool.fetchPlayer(entityPool.address) != globals::mostFreq) {
                std::this_thread::sleep_for(std::chrono::microseconds(935));
                continue;
            }
            RBX::PlayerInstance LocalPlayer;
            LocalPlayer.aim = globals::players.GetLocalPlayer().FindFirstChild("PlayerGui", false, {}).FindFirstChild("MainScreenGui", true, {}).FindFirstChild("Aim", true, {});
            LocalPlayer.hc_aim = globals::players.GetLocalPlayer().FindFirstChild("PlayerGui", false, {}).FindFirstChild("Main Screen", true, {}).FindFirstChild("Aim", true, {});
            LocalPlayer.team = globals::players.GetLocalPlayer().GetTeam();
            globals::localplayer = LocalPlayer;
            knownAddresses.clear();
            tempCache.clear();



            auto entityList = entityPool.GetChildren(false);

            std::unordered_map<std::uintptr_t, RBX::PlayerInstance> partCache;

            for (const auto& entityInstance : entityList) {
                if ((globals::onlylocal && entityInstance.GetName() != globals::localplayername) ||
                    (!globals::localplayercheck && entityInstance.GetName() == globals::localplayername)) {
                    continue;
                }

                RBX::PlayerInstance currentEntity;
                currentEntity.name = entityInstance.GetName();
                currentEntity.character = entityInstance.GetModelInstance();
                std::this_thread::sleep_for(std::chrono::microseconds(globals::threadrestarttime));
                auto humanoidData = currentEntity.character.FindFirstChild("Humanoid", true, {});
                if (currentEntity.character.address == 0 || humanoidData.address == 0) {
                    continue;
                }

                currentEntity.humanoid = humanoidData;
                currentEntity.team = entityInstance.GetTeam();
                currentEntity.address = entityInstance.address;
                currentEntity.children = currentEntity.character.GetChildren(false);
                currentEntity.r15 = humanoidData.GetRigType();

                if (globals::tool_esp) {
                    currentEntity.currentTool = currentEntity.character.GetService("Tool");
                    currentEntity.currentToolName = currentEntity.character.GetService("Tool").GetName();
                }

                if (globals::cframe && !isOnScreen(currentEntity)) {
                    continue;
                }

                std::unordered_map<std::string, RBX::Instance&> partMap;

                if (currentEntity.r15 == 0) {
                    partMap = {
                        {"Torso", currentEntity.upperTorso},
                        {"Head", currentEntity.head},
                        {"HumanoidRootPart", currentEntity.rootPart},
                        {"Right Arm", currentEntity.rightUpperArm},
                        {"Left Arm", currentEntity.leftUpperArm},
                        {"Left Leg", currentEntity.leftUpperLeg},
                        {"Right Leg", currentEntity.rightUpperLeg}
                    };
                }
                else {
                    partMap = {
                        {"Head", currentEntity.head},
                        {"UpperTorso", currentEntity.upperTorso},
                        {"LowerTorso", currentEntity.lowerTorso},
                        {"HumanoidRootPart", currentEntity.rootPart},
                        {"LeftUpperLeg", currentEntity.leftUpperLeg},
                        {"RightUpperLeg", currentEntity.rightUpperLeg},
                        {"LeftLowerLeg", currentEntity.leftLowerLeg},
                        {"RightLowerLeg", currentEntity.rightLowerLeg},
                        {"LeftFoot", currentEntity.leftFoot},
                        {"RightFoot", currentEntity.rightFoot},
                        {"LeftUpperArm", currentEntity.leftUpperArm},
                        {"RightUpperArm", currentEntity.rightUpperArm},
                        {"LeftLowerArm", currentEntity.leftLowerArm},
                        {"RightLowerArm", currentEntity.rightLowerArm},
                        {"LeftHand", currentEntity.leftHand},
                        {"RightHand", currentEntity.rightHand}
                    };
                }

                for (auto& child : currentEntity.character.GetChildren(false)) {
                    auto it = partMap.find(child.GetName());
                    if (it != partMap.end()) {
                        it->second = child;
                        partCache[child.address] = currentEntity;
                    }
                }

                tempCache.push_back(currentEntity);
                knownAddresses.insert(currentEntity.address);
            }

            try {

                std::lock_guard<std::mutex> lock(updateMutex);

                globals::cached_players.erase(
                    std::remove_if(globals::cached_players.begin(), globals::cached_players.end(),
                        [&knownAddresses](const RBX::PlayerInstance& player) {
                            return knownAddresses.find(player.address) == knownAddresses.end();
                        }),
                    globals::cached_players.end());

                for (const auto& playerInstance : tempCache) {
                    if (knownAddresses.find(playerInstance.address) == knownAddresses.end()) {
                        globals::cached_players.push_back(playerInstance);
                    }
                }

                syncList(globals::cached_players, tempCache);

            }
            catch (const std::exception& e) {
                std::cerr << "Exception caught while updating players: " << e.what() << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::microseconds(globals::threadtime));
        }
    }


}

