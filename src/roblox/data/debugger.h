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

#include "../roblox/rbx_engine.h"
#include <Psapi.h>

void debug() {
    if (globals::debug_info) {
        RBX::TaskScheduler Instance;
        RBX::Instance Instance2;

        uintptr_t job_count = Instance.GetJobCount();
        auto Viewp = Instance2.GetViewportSize();
        uintptr_t renderjob = Instance.GetJobByName("RenderJob");
        uintptr_t renderaddress = driver::read<uintptr_t>(renderjob);
        uintptr_t renderview = Instance2.GetRenderView();
        uintptr_t ScriptContext = Instance2.GetScriptContext();
        uintptr_t ScriptContextDM = Instance2.GetDataModelViaScriptContext();

        if (Instance.GetScheduler() == 0) {
            utils::output::printaddress("Scheduler: ", (void*)Instance.GetScheduler());
        }
        if (renderaddress == 0) {
            utils::output::printaddress("Render_Job: ", (void*)renderaddress);
        }

        utils::output::info("Scheduler Thread Pool Size: " + std::to_string(job_count));
        std::pair<int, int> viewportSize = Instance2.GetViewportSize();
        utils::output::info("Viewport Size: " + std::to_string(viewportSize.first) + ", " + std::to_string(viewportSize.second));

        utils::output::printaddress("Render_View: ", (void*)renderview);
        utils::output::printaddress("Visual_Engine: ", (void*)globals::visualengine.address);
        utils::output::printaddress("Script_Context: ", (void*)ScriptContext);
        utils::output::printaddress("Script_Context To Data_Model: ", (void*)ScriptContextDM);
        utils::output::printaddress("Data_Model: ", (void*)globals::game.address);

        utils::output::info("Lplr Name: " + globals::players.GetLocalPlayer().GetName());
    }
}
