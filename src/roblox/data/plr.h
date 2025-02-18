#include "../rbx_engine.h"
#include "../../Graphics/overlay/overlay.hpp"
#include "../../driver/driver_impl.hpp"
#include "../../misc/output/output.hpp"
#include "../offsets/Offsets.hpp"
#include "../../misc/globals/globals.hpp"
#include <windows.h> 
#include <TlHelp32.h> 
#include <unordered_map>
#include <vector>

void plrdata() {

    std::vector<int> playersVal;
    playersVal.reserve(globals::players.GetChildren(false).size());

    for (int i = 0; i < 30; i++) {
        playersVal.push_back(globals::players.fetchPlayer(globals::players.address));
    }

    if (playersVal.empty()) {
        utils::output::error("players empty");
        return;
    }

    std::unordered_map<int, int> countMap;
    int maxCount = 0;

    for (int num : playersVal) {
        countMap[num]++;
        if (countMap[num] > maxCount) {
            maxCount = countMap[num];
            globals::mostFreq = num;
        }
    }
}