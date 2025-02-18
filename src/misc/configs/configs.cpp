#include "configs.hpp"

#include <iostream>
#include <fstream>
#include <ShlObj.h>

#include "../globals/globals.hpp"
#include "../../library/json/json.hpp"

using namespace nlohmann;

void replace(std::string& str, const std::string& from, const std::string& to) {
    std::size_t start_position = str.find(from);
    if (start_position == std::string::npos) return;
    str.replace(start_position, from.length(), to);
}

void save_file(const std::string& path, const std::string& data) {
    std::ofstream out_file;
    out_file.open(path);
    out_file << data;
    out_file.close();
}

void load_file(const std::string& path, std::string& data) {
    std::ifstream in_file;
    in_file.open(path);
    in_file >> data;
    in_file.close();
}

template<typename T>
void load_cfg_option(T& globalsetting, const json& cfg_obj, const std::string& cfg_setting) {
    if (!cfg_obj.contains(cfg_setting)) return;
    globalsetting = cfg_obj[cfg_setting];
}


void RBX::configs::save(const char* name)
{
    json config;

#define SAVE_GLOBAL(var) config[#var] = globals::var;

    SAVE_GLOBAL(chams_opacity)
        SAVE_GLOBAL(menu_glow)
        SAVE_GLOBAL(tracer_esp)
        SAVE_GLOBAL(skeletonThickness)
        SAVE_GLOBAL(health_bar_text)
        SAVE_GLOBAL(silentaim)
        SAVE_GLOBAL(glow_opacity)
        SAVE_GLOBAL(glow_size)
        SAVE_GLOBAL(HeavyOptimize)
        SAVE_GLOBAL(skeletonOutline)
        SAVE_GLOBAL(crosshair_gap)
        SAVE_GLOBAL(crosshair_size)
        SAVE_GLOBAL(crosshair)
        SAVE_GLOBAL(alpha)
        SAVE_GLOBAL(crosshair_speed)
        SAVE_GLOBAL(OptimizeCache)
        SAVE_GLOBAL(selected_player)
        SAVE_GLOBAL(fonttype)
        SAVE_GLOBAL(tool_outline)
        SAVE_GLOBAL(fasttools)
        SAVE_GLOBAL(distancecheck)
        SAVE_GLOBAL(offscreen_Check)
        SAVE_GLOBAL(spectate_target)
        SAVE_GLOBAL(onlylocal)
        SAVE_GLOBAL(autoparry)
        SAVE_GLOBAL(skeleton_esp)
        SAVE_GLOBAL(onlytarget)
        SAVE_GLOBAL(chams)
        SAVE_GLOBAL(camera_fov)
        SAVE_GLOBAL(mouse_service)
        SAVE_GLOBAL(fill_box)
        SAVE_GLOBAL(flame_bar)
        SAVE_GLOBAL(triggerbot_delay)
        SAVE_GLOBAL(fill_chams)
        SAVE_GLOBAL(threadcrash)
        SAVE_GLOBAL(high_cpu_usage)
        SAVE_GLOBAL(mostFreq)
        SAVE_GLOBAL(teambasedcolor)
        SAVE_GLOBAL(localplayercheck)
        SAVE_GLOBAL(tool_esp)
        SAVE_GLOBAL(noclip)
        SAVE_GLOBAL(aimbot)
        SAVE_GLOBAL(rescan)
        SAVE_GLOBAL(draw_aimbot_fov)
        SAVE_GLOBAL(aimbot_fov_size)
        SAVE_GLOBAL(fly)
        SAVE_GLOBAL(max_aimbot_distance)
        SAVE_GLOBAL(triggerbot)
        SAVE_GLOBAL(triggerbot_sticky)
        SAVE_GLOBAL(draw_triggerbot_fov)
        SAVE_GLOBAL(esp)
        SAVE_GLOBAL(max_render_distance)
        SAVE_GLOBAL(box_esp)
        SAVE_GLOBAL(health_bar)
        SAVE_GLOBAL(shield_bar)
        SAVE_GLOBAL(name_esp)
        SAVE_GLOBAL(name_outline)
        SAVE_GLOBAL(distance_esp)
        SAVE_GLOBAL(distance_outline)
        SAVE_GLOBAL(nojumpcooldown)
        SAVE_GLOBAL(walkspeed_amount)
        SAVE_GLOBAL(JumpPower)
        SAVE_GLOBAL(fly_speed)
        SAVE_GLOBAL(force_projection_update)
        SAVE_GLOBAL(highcpuusageesp)
        SAVE_GLOBAL(vsync)
        SAVE_GLOBAL(streamproof)
        SAVE_GLOBAL(font_type)
        SAVE_GLOBAL(chams_outline)
        SAVE_GLOBAL(debug_info)
        SAVE_GLOBAL(use_class_names)

        save_file(appdata_path() + "\\Shadow\\configs\\" + static_cast<std::string>(name) + ".cfg", config.dump());
}

void RBX::configs::load(const char* name)
{
    std::string cfg_json = "";
    load_file(appdata_path() + "\\Shadow\\configs\\" + static_cast<std::string>(name) + ".cfg", cfg_json);

    if (!cfg_json[0]) return;

    json config = json::parse(cfg_json);

#define LOAD_GLOBAL(var) if (config.contains(#var)) globals::var = config[#var];

    LOAD_GLOBAL(chams_opacity)
        LOAD_GLOBAL(menu_glow)
        LOAD_GLOBAL(tracer_esp)
        LOAD_GLOBAL(skeletonThickness)
        LOAD_GLOBAL(health_bar_text)
        LOAD_GLOBAL(silentaim)
        LOAD_GLOBAL(glow_opacity)
        LOAD_GLOBAL(glow_size)
        LOAD_GLOBAL(HeavyOptimize)
        LOAD_GLOBAL(skeletonOutline)
        LOAD_GLOBAL(crosshair_gap)
        LOAD_GLOBAL(crosshair_size)
        LOAD_GLOBAL(crosshair)
        LOAD_GLOBAL(alpha)
        LOAD_GLOBAL(crosshair_speed)
        LOAD_GLOBAL(OptimizeCache)
        LOAD_GLOBAL(selected_player)
        LOAD_GLOBAL(fonttype)
        LOAD_GLOBAL(tool_outline)
        LOAD_GLOBAL(fasttools)
        LOAD_GLOBAL(distancecheck)
        LOAD_GLOBAL(offscreen_Check)
        LOAD_GLOBAL(spectate_target)
        LOAD_GLOBAL(onlylocal)
        LOAD_GLOBAL(autoparry)
        LOAD_GLOBAL(skeleton_esp)
        LOAD_GLOBAL(onlytarget)
        LOAD_GLOBAL(chams)
        LOAD_GLOBAL(camera_fov)
        LOAD_GLOBAL(mouse_service)
        LOAD_GLOBAL(fill_box)
        LOAD_GLOBAL(flame_bar)
        LOAD_GLOBAL(triggerbot_delay)
        LOAD_GLOBAL(fill_chams)
        LOAD_GLOBAL(threadcrash)
        LOAD_GLOBAL(high_cpu_usage)
        LOAD_GLOBAL(mostFreq)
        LOAD_GLOBAL(teambasedcolor)
        LOAD_GLOBAL(localplayercheck)
        LOAD_GLOBAL(tool_esp)
        LOAD_GLOBAL(noclip)
        LOAD_GLOBAL(aimbot)
        LOAD_GLOBAL(rescan)
        LOAD_GLOBAL(draw_aimbot_fov)
        LOAD_GLOBAL(aimbot_fov_size)
        LOAD_GLOBAL(fly)
        LOAD_GLOBAL(max_aimbot_distance)
        LOAD_GLOBAL(triggerbot)
        LOAD_GLOBAL(triggerbot_sticky)
        LOAD_GLOBAL(draw_triggerbot_fov)
        LOAD_GLOBAL(esp)
        LOAD_GLOBAL(max_render_distance)
        LOAD_GLOBAL(box_esp)
        LOAD_GLOBAL(health_bar)
        LOAD_GLOBAL(shield_bar)
        LOAD_GLOBAL(name_esp)
        LOAD_GLOBAL(name_outline)
        LOAD_GLOBAL(distance_esp)
        LOAD_GLOBAL(distance_outline)
        LOAD_GLOBAL(nojumpcooldown)
        LOAD_GLOBAL(walkspeed_amount)
        LOAD_GLOBAL(JumpPower)
        LOAD_GLOBAL(fly_speed)
}
