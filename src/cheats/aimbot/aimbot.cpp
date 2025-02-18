#include <Windows.h>
#include <thread>
#include <random>
#include <vector>
#include <immintrin.h>
#include <cmath>
#include <future>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")


#include "../features.h"
#include "../../misc/globals/globals.hpp"


#define M_PI 3.14159265358979323846

static RBX::Vector3 Recalculate_Velocity(RBX::PlayerInstance player)
{
    RBX::Vector3 old_Position = player.rootPart.GetPosition();
    std::this_thread::sleep_for(std::chrono::milliseconds(115));
    return (player.rootPart.GetPosition() - old_Position) / 0.115;
}

RBX::Instance FindPartByName(RBX::Instance& character, const std::string& partName) {
    return character.FindFirstChild(partName, true, {});
}

static float sigmoid(float x) {
    return 1 / (1 + std::exp(-x));
}

static RBX::Vector3 Random_Vector3(const float x, const float y) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis_x(-x, x);
    std::uniform_real_distribution<float> dis_y(-y, y);
    return { dis_x(gen) , dis_y(gen) , dis_x(gen) };
}

static float Ease(float t) {
    switch (globals::aimbot_easing_style) {
    case 0: return t;
    case 1: return 1 - std::cos((t * M_PI) / 2);
    case 2: return t * t;
    case 3: return t * t * t;
    case 4: return t * t * t * t;
    case 5: return t * t * t * t * t;
    case 6: return t == 0 ? 0 : std::pow(2, 10 * (t - 1));
    case 7: return 1 - std::sqrt(1 - std::pow(t, 2));
    case 8: return t * t * (2.70158f * t - 1.70158f);
    case 9:
        if (t < 1 / 2.75f) return 7.5625f * t * t;
        else if (t < 2 / 2.75f) return 7.5625f * (t -= 1.5f / 2.75f) * t + 0.75f;
        else if (t < 2.5f / 2.75f) return 7.5625f * (t -= 2.25f / 2.75f) * t + 0.9375f;
        else return 7.5625f * (t -= 2.625f / 2.75f) * t + 0.984375f;
    case 10: return t == 0 ? 0 : t == 1 ? 1 : -std::pow(2, 10 * (t - 1)) * std::sin((t - 1.1f) * 5 * M_PI);
    default: return t;
    }
}

static float SmoothEase(float t, float strength = 1.0f) {
    t = Ease(t);
    return std::pow(t, strength);
}

static RBX::Matrix3x3 Slerp_Matrix3(const RBX::Matrix3x3& a, const RBX::Matrix3x3& b, float t) {
    t = SmoothEase(t, 1.2f);
    if (t == 1) return b;

    RBX::Matrix3x3 result{};
    for (int i = 0; i < 9; ++i) {
        result.data[i] = a.data[i] + (b.data[i] - a.data[i]) * t;
    }
    return result;
}

static RBX::Vector2 SmoothMouseDelta(const RBX::Vector2& current, const RBX::Vector2& target, float smoothFactor) {
    return current + (target - current) * smoothFactor;
}


static RBX::Matrix3x3 Lerp_Matrix3(const RBX::Matrix3x3& a, const RBX::Matrix3x3& b, float t) {
    t = Ease(t);
    if (t == 1) return b;

    RBX::Matrix3x3 result{};
    for (int i = 0; i < 9; ++i) {
        result.data[i] = a.data[i] + (b.data[i] - a.data[i]) * t;
    }
    return result;
}

static RBX::Vector3 Cross_Product(const RBX::Vector3& vec1, const RBX::Vector3& vec2) {
    return {
        vec1.y * vec2.z - vec1.z * vec2.y,
        vec1.z * vec2.x - vec1.x * vec2.z,
        vec1.x * vec2.y - vec1.y * vec2.x
    };
}

static RBX::Matrix3x3 Look_At_To_Matrix(const RBX::Vector3& cameraPosition, const RBX::Vector3& targetPosition) {
    RBX::Vector3 forward = (targetPosition - cameraPosition).normalize();
    RBX::Vector3 right = Cross_Product({ 0, 1, 0 }, forward).normalize();
    RBX::Vector3 up = Cross_Product(forward, right);

    RBX::Matrix3x3 lookAtMatrix{};
    lookAtMatrix.data[0] = -right.x;  lookAtMatrix.data[1] = up.x;  lookAtMatrix.data[2] = -forward.x;
    lookAtMatrix.data[3] = right.y;  lookAtMatrix.data[4] = up.y;  lookAtMatrix.data[5] = -forward.y;
    lookAtMatrix.data[6] = -right.z;  lookAtMatrix.data[7] = up.z;  lookAtMatrix.data[8] = -forward.z;

    return lookAtMatrix;
}

void InitializePlayerParts(RBX::PlayerInstance& player) {
    std::vector<std::string> partNames = { "Head", "HumanoidRootPart", "UpperTorso", "LowerTorso", "LeftUpperLeg", "LeftUpperArm", "LeftHand", "RightUpperArm", "RightHand" };

    std::vector<RBX::Instance> parts;
    for (const std::string& partName : partNames) {
        parts.push_back(FindPartByName(player.character, partName));
    }

    player.head = parts[0];
    player.rootPart = parts[1];
    player.upperTorso = parts[2];
    player.lowerTorso = parts[3];
    player.leftUpperLeg = parts[4];
    player.leftUpperArm = parts[5];
    player.leftHand = parts[6];
    player.rightUpperArm = parts[7];
    player.rightHand = parts[8];
}

RBX::Instance getClosestPart(RBX::PlayerInstance& player, const POINT& cursor_point) {
    std::vector<RBX::Instance> parts = {
        player.head, player.rootPart, player.upperTorso, player.lowerTorso,
        player.leftUpperLeg, player.leftFoot, player.rightFoot,
        player.leftUpperArm, player.leftHand, player.rightUpperArm, player.rightHand,
    };

    RBX::Vector2 dimensions = globals::visualengine.GetDimensions();
    RBX::Matrix4x4 view_matrix = globals::visualengine.GetViewMatrix();
    RBX::Vector2 cursor = { static_cast<float>(cursor_point.x), static_cast<float>(cursor_point.y) };
    float min_distance = FLT_MAX;
    RBX::Instance closest_part;

    for (size_t i = 0; i < parts.size(); ++i) {
        if (!parts[i].address) continue;

        RBX::Vector3 part_position = parts[i].GetPosition();
        RBX::Vector2 part_screen_position = RBX::WorldToScreen(part_position, dimensions, view_matrix);
        float distance = (part_screen_position - cursor).getMagnitude();

        if (distance < min_distance) {
            min_distance = distance;
            closest_part = parts[i];
        }
    }

    return closest_part;
}

static RBX::PlayerInstance getClosestPlayerFromCursor() {
    
    std::vector<RBX::PlayerInstance>& cached_players = globals::cached_players;
    RBX::PlayerInstance closestPlayer{};
    float shortestDistance = 9e9f;

    RBX::PlayerInstance localPlayer = globals::localplayer;
    RBX::Instance localPlayerTeam = localPlayer.team;

    POINT cursor_point;
    if (globals::aimbot_mode == 1) {
        GetCursorPos(&cursor_point);
        ScreenToClient(FindWindowA(0, ("Roblox")), &cursor_point);
    }
    RBX::Vector2 cursor = { static_cast<float>(cursor_point.x), static_cast<float>(cursor_point.y) };
    RBX::Vector2 screenDimensions = globals::visualengine.GetDimensions();
    RBX::Matrix4x4 viewMatrix = globals::visualengine.GetViewMatrix();

    for (RBX::PlayerInstance& player : cached_players) {
        if (player.address == localPlayer.address || !player.character.address || !player.humanoid.address)
            continue;


        if (player.address == 0)
            continue;

        bool knockedCheck = globals::aimbot_checks[0];
        bool deadCheck = globals::aimbot_checks[1];
        bool grabbedCheck = globals::aimbot_checks[2];
        bool teamCheck = globals::aimbot_checks[3];
        bool sticky_target = globals::aimbot_sticky;

        if (knockedCheck && player.knockedOut.getBoolFromValue())
            continue;

        if (deadCheck == true && player.humanoid.GetHealth() <= 0)
            continue;

        if (grabbedCheck && player.ifGrabbed.address != 0)
            continue;

        if (teamCheck && player.team.address == localPlayerTeam.address)
            continue;

        RBX::Instance part = player.rootPart;
        RBX::Vector3 partPosition = part.GetPosition();

        if (globals::aimbot_mode == 0) {
            float distance_to_player = (globals::camera.GetCameraPosition() - partPosition).magnitude();

            if (distance_to_player > globals::max_aimbot_distance)
                continue;

            if (shortestDistance > distance_to_player) {
                closestPlayer = player;
                shortestDistance = distance_to_player;
            }
        }
        else if (globals::aimbot_mode == 1) {
            RBX::Vector2 partPositionOnScreen = RBX::WorldToScreen(partPosition, screenDimensions, viewMatrix);

            float distance_from_cursor = (partPositionOnScreen - cursor).getMagnitude();
            if (shortestDistance > distance_from_cursor) {
                closestPlayer = player;
                shortestDistance = distance_from_cursor;
            }
        }
    }

    return closestPlayer;
}


static bool isWithinFOV(const RBX::Vector3& hit_position_3D) {
    POINT cursor_point;
    GetCursorPos(&cursor_point);
    ScreenToClient(FindWindowA(0, ("Roblox")), &cursor_point);

    auto cursor_pos_x = cursor_point.x;
    auto cursor_pos_y = cursor_point.y;

    RBX::Instance visualengine = globals::visualengine;
    RBX::Vector2 screen_dimensions = visualengine.GetDimensions();
    RBX::Vector2 hit_position_2D = RBX::WorldToScreen(hit_position_3D, screen_dimensions, visualengine.GetViewMatrix());

    float magnitude = (hit_position_2D - RBX::Vector2{ static_cast<float>(cursor_pos_x), static_cast<float>(cursor_pos_y) }).getMagnitude();
    return (magnitude <= globals::aimbot_fov_size);
}

static void run(RBX::PlayerInstance player, RBX::Vector3 resolvedVelocity)
{
    
    RBX::Vector3 hit_position_3D{};
    POINT cursor_point;

    RBX::Instance character = player.character;
    RBX::Instance hitbox{};
    static POINT game_mouse_pos;

    if (game_mouse_pos.x == 0 && game_mouse_pos.y == 0) {
        GetCursorPos(&game_mouse_pos);
        ScreenToClient(FindWindowA(0, ("Roblox")), &game_mouse_pos);
    }

    RBX::PlayerInstance localPlayer = globals::localplayer;

    bool knockedCheck = globals::aimbot_checks[0];
    bool deadCheck = globals::aimbot_checks[1];
    bool grabbedCheck = globals::aimbot_checks[2];
    bool teamCheck = globals::aimbot_checks[3];

    if (knockedCheck & player.knockedOut.getBoolFromValue())
        return;

    if (deadCheck == true && player.humanoid.GetHealth() <= 0)
        return;

    if (grabbedCheck && player.ifGrabbed.address != 0)
        return;

    if (teamCheck == true && player.team.address == localPlayer.team.address)
        return;

    if (globals::closest_part) {
        hitbox = getClosestPart(player, cursor_point);
    }
    else {
        switch (globals::aimbot_part) {
        case 0: hitbox = player.head; break;
        case 1: hitbox = player.rootPart; break;
        case 2: hitbox = player.upperTorso; break;
        case 3: hitbox = player.lowerTorso; break;
        case 4: hitbox = player.leftHand; break;
        case 5: hitbox = player.rightHand; break;
        case 6: hitbox = player.leftUpperArm; break;
        case 7: hitbox = player.rightUpperArm; break;
        case 8: hitbox = player.leftUpperLeg; break;
        case 9: hitbox = player.rightUpperLeg; break;
        case 10: hitbox = player.leftFoot; break;
        case 11: hitbox = player.rightFoot; break;
        }
    }

    hit_position_3D = hitbox.GetPosition();



    if (globals::aimbot_type == 0) { // camlock
        POINT cursor_point;
        GetCursorPos(&cursor_point);
        ScreenToClient(FindWindowA(0, ("Roblox")), &cursor_point);

        auto cursor_pos_x = cursor_point.x;
        auto cursor_pos_y = cursor_point.y;

        RBX::Instance visualengine = globals::visualengine;
        RBX::Vector2 dimensions = visualengine.GetDimensions();
        RBX::Matrix4x4 view_matrix = visualengine.GetViewMatrix();

        if (globals::camera_prediction) {
            RBX::Vector3 velocity_vec = (globals::resolver ? resolvedVelocity : hitbox.GetVelocity());
            velocity_vec = velocity_vec / RBX::Vector3{ globals::camera_prediction_x, globals::camera_prediction_y, globals::camera_prediction_x };
            hit_position_3D = hitbox.GetPosition() + velocity_vec + (globals::shake ? Random_Vector3(globals::shake_x, globals::shake_y) : RBX::Vector3{});
        }
        else {
            hit_position_3D = hitbox.GetPosition() + (globals::shake ? Random_Vector3(globals::shake_x, globals::shake_y) : RBX::Vector3{});
        }

        float smoothness_camera = (100.1f - globals::smoothness_camera) / 100.0f;
        smoothness_camera = std::pow(smoothness_camera, 1.2f);

        RBX::Instance camera = globals::workspace.GetService("Camera");
        RBX::Matrix3x3 hit_matrix = Look_At_To_Matrix(camera.GetCameraPosition(), hit_position_3D);
        RBX::Matrix3x3 relative_matrix = Slerp_Matrix3(camera.GetCameraRotation(), hit_matrix, smoothness_camera);

        RBX::Vector2 hit_position_2D = RBX::WorldToScreen(hit_position_3D, dimensions, view_matrix);

        if (hit_position_2D.x == -1 || hit_position_2D.y == -1)
            return;

        float magnitude = (hit_position_2D - RBX::Vector2{ static_cast<float>(cursor_pos_x), static_cast<float>(cursor_pos_y) }).getMagnitude();

        RBX::Vector2 target_position = hit_position_2D;
        RBX::Vector2 current_position = { static_cast<float>(cursor_pos_x), static_cast<float>(cursor_pos_y) };

        RBX::Vector2 move_delta = SmoothMouseDelta(current_position, target_position, smoothness_camera);

        camera.SetCameraRotation(relative_matrix);
    }

    else if (globals::aimbot_type == 1) { // mouse lock
        POINT cursor_point;
        GetCursorPos(&cursor_point);
        ScreenToClient(FindWindowA(0, ("Roblox")), &cursor_point);

        auto cursor_pos_x = cursor_point.x;
        auto cursor_pos_y = cursor_point.y;

        RBX::Instance visualengine = globals::visualengine;
        RBX::Vector2 dimensions = visualengine.GetDimensions();
        RBX::Matrix4x4 view_matrix = visualengine.GetViewMatrix();

        if (globals::camera_prediction) {
            RBX::Vector3 velocity_vec;
            velocity_vec = (globals::resolver ? resolvedVelocity : hitbox.GetVelocity()) / RBX::Vector3{ globals::free_aim_prediction_x, globals::free_aim_prediction_y, globals::free_aim_prediction_x };
            hit_position_3D = (hitbox.GetPosition() + velocity_vec) + (globals::shake == true ? Random_Vector3(globals::shake_x, globals::shake_y) : RBX::Vector3{});
        }
        else {
            hit_position_3D = hitbox.GetPosition() + (globals::shake == true ? Random_Vector3(globals::shake_x, globals::shake_y) : RBX::Vector3{});
        }

        RBX::Vector2 hit_position_2D = RBX::WorldToScreen(hit_position_3D, dimensions, view_matrix);

        if (hit_position_2D.x == -1)
            return;

        float magnitude = (hit_position_2D - RBX::Vector2{ static_cast<float>(cursor_pos_x), static_cast<float>(cursor_pos_y) }).getMagnitude();

        RBX::Vector2 relative_2D = { 0, 0 };

        float sensitivity = globals::mouse_sensitivity;
        float smoothness_mouse = globals::mouse_smoothness;

        relative_2D.x = (hit_position_2D.x - cursor_pos_x) * sensitivity / smoothness_mouse;
        relative_2D.y = (hit_position_2D.y - cursor_pos_y) * sensitivity / smoothness_mouse;

        if (relative_2D.x == -1 || relative_2D.y == -1)
            return;

        INPUT input{};
        input.mi.time = 0;
        input.type = INPUT_MOUSE;
        input.mi.mouseData = 0;
        input.mi.dx = relative_2D.x;
        input.mi.dy = relative_2D.y;
        input.mi.dwFlags = MOUSEEVENTF_MOVE;
        SendInput(1, &input, sizeof(input));
    }

    return;
}

RBX::PlayerInstance aimbot_target;


void RBX::InitializeAimbot() {
    ::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    RBX::PlayerInstance saved_player{};
    bool is_aimboting = false;
    RBX::Vector3 velocity{};
    RBX::PlayerInstance current_player;
    RBX::PlayerInstance localplayer = globals::localplayer;

    HWND rblx = FindWindowA(0, ("Roblox"));
    while (true) {
        ::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
      //  
        //  CFrameFly();
        POINT cursor_point;
        GetCursorPos(&cursor_point);

        if (GetForegroundWindow() != rblx) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        globals::aimbot_bind.update();

        if (!globals::aimbot_bind.enabled || !globals::aimbot) {
            is_aimboting = false;
            aimbot_target.address = 0;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        current_player = (globals::aimbot_sticky && is_aimboting && saved_player.address != 0) ? saved_player : getClosestPlayerFromCursor();
        if (current_player.address == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            is_aimboting = false;
            aimbot_target.address = 0;
            continue;
        }
     //   std::this_thread::sleep_for(std::chrono::milliseconds(50));
        aimbot_target = current_player;
        if (globals::closest_part) InitializePlayerParts(current_player);

        run(current_player, velocity);
        saved_player = current_player;
        is_aimboting = true;

        if (globals::resolver) {
            std::thread([&velocity, &current_player]() { velocity = Recalculate_Velocity(current_player); }).detach();
        }
      /*  if (aimbot_target.address != 0) {
            aimbot_target.head.Spectate(aimbot_target.head);
        }
        else {
            aimbot_target.head.UnSpectate();
        }*/ // camera desync
     //   std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}