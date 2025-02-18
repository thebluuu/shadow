#include "globals.hpp"


//#include "../../framework/Graphics/overlay/imgui//.hpp"
RBX::Instance  globals::DataModelPTR{};
RBX::Instance  globals::game{};
std::string  globals::team{};
RBX::WorkSpace  globals::game1{};
std::uint64_t globals::mouse_service{};
RBX::Instance globals::visualengine{};
RBX::Instance globals::players{};
RBX::Instance globals::workspace{};
RBX::Instance globals::lighting{};
RBX::Instance globals::camera{};
RBX::PlayerInstance globals::target{};
RBX::Instance globals::aim{};
bool globals::HeavyOptimize = false;
bool globals::health_bar_text = false;
bool globals::silentaim = false;
RBX::PlayerInstance globals::localplayer{};
std::vector<RBX::PlayerInstance> globals::cached_players{};
bool globals::onlytarget = false;
// OTHER STUFF
bool globals::chams = false;
bool globals::threadcrash = false;
bool globals::high_cpu_usage = false;
int globals::mostFreq = 0;
bool globals::dropped_items = false;
bool globals::vehicle_esp = false;

float g_Time = 0.0f;
float g_DeltaTime = 0.0f;
float globals::glow_size = 0.0f;
int globals::glow_opacity = .0f;
bool globals::skeleton_esp = false;
std::set<RBX::PlayerInstance> globals::cachedInstances;
bool globals::rescan = false;
HWND globals::window_handle = FindWindowA(NULL, ("Roblox"));
bool globals::autoparry = false;
// AIMBOT
int globals::fonttype = 0;
bool globals::tool_outline = true;


bool globals::fasttools = false;

CKeybind globals::fly_bind{ ("fly") };
bool globals::crosshair = false;
int globals::crosshair_gap = 15;
int globals::crosshair_size = 40;
int globals::crosshair_speed = 2;
bool globals::OptimizeCache = true;
int globals::selected_player = -1;
float globals::alpha = 1.0;
bool globals::spectate_target = false;
bool globals::onlylocal = false;
std::string globals::localplayername{};
int globals::triggerbot_delay = 0;
bool globals::fill_box = false;;
bool globals::aimbot = true;
bool globals::offscreen_Check = true;
bool globals::draw_aimbot_fov = false;
float globals::aimbot_fov_size = 200.f;
bool globals::distancecheck = false;
float globals::max_aimbot_distance = 200.f;



int globals::aimbot_type = 0;
int globals::aimbot_mode = 1;

int globals::aimbot_part = 1;
bool globals::closest_part = false;
int globals::aimbot_easing_style = 0;
bool globals::aimbot_sticky = false;

float globals::smoothness_camera = 0.0f;

bool globals::camera_prediction = true;
float globals::camera_prediction_x = 5.0f;
float globals::camera_prediction_y = 5.0f;
float globals::camera_prediction_z = 5.0f;

float globals::mouse_sensitivity = 1.5f;
float globals::mouse_smoothness = 2.0f;

bool globals::resolver = false;
std::vector<bool> globals::aimbot_checks = { false, false, false, false };

bool globals::shake = false;
float globals::shake_x = 0.65;
float globals::shake_y = 0.15;
bool globals::tool_esp = false;
float globals::shake_time_interval = 0.85f;
float globals::shake_amount = 6.0f;
float globals::shake_smoothness = 0.1f;
int globals::cframespeed = 0;
// FREE AIM

bool globals::free_aim = false;
bool globals::free_aim_sticky = false;
bool globals::localplayercheck = false;
bool globals::free_aim_resolver = false;
bool globals::free_aim_is_in_fov = false;
bool globals::free_aim_draw_fov = false;
std::vector<bool> globals::silent_aim_checks = { false, false, false, false };
float globals::camera_fov = 0.0f;
float globals::max_free_aim_distance = 200.f;
float globals::free_aim_fov = 200.f;
bool globals::cframe = false;;

int globals::free_aim_part = 1;
bool globals::free_aim_closest_part = false;

bool globals::free_aim_prediction = true;
bool globals::trigger_bot_prediction = false;
float globals::free_aim_prediction_x = 17.5f;
float globals::free_aim_prediction_y = 20.25f;
float globals::trigger_bot_prediction_x = 17.5f;
float globals::trigger_bot_prediction_y = 20.25f;
// TRIGGER BOT

bool globals::triggerbot = false;
bool globals::triggerbot_sticky = false;
bool globals::draw_triggerbot_fov = false;
bool globals::fly = false;
std::vector<bool> globals::triggerbot_checks = { false, false, false, false };

float globals::max_triggerbot_distance = 200.f;
bool globals::cashier_esp = false;
bool globals::soccer_ball_esp = false;
bool globals::jail_esp = false;
float globals::triggerbot_radius = 20.5;


// ESP

bool globals::esp = true;
bool globals::cash_esp = false;

float globals::max_render_distance = 5000.f;
int globals::fly_speed = 0;
bool globals::box_esp = false;
int globals::box_type = 0;

float globals::box_color[3] = { 0.562, 0.450, 0.700 };
float globals::color_3[3] = { 0.562, 0.450, 0.700 };
float globals::color_2[3] = { 0.562, 0.450, 0.700 };
float globals::color_1[3] = { 0.562, 0.450, 0.700 };
float globals::color_5[3] = { 0.562, 0.450, 0.700 };
float globals::color_4[3] = { 0.562, 0.450, 0.700 };
bool globals::health_bar = false;

bool globals::shield_bar = false;

bool globals::name_esp = false;
bool globals::name_outline = true;
int globals::name_type = 0;
float globals::name_color[3] = { 0.562, 0.450, 0.700 };

bool globals::distance_esp = false;
bool globals::distance_outline = true;
float globals::distance_color[3] = { 0.562, 0.450, 0.700 };

std::vector<bool> globals::esp_checks = { true, true, false, false };
CKeybind globals::aimbot_bind{ ("Aimbot_Bind") };
CKeybind globals::free_aim_bind{ ("free_aim_bindA") };
CKeybind globals::silent_Aim_Bind{ ("silent") };
// MOVEMENTS
bool globals::noclip = false;
bool globals::nojumpcooldown = false;
int globals::threadrestarttime = 700;
int globals::threadtime = 1200;
float globals::walkspeed_amount = 16.f;
float globals::JumpPower = 50.f;
bool globals::skeletonOutline = false;
int_fast64_t globals::game_id = 0;
bool globals::teambasedcolor = false;
float globals::skeletonThickness = 1;
bool globals::tracer_esp = false;
// OVERLAY SETTINGS
bool globals::flame_bar = false;
bool globals::force_projection_update = true;
bool globals::highcpuusageesp = true;
bool globals::vsync = false;
bool globals::streamproof = false;

int globals::font_type = 0;
int globals::menu_glow = 90;
// CHEAT SETTINGS
float globals::chams_opacity = 0.7f;
bool globals::fill_chams;
bool globals::chams_outline;
bool globals::debug_info = true;
bool globals::use_class_names = false;
ImVec4 globals::ImGuiColors[ImGuiCol_COUNT];