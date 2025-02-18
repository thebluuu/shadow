#include "../roblox/rbx_engine.h"
#include "../misc/globals/globals.hpp"


#include "../roblox/offsets/offsets.hpp"
#include "../driver/driver_impl.hpp"

#include <Windows.h>
#include <iomanip>
#include <sstream>
#include <thread>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <cmath>

#include <chrono>
namespace RBX {
	void hook_esp();
    //void SilentAimInit();
    void hook_exploits();
	void InitializeAimbot();
	void initsilent();

}
extern RBX::PlayerInstance aimbot_target;
extern RBX::PlayerInstance aimbot_target2;