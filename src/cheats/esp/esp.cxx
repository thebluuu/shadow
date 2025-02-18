#include "../features.h"
#include "render.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#define M_PI 3.14159265358979323846

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

 
inline static ImFont* verdana_12;

RBX::Vector3 Rotate(const RBX::Vector3& vec, const RBX::Matrix3x3& rotation_matrix) {

    const auto& row1 = rotation_matrix.data[0];
    const auto& row2 = rotation_matrix.data[3];
    const auto& row3 = rotation_matrix.data[6];

    float Sigma = vec.x * row1 + vec.y * rotation_matrix.data[1] + vec.z * rotation_matrix.data[2];
    float Sigma1 = vec.x * row2 + vec.y * rotation_matrix.data[4] + vec.z * rotation_matrix.data[5];
    float Sigma2 = vec.x * row3 + vec.y * rotation_matrix.data[7] + vec.z * rotation_matrix.data[8];

    return RBX::Vector3{ Sigma, Sigma1, Sigma2 };

}


ImVec2 RotatePointAroundCenter(const ImVec2& point, const ImVec2& center, float angle) {
    float s = std::sin(angle);
    float c = std::cos(angle);

    ImVec2 p = { point.x - center.x, point.y - center.y };

    float xnew = p.x * c - p.y * s;
    float ynew = p.x * s + p.y * c;

    p.x = xnew + center.x;
    p.y = ynew + center.y;

    return p;
}

void DrawFOV(ImVec2 center, float radius, ImU32 color) {
    ImGui::GetBackgroundDrawList()->AddCircleFilled(center, radius, color, 64);
}
void DrawFOVA(ImVec2 topLeft, ImVec2 size, ImU32 color) {
    ImGui::GetBackgroundDrawList()->AddRectFilled(topLeft, topLeft + size, color);
}


void DrawSkeletonESP(const RBX::PlayerInstance& player, const RBX::Matrix4x4& viewMatrix, const RBX::Vector2& dimensions, ImDrawList* draw) {

    std::vector<std::pair<RBX::Instance, RBX::Instance>> boneConnections;

    if (player.r15) {
        boneConnections = {
            {player.upperTorso, player.lowerTorso},
            {player.upperTorso, player.rightUpperArm},
            {player.rightUpperArm, player.rightLowerArm},
            {player.rightLowerArm, player.rightHand},
            {player.upperTorso, player.leftUpperArm},
            {player.leftUpperArm, player.leftLowerArm},
            {player.leftLowerArm, player.leftHand},
            {player.lowerTorso, player.rightUpperLeg},
            {player.rightUpperLeg, player.rightLowerLeg},
            {player.rightLowerLeg, player.rightFoot},
            {player.lowerTorso, player.leftUpperLeg},
            {player.leftUpperLeg, player.leftLowerLeg},
            {player.leftLowerLeg, player.leftFoot}
        };
    }
    else {
        boneConnections = {
            {player.upperTorso, player.lowerTorso},
            {player.upperTorso, player.rightUpperArm},
            {player.rightUpperArm, player.rightLowerArm},
            {player.rightLowerArm, player.rightHand},
            {player.upperTorso, player.leftUpperArm},
            {player.leftUpperArm, player.leftLowerArm},
            {player.leftLowerArm, player.leftHand},
            {player.lowerTorso, player.rightUpperLeg},
            {player.rightUpperLeg, player.rightLowerLeg},
            {player.rightLowerLeg, player.rightFoot},
            {player.lowerTorso, player.leftUpperLeg},
            {player.leftUpperLeg, player.leftLowerLeg},
            {player.leftLowerLeg, player.leftFoot}
        };
    }

    const RBX::Vector3 upperTorsoOffset(0, 0.5f, 0);
    const RBX::Vector3 upperArmOffset(0, 0.4f, 0);

    for (const auto& connection : boneConnections) {
        const auto& start = connection.first;
        const auto& end = connection.second;

        if (!start.address || !end.address)
            continue;

        RBX::Vector3 startPos = start.GetPosition();
        RBX::Vector3 endPos = end.GetPosition();

        if (start == player.upperTorso) {
            startPos += upperTorsoOffset;
        }
        if (end == player.upperTorso) {
            endPos += upperTorsoOffset;
        }
        if (start == player.leftUpperArm) {
            startPos += upperArmOffset;
        }
        if (end == player.leftUpperArm) {
            endPos += upperArmOffset;
        }
        if (start == player.rightUpperArm) {
            startPos += upperArmOffset;
        }
        if (end == player.rightUpperArm) {
            endPos += upperArmOffset;
        }

        RBX::Vector2 startScreen = RBX::WorldToScreen(startPos, dimensions, viewMatrix);
        RBX::Vector2 endScreen = RBX::WorldToScreen(endPos, dimensions, viewMatrix);

        if (startScreen.x == -1 || startScreen.y == -1 || endScreen.x == -1 || endScreen.y == -1)
            continue;

        ImVec2 startPoint = ImVec2(floorf(startScreen.x) + 0.5f, floorf(startScreen.y) + 0.5f);
        ImVec2 endPoint = ImVec2(floorf(endScreen.x) + 0.5f, floorf(endScreen.y) + 0.5f);

        if (globals::skeletonOutline) {
            draw->AddLine(startPoint, endPoint, IM_COL32(0, 0, 0, 255), globals::skeletonThickness + 1.0f);
        }

        draw->AddLine(startPoint, endPoint, IM_COL32(255, 255, 255, 255), globals::skeletonThickness);
    }
}



/// //////////////////////////////////////////////////////
// stuff im hding from nigga Koda im scared of him nigga fuck 
int current_armor = 0;
float shield_padding = 0.0f;
int current_flamearmor = 0;
float flame_padding = 0.0f;
/// ////////////////////////////////
void DrawCrosshair(ImVec2 center, float size, float gap, float angle, ImU32 color) {
    static ImVec2 pos = center;
    pos += (center - pos) * 0.08f;

    auto draw = ImGui::GetBackgroundDrawList();

    float halfGap = gap / 2.0f;

    ImVec2 l1_start = { pos.x - size, pos.y };
    ImVec2 l1_end = { pos.x - halfGap, pos.y };
    ImVec2 l2_start = { pos.x + halfGap, pos.y };
    ImVec2 l2_end = { pos.x + size, pos.y };

    ImVec2 l3_start = { pos.x, pos.y - size };
    ImVec2 l3_end = { pos.x, pos.y - halfGap };
    ImVec2 l4_start = { pos.x, pos.y + halfGap };
    ImVec2 l4_end = { pos.x, pos.y + size };

    auto rotate = [&](ImVec2 p1, ImVec2 p2) {
        return std::make_pair(
            RotatePointAroundCenter(p1, pos, angle),
            RotatePointAroundCenter(p2, pos, angle)
        );
        };

    auto [r1_start, r1_end] = rotate(l1_start, l1_end);
    auto [r2_start, r2_end] = rotate(l2_start, l2_end);
    auto [r3_start, r3_end] = rotate(l3_start, l3_end);
    auto [r4_start, r4_end] = rotate(l4_start, l4_end);

    ImU32 outlineColor = IM_COL32(0, 0, 0, 255);

    draw->AddLine(r1_start, r1_end, outlineColor, 4.0f);
    draw->AddLine(r2_start, r2_end, outlineColor, 4.0f);
    draw->AddLine(r3_start, r3_end, outlineColor, 4.0f);
    draw->AddLine(r4_start, r4_end, outlineColor, 4.0f);

    draw->AddLine(r1_start, r1_end, color, 2.0f);
    draw->AddLine(r2_start, r2_end, color, 2.0f);
    draw->AddLine(r3_start, r3_end, color, 2.0f);
    draw->AddLine(r4_start, r4_end, color, 2.0f);

    const char* text1 = "Shadow.";
    const char* text2 = "Hook";

    ImU32 textColor1 = ImGui::ColorConvertFloat4ToU32({ 0.400, 0.310, 0.510, globals::alpha * 255 });
    ImU32 textColor2 = IM_COL32(73, 70, 227, 255);  // Dark blue (RGB 73, 70, 227)


    ImVec2 text1Size = ImGui::CalcTextSize(text1);
    ImVec2 text2Size = ImGui::CalcTextSize(text2);

    float totalTextWidth = text1Size.x + text2Size.x;

    ImVec2 text1Pos = { pos.x - totalTextWidth / 2, pos.y + size + 5.0f };
    ImVec2 text2Pos = { text1Pos.x + text1Size.x, pos.y + size + 5.0f };

    ImVec2 offset = { 1.0f, 1.0f };

    auto drawText = [&](const ImVec2& pos, const char* text, ImU32 color) {
        draw->AddText(pos - offset, IM_COL32(0, 0, 0, 255), text);
        draw->AddText(pos + ImVec2(offset.x, -offset.y), IM_COL32(0, 0, 0, 255), text);
        draw->AddText(pos + ImVec2(-offset.x, offset.y), IM_COL32(0, 0, 0, 255), text);
        draw->AddText(pos + offset, IM_COL32(0, 0, 0, 255), text);
        draw->AddText(pos, color, text);
        };

    drawText(text1Pos, text1, textColor1);
    drawText(text2Pos, text2, textColor2);
}

float GetAnimatedGap(float baseGap, float amplitude, float frequency, float time) {
    return baseGap + amplitude * std::sin(frequency * time);
}
static std::vector< RBX::Vector3> GetCorners(const RBX::Vector3& partCF, const RBX::Vector3& partSize) {
    std::vector<RBX::Vector3> corners;

    for (int X = -1; X <= 1; X += 2) {
        for (int Y = -1; Y <= 1; Y += 2) {
            for (int Z = -1; Z <= 1; Z += 2) {
                RBX::Vector3 cornerPosition = {
                    partCF.x + partSize.x * X,
                    partCF.y + partSize.y * Y,
                    partCF.z + partSize.z * Z
                };
                corners.push_back(cornerPosition);
            }
        }
    }

    return corners;
}
inline float cross_product_2d(const ImVec2& O, const ImVec2& A, const ImVec2& B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

inline float distance_sq(const ImVec2& A, const ImVec2& B) {
    return (A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y);
}

std::vector<ImVec2> convexHull(std::vector<ImVec2>& points) {
    if (points.size() <= 3) return points;

    auto it = std::min_element(points.begin(), points.end(), [](const ImVec2& a, const ImVec2& b) {
        return (a.y < b.y) || (a.y == b.y && a.x < b.x);
        });

    std::swap(points[0], *it);
    ImVec2 p0 = points[0];

    std::sort(points.begin() + 1, points.end(), [&p0](const ImVec2& a, const ImVec2& b) {
        float cross = cross_product_2d(p0, a, b);
        return (cross > 0) || (cross == 0 && distance_sq(p0, a) < distance_sq(p0, b));
        });

    std::vector<ImVec2> hull;
    hull.push_back(points[0]);
    hull.push_back(points[1]);

    for (size_t i = 2; i < points.size(); i++) {
        while (hull.size() > 1 && cross_product_2d(hull[hull.size() - 2], hull.back(), points[i]) <= 0) {
            hull.pop_back();
        }
        hull.push_back(points[i]);
    }

    return hull;
}

void RBX::hook_esp()
{
    ::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    ImDrawList* draw = ImGui::GetBackgroundDrawList();

    if (!draw)
        return;

    POINT cursor_pos;
    GetCursorPos(&cursor_pos);
    ScreenToClient(FindWindowA(0, ("Roblox")), &cursor_pos);
    if (globals::crosshair) {
        ImVec2 center = ImVec2(cursor_pos.x, cursor_pos.y);
        float size = (float)globals::crosshair_size;
        float angle = ImGui::GetTime() * globals::crosshair_speed;
        float animated_gap = GetAnimatedGap((float)globals::crosshair_gap, 30, 2, ImGui::GetTime());
        ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(globals::box_color[0], globals::box_color[1], globals::box_color[2], 0.3f));

        DrawCrosshair(center, size, animated_gap, angle, color);
    }

    RBX::Instance visualengine = globals::visualengine;
    RBX::Vector2 dimensions = visualengine.GetDimensions();
    float screen_width = dimensions.x;
    float screen_height = dimensions.y;
    RBX::PlayerInstance localplayer = globals::localplayer;
    RBX::Instance localplayerHead = localplayer.head;


    static float rotation_offset = 0.0f;
    static float time_elapsed = 0.0f;

    if (globals::draw_aimbot_fov) {

        POINT cursor_screen_pos;
        GetCursorPos(&cursor_screen_pos);


        HWND hwnd = GetActiveWindow();
        ScreenToClient(hwnd, &cursor_screen_pos);


        ImVec2 size(400, 150);


        ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(globals::box_color[0], globals::box_color[1], globals::box_color[2], 0.3f));



        ImVec2 topLeft(cursor_pos.x - size.x / 2, cursor_pos.y - size.y / 2);


        DrawFOVA(topLeft, size, color);
    }



    if (globals::draw_triggerbot_fov) {
        draw->AddCircle(ImVec2(cursor_pos.x, cursor_pos.y), globals::triggerbot_radius, ImGui::ColorConvertFloat4ToU32(ImVec4(255, 70, 255, 255)), 64, 1.0f);
    }

    if (globals::free_aim_draw_fov) {
        draw->AddCircle(ImVec2(cursor_pos.x, cursor_pos.y), globals::free_aim_fov, ImGui::ColorConvertFloat4ToU32(ImVec4(255, 255, 70, 255)), 64, 1.0f);
    }
    if (globals::autoparry) {
        draw->Flags = ImDrawListFlags_AntiAliasedLines;
    }

    for (RBX::PlayerInstance& player : globals::cached_players)
    {
        ::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
        
        if (globals::autoparry) {
            draw->Flags = ImDrawListFlags_AntiAliasedLines;
        }

        RBX::Matrix4x4 viewMatrix = visualengine.GetViewMatrix();

        if (globals::localplayercheck && player.address == globals::localplayer.address)
            continue;
        if (globals::esp_checks[0] && player.knockedOut.getBoolFromValue())
            continue;
        if (globals::esp_checks[1] && player.humanoid.GetHealth() <= 0)
            continue;
        if (globals::esp_checks[2] && player.ifGrabbed.address != 0)
            continue;
        RBX::Instance localPlayerTeam = localplayer.team;
        RBX::Instance local_team = localPlayerTeam;
        RBX::Instance other_team = player.team;
        RBX::Instance character = player.character;
        RBX::Instance humanoidRootPart = player.rootPart;
        if (globals::esp_checks[3] && local_team.address == other_team.address)
            continue;
        if (globals::esp_checks[3] && local_team.GetName() == other_team.GetName())
            continue;
        RBX::Instance humanoid = player.humanoid;
        if (!character.address || !humanoid.address)
            continue;
        bool is_aimbot_target =
            (aimbot_target.address != 0 && player.address == aimbot_target.address
                
            || player.address ==  aimbot_target2.address 
                || globals::target.address != 0 && player.address == globals::target.address);
        if (globals::onlytarget) {
            if (!is_aimbot_target)
                continue;
        }
        float distanceMagnitude = (globals::camera.GetCameraPosition() - player.rootPart.GetPosition()).magnitude();
        if (globals::distancecheck) {
            if (distanceMagnitude > globals::max_render_distance) {
                continue;
            }
        }


        float alpha_factor = 1.0f - (distanceMagnitude / globals::max_render_distance);
        alpha_factor = std::clamp(alpha_factor, 0.0f, 1.0f);



        RBX::Vector2 headPosition2D = RBX::WorldToScreen(player.head.GetPosition(), dimensions, viewMatrix);

        if (headPosition2D.x == -1 || headPosition2D.y == -1)
            continue;

        if (globals::offscreen_Check) {

            if (headPosition2D.x < 0 || headPosition2D.x >= screen_width || headPosition2D.y < 0 || headPosition2D.y >= screen_height) {
                continue;
            }
        }
        if (globals::tracer_esp) {
            RBX::Vector2 playerHead2D = RBX::WorldToScreen(player.head.GetPosition(), dimensions, viewMatrix);
            ImVec2 playerHead2DImVec2(playerHead2D.x, playerHead2D.y);

            if (playerHead2D.x != -1 && playerHead2D.y != -1) {

                ImVec2 cursorPosImVec2(cursor_pos.x, cursor_pos.y);

                ImU32 tracerColor = ImGui::ColorConvertFloat4ToU32({ 0.400, 0.310, 0.510, globals::alpha * 255 });
                draw->AddLine(cursorPosImVec2, playerHead2DImVec2, tracerColor, 1.0f);
            }
        }
        if (!globals::esp)
            return;
        ImVec2 top_left, bottom_right;

        RBX::Instance parts[16] = { player.head, player.upperTorso, player.lowerTorso, player.leftFoot, player.rightFoot, player.leftUpperLeg, player.rightUpperLeg, player.rightLowerLeg, player.leftLowerLeg, player.leftHand, player.rightHand, player.leftUpperArm, player.rightUpperArm, player.leftLowerArm, player.rightLowerArm, player.rootPart };
        float minX = FLT_MAX, minY = FLT_MAX, maxX = -FLT_MAX, maxY = -FLT_MAX;
        for (int i = 0; i < 16; i++) {
            RBX::Instance p = parts[i];
            if (!p.address)
                continue;
            RBX::Vector3 s = p.GetSize();
            RBX::Vector3 pos = p.GetPosition();
            RBX::Matrix3x3 r = p.GetRotation();
            float hx = s.x * 0.5f, hy = s.y * 0.5f, hz = s.z * 0.5f;
            for (int j = 0; j < 8; j++) {
                RBX::Vector3 local;
                local.x = (j & 1) ? hx : -hx;
                local.y = (j & 2) ? hy : -hy;
                local.z = (j & 4) ? hz : -hz;
                RBX::Vector3 rotated;
                rotated.x = r.data[0] * local.x + r.data[1] * local.y + r.data[2] * local.z;
                rotated.y = r.data[3] * local.x + r.data[4] * local.y + r.data[5] * local.z;
                rotated.z = r.data[6] * local.x + r.data[7] * local.y + r.data[8] * local.z;
                RBX::Vector3 worldCorner = rotated + pos;
                RBX::Vector2 screen = RBX::WorldToScreen(worldCorner, dimensions, viewMatrix);
                if (screen.x != -1 && screen.y != -1) {
                    if (screen.x < minX)
                        minX = screen.x;
                    if (screen.y < minY)
                        minY = screen.y;
                    if (screen.x > maxX)
                        maxX = screen.x;
                    if (screen.y > maxY)
                        maxY = screen.y;
                }
            }
        }
        if (minX < maxX && minY < maxY) {
            float cx = (minX + maxX) * 0.5f;
            float cy = (minY + maxY) * 0.5f;
            float hw = (maxX - minX) * 0.5f;
            float hh = (maxY - minY) * 0.5f;
            top_left = { cx - hw, cy - hh };
            bottom_right = { cx + hw, cy + hh };

            if (globals::box_esp) {
                ImVec4 base_color;

                ImVec4 sigma = ImGui::ColorConvertU32ToFloat4(
                    ImGui::ColorConvertFloat4ToU32(
                        ImVec4(globals::box_color[0], globals::box_color[1], globals::box_color[2], globals::alpha)
                    )
                );

                if (is_aimbot_target) {

                    base_color = ImVec4(1.0f, 0.0f, 0.0f, globals::alpha);
                }
                else {

                    base_color = sigma;
                }

                ImU32 Base_color2 = ImGui::ColorConvertFloat4ToU32(base_color);
                ImVec2 pos = { top_left.x, top_left.y };
                ImVec2 size = { bottom_right.x - top_left.x, bottom_right.y - top_left.y };

                auto draw = ImGui::GetBackgroundDrawList();


                float glow_size = 34.0f;

                //  ImVec4 glow_color_vec = { base_color.x, base_color.y, base_color.z, base_color.w * 0.5f };
                ImU32 glowboy = ImGui::ColorConvertFloat4ToU32(ImVec4(globals::color_1[0], globals::color_1[1], globals::color_1[2], globals::glow_opacity));
                ImU32 glow_color = glowboy;

                ImVec2 glow_offset = { 0, 0 };


             //   draw->AddShadowRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), glow_color, globals::glow_size, glow_offset, ImDrawFlags_ShadowCutOutShapeBackground, 0.0f);


                switch (globals::box_type) {
                case 0:
                    Visualize.outlined_rect(pos, size, Base_color2, 0.0f);
                    break;
                case 1:
                    Visualize.cornered_rect(pos, size, Base_color2);
                    break;
                }
            }

        }


        if (globals::skeleton_esp) {
            DrawSkeletonESP(player, viewMatrix, dimensions, draw);

        }


        if (globals::shield_bar) {
            auto armor_obj = player.armor_obj;
            int current_armor = 0;
            if (armor_obj.address) {
                RBX::Instance cool;
                current_armor = player.shield;
            }
            ImVec2 shield_pos = { top_left.x, bottom_right.y + 1.f };
            ImVec2 shield_size = { bottom_right.x - top_left.x, 5.f };

            Visualize.shield_bar(200.f, current_armor, shield_pos, shield_size, alpha_factor);

            shield_padding = shield_size.y + 1.f;
        }


        if (globals::flame_bar) {
            auto flame_obj = player.flame_obj;
            if (flame_obj.address) {
                current_flamearmor = driver::read<int>(flame_obj.address + Offsets::Value);
            }
            ImVec2 flame_bar_pos = { top_left.x, bottom_right.y + shield_padding };
            ImVec2 flame_bar_size = { bottom_right.x - top_left.x, 6.f };
            Visualize.flame_bar(200.f, current_flamearmor, flame_bar_pos, flame_bar_size, alpha_factor);
            flame_padding = flame_bar_size.y + 1.f;
        }


        if (globals::vehicle_esp) {
            auto workspace = globals::workspace;
            RBX::Instance vehicleFile = workspace.FindFirstChild("Vehicles", false, {});
            if (vehicleFile.address) {
                auto vehicles = vehicleFile.GetChildren(false);
                for (const auto& vehicle : vehicles) {
                    RBX::Vector3 vehiclePos = vehicle.GetPosition();
                    RBX::Vector2 tempScreenPos = RBX::WorldToScreen(vehiclePos, dimensions, viewMatrix);
                    ImVec2 vehicleScreenPos(tempScreenPos.x, tempScreenPos.y);

                    if (vehicleScreenPos.x != -1 && vehicleScreenPos.y != -1) {
                        ImU32 vehicle_box_color = ImGui::ColorConvertFloat4ToU32({ 0.0f, 1.0f, 0.0f, globals::alpha });
                        float radius = 20.0f;
                        ImGui::GetBackgroundDrawList()->AddCircle(vehicleScreenPos, radius, vehicle_box_color, 16, 1.0f);
                    }
                }

            }
        }


        if (globals::jail_esp) {
            static std::vector<std::pair<RBX::Vector3, std::string>> cached_jail_locations;
            static float last_update = 0.0f;
            float current_time = ImGui::GetTime();

            if (current_time - last_update >= 2.0f) {
                cached_jail_locations.clear();
                auto workspace = globals::workspace;
                RBX::Instance ignoredFolder = workspace.FindFirstChild("Ignored", false, {});
                if (ignoredFolder.address) {
                    RBX::Instance jailLocation1 = ignoredFolder.FindFirstChild("JAILLOCATION1", false, {});
                    RBX::Instance jailLocation2 = ignoredFolder.FindFirstChild("JAILLOCATION2", false, {});

                    if (jailLocation1.address) {
                        cached_jail_locations.push_back({ jailLocation1.GetPosition(), "JAILLOCATION1" });
                    }
                    if (jailLocation2.address) {
                        cached_jail_locations.push_back({ jailLocation2.GetPosition(), "JAILLOCATION2" });
                    }
                }
                last_update = current_time;
            }

            if (cached_jail_locations.empty()) {
                Visualize.text_shadowed(ImVec2(50, 50), "No Jail Locations Found", ImColor(255, 0, 0), verdana_12);
            }
            else {
                for (int i = 0; i < cached_jail_locations.size(); ++i) {
                    auto& [pos, text] = cached_jail_locations[i];

                    RBX::Vector2 tempScreenPos = RBX::WorldToScreen(pos, dimensions, viewMatrix);
                    ImVec2 locationScreenPos(tempScreenPos.x, tempScreenPos.y);

                    if (locationScreenPos.x != -1 && locationScreenPos.y != -1) {

                        Visualize.text_shadowed(locationScreenPos, text.c_str(), ImColor(0, 255, 255), verdana_12);

                        ImGui::EndChild();
                    }
                }
            }
        }





        if (globals::soccer_ball_esp) {
            static std::vector<std::pair<RBX::Vector3, std::string>> cached_soccer_balls;
            static float last_update = 0.0f;
            float current_time = ImGui::GetTime();

            if (current_time - last_update >= 2.0f) {
                cached_soccer_balls.clear();
                auto workspace = globals::workspace;
                auto ignoredFolder = workspace.FindFirstChild("Ignored", false, {});

                if (ignoredFolder.address) {
                    auto soccerBall = ignoredFolder.FindFirstChild("SoccerBall", false, {});
                    if (soccerBall.address && soccerBall.IsA("MeshPart")) {
                        cached_soccer_balls.emplace_back(soccerBall.GetPosition(), "SoccerBall");
                    }
                }
                last_update = current_time;
            }

            if (cached_soccer_balls.empty()) {
                Visualize.text_shadowed(ImVec2(50, 50), "No Soccer Balls Found", ImColor(255, 0, 0), verdana_12);
            }
            else {
                const ImColor colors[] = {
                    ImColor(255, 0, 0),
                    ImColor(255, 165, 0),
                    ImColor(255, 255, 0),
                    ImColor(0, 0, 255)
                };

                for (size_t i = 0; i < cached_soccer_balls.size(); ++i) {
                    const auto& [pos, text] = cached_soccer_balls[i];
                    RBX::Vector2 tempScreenPos = RBX::WorldToScreen(pos, dimensions, viewMatrix);
                    ImVec2 ballScreenPos(tempScreenPos.x, tempScreenPos.y);

                    if (ballScreenPos.x != -1 && ballScreenPos.y != -1) {
                        Visualize.text_shadowed(ballScreenPos, text.c_str(), colors[i % 4], verdana_12);
                    }
                }
            }
        }




        if (globals::cashier_esp) {
            static std::vector<std::pair<RBX::Vector3, std::string>> cached_spawn_locations;
            static float last_update = 0.0f;
            float current_time = ImGui::GetTime();

            if (current_time - last_update >= 2.0f) {
                cached_spawn_locations.clear();
                auto workspace = globals::workspace;
                RBX::Instance ignoredFolder = workspace.FindFirstChild("Ignored", false, {});
                if (ignoredFolder.address) {
                    RBX::Instance spawnFolder = ignoredFolder.FindFirstChild("Spawn", false, {});
                    if (spawnFolder.address) {
                        auto spawnLocations = spawnFolder.GetChildren(false);
                        for (const auto& spawn : spawnLocations) {
                            if (spawn.GetName().find("SpawnLocation") != std::string::npos) {
                                cached_spawn_locations.push_back({ spawn.GetPosition(), "Spawn Location" });
                            }
                        }
                    }
                }
                last_update = current_time;
            }

            if (cached_spawn_locations.empty()) {
                Visualize.text_shadowed(ImVec2(50, 50), "No Spawn Locations Found", ImColor(255, 0, 0), verdana_12);
            }
            else {
                for (const auto& [pos, text] : cached_spawn_locations) {
                    RBX::Vector2 tempScreenPos = RBX::WorldToScreen(pos, dimensions, viewMatrix);
                    ImVec2 spawnScreenPos(tempScreenPos.x, tempScreenPos.y);
                    if (spawnScreenPos.x != -1 && spawnScreenPos.y != -1) {
                        Visualize.text_shadowed(spawnScreenPos, text.c_str(), ImColor(0, 255, 255), verdana_12);
                    }
                }
            }
        }


        if (globals::cash_esp) {
            static std::vector<std::pair<RBX::Vector3, std::string>> cached_cash;
            static float last_update = 0.0f;
            float current_time = ImGui::GetTime();

            if (current_time - last_update >= 2.0f) {
                cached_cash.clear();
                auto workspace = globals::workspace;
                RBX::Instance ignoredFolder = workspace.FindFirstChild("Ignored", false, {});
                if (ignoredFolder.address) {
                    RBX::Instance dropFolder = ignoredFolder.FindFirstChild("Drop", false, {});
                    if (dropFolder.address) {
                        auto moneyDrops = dropFolder.GetChildren(false);
                        for (const auto& cash : moneyDrops) {
                            if (cash.GetName().find("MoneyDrop") != std::string::npos) {
                                cached_cash.push_back({ cash.GetPosition(), "Cash" });
                            }
                        }
                    }
                }
                last_update = current_time;
            }

            if (cached_cash.empty()) {
                Visualize.text_shadowed(ImVec2(50, 50), "No Cash Found", ImColor(255, 0, 0), verdana_12);
            }
            else {
                for (const auto& [pos, text] : cached_cash) {
                    RBX::Vector2 tempScreenPos = RBX::WorldToScreen(pos, dimensions, viewMatrix);
                    ImVec2 cashScreenPos(tempScreenPos.x, tempScreenPos.y);
                    if (cashScreenPos.x != -1 && cashScreenPos.y != -1) {
                        Visualize.text_shadowed(cashScreenPos, text.c_str(), ImColor(0, 255, 0), verdana_12);
                    }
                }
            }
        }

        if (globals::dropped_items) {
            ImVec2 screenCenter = ImVec2(dimensions.x / 2.0f, dimensions.y / 2.0f);
            float arrowSize = 15.0f;
            float edgePadding = 10.0f;
            float arrowDistance = dimensions.y / 3.0f;

            for (const auto& player : globals::cached_players) {
                if (!player.rootPart.address) continue;
                RBX::Vector2 screenPos = RBX::WorldToScreen(player.rootPart.GetPosition(), dimensions, viewMatrix);

                if (screenPos.x == -1 || screenPos.y == -1) {
                    RBX::Vector3 dir = (player.rootPart.GetPosition() - globals::camera.GetCameraPosition()).normalize();
                    float angle = atan2(dir.z, dir.x);

                    ImVec2 projectedPos = ImVec2(
                        screenCenter.x + cos(angle) * arrowDistance,
                        screenCenter.y + sin(angle) * arrowDistance
                    );

                    projectedPos.x = std::clamp(projectedPos.x, edgePadding, dimensions.x - edgePadding);
                    projectedPos.y = std::clamp(projectedPos.y, edgePadding, dimensions.y - edgePadding);

                    ImVec2 arrowTip = projectedPos;
                    ImVec2 arrowLeft = ImVec2(
                        arrowTip.x + cos(angle + 2.5f) * arrowSize,
                        arrowTip.y + sin(angle + 2.5f) * arrowSize
                    );
                    ImVec2 arrowRight = ImVec2(
                        arrowTip.x + cos(angle - 2.5f) * arrowSize,
                        arrowTip.y + sin(angle - 2.5f) * arrowSize
                    );

                    ImU32 arrowColor = ImGui::ColorConvertFloat4ToU32(ImVec4(globals::box_color[0], globals::box_color[1], globals::box_color[2], globals::alpha));
                    draw->AddTriangleFilled(arrowTip, arrowLeft, arrowRight, arrowColor);
                }
            }
        }




        if (globals::health_bar) {
            float health = player.humanoid.GetHealth();
            float maxHealth = player.humanoid.GetHealth();

            int health_bar_width = 4;

            float box_height = maxY - minY;

            ImVec2 pos = { top_left.x + 1 - health_bar_width - 0.7f, top_left.y - 2.0f };
            ImVec2 size = { float(health_bar_width), box_height + 4.0f };

            float health_percentage = health / maxHealth;
            health_percentage = std::clamp(health_percentage, 0.0f, 1.0f);

            float health_height = size.y * health_percentage;
            ImVec2 filled_pos = { pos.x, pos.y + size.y - health_height };
            ImU32 box_color = ImGui::ColorConvertFloat4ToU32(ImVec4(globals::box_color[0], globals::box_color[1], globals::box_color[2], globals::alpha));


            Visualize.health_bar(maxHealth, health, pos, size, globals::alpha);
            ImU32 glowboy = ImGui::ColorConvertFloat4ToU32(ImVec4(globals::color_1[0], globals::color_1[1], globals::color_1[2], globals::glow_opacity));
            ImU32 glow_color = glowboy;

            ImVec2 glow_offset = { 0, 0 };


            //    draw->AddShadowRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), glow_color, globals::glow_size, glow_offset, ImDrawFlags_ShadowCutOutShapeBackground, 0.0f);

            if (globals::health_bar_text) {
                char health_text[8];
                snprintf(health_text, sizeof(health_text), "%.0f", health_percentage * 100.0f);

                float text_x = pos.x + size.x - 23.0f;
                float text_y = filled_pos.y - 12.0f;

                ImVec2 cool_pos = ImVec2(text_x, text_y);

                Visualize.text_shadowed(cool_pos, health_text, box_color, verdana_12);

            }
        }




        if (globals::tool_esp) {
            RBX::Instance equippedTool = player.currentTool;
            std::string toolName = "None";

            if (equippedTool.address) {
                toolName = player.currentToolName;
            }

            ImVec2 toolTextSize = ImGui::CalcTextSize(toolName.c_str());



            ImVec2 boxBottomCenter((top_left.x + bottom_right.x) / 2, bottom_right.y);

            ImVec2 distancePos = { boxBottomCenter.x - toolTextSize.x / 2.f, boxBottomCenter.y + 3.0f };
            ImU32 sigma = ImGui::ColorConvertFloat4ToU32(ImVec4(globals::color_2[0], globals::color_2[1], globals::color_2[2], globals::alpha));
            ImU32 toolColor = sigma;
            ImU32 glowboy = ImGui::ColorConvertFloat4ToU32(ImVec4(globals::color_1[0], globals::color_1[1], globals::color_1[2], globals::glow_opacity));
            ImU32 glow_color = glowboy;


            ImVec2 glow_offset = { 0, 0 };


            //        draw->AddShadowRect(distancePos, ImVec2(distancePos.x + toolTextSize.x, distancePos.y + toolTextSize.y), glow_color, globals::glow_size, glow_offset, ImDrawFlags_ShadowCutOutShapeBackground, 0.0f);

            if (globals::tool_outline) {
                Visualize.text_shadowed(distancePos, toolName.c_str(), toolColor, verdana_12);
            }
            else {
                Visualize.text(distancePos, toolName.c_str(), toolColor, verdana_12);
            }
        }

        if (globals::distance_esp) {
            float distanceMagnitude = (globals::camera.GetCameraPosition() - player.rootPart.GetPosition()).magnitude();
            distanceMagnitude = roundf(distanceMagnitude * 100) / 100;

            char distanceStr[32];
            sprintf(distanceStr, ("%.0fm"), distanceMagnitude);

            ImVec2 distanceTextSize = ImGui::CalcTextSize(distanceStr);



            float toolTextPadding = 13.5f;

            ImVec2 boxBottomCenter((top_left.x + bottom_right.x) / 2, bottom_right.y);
            ImVec2 distancePoAs = { boxBottomCenter.x - distanceTextSize.x / 2.f, boxBottomCenter.y + 1.0f };

            ImVec2 toolTextPos = { distancePoAs.x, distancePoAs.y + toolTextPadding };
            ImU32 sigma = ImGui::ColorConvertFloat4ToU32(ImVec4(globals::distance_color[0], globals::distance_color[1], globals::distance_color[2], globals::alpha));
            ImU32 distanceColor = sigma;
            ImU32 glowboy = ImGui::ColorConvertFloat4ToU32(ImVec4(globals::color_1[0], globals::color_1[1], globals::color_1[2], globals::glow_opacity));
            ImU32 glow_color = glowboy;

            ImVec2 glow_offset = { 0, 0 };


            //     draw->AddShadowRect(toolTextPos, ImVec2(toolTextPos.x + distanceTextSize.x, toolTextPos.y + distanceTextSize.y), glow_color, globals::glow_size, glow_offset, ImDrawFlags_ShadowCutOutShapeBackground, 0.0f);

            if (globals::distance_outline) {
                Visualize.text_shadowed(toolTextPos, distanceStr, distanceColor, verdana_12);
            }
            else {
                Visualize.text(toolTextPos, distanceStr, distanceColor, verdana_12);
            }
        }
        std::vector<RBX::Instance> partsA;

        if (!player.r15) {

            partsA = {

                  player.head,
                player.upperTorso,
                player.lowerTorso,
                player.rightUpperArm,
                player.rightLowerArm,
                player.rightHand,
                player.leftUpperArm,
                player.leftLowerArm,
                player.leftHand,
                player.rightUpperLeg,
                player.rightLowerLeg,
                player.rightFoot,
                player.leftUpperLeg,
                player.leftLowerLeg,
                player.leftFoot,
                player.rootPart
            };
        }
        else {

            partsA = {

                player.head,
                player.upperTorso,
                player.lowerTorso,
                player.rightUpperArm,
                player.rightLowerArm,
                player.rightHand,
                player.leftUpperArm,
                player.leftLowerArm,
                player.leftHand,
                player.rightUpperLeg,
                player.rightLowerLeg,
                player.rightFoot,
                player.leftUpperLeg,
                player.leftLowerLeg,
                player.leftFoot,

            };
        }
        if (globals::chams) {
            for (const auto& bone : partsA) {
                if (!bone.address)
                    continue;

                RBX::Vector3 bone_pos = bone.GetPosition();
                RBX::Vector3 bone_size = bone.GetSize();
                RBX::Matrix3x3 bone_rotation = bone.GetRotation();

                std::vector<RBX::Vector3> CubeVertices = GetCorners({ 0, 0, 0 }, { bone_size.x / 2, bone_size.y / 2, bone_size.z / 2 });

                for (auto& vertex : CubeVertices) {
                    vertex = Rotate(vertex, bone_rotation);
                    vertex = { vertex.x + bone_pos.x, vertex.y + bone_pos.y, vertex.z + bone_pos.z };
                }

                std::vector<ImVec2> screen_vertices;
                for (const auto& vertex : CubeVertices) {
                    RBX::Vector2 screen_pos = RBX::WorldToScreen(vertex, dimensions, viewMatrix);
                    if (screen_pos.x == -1 || screen_pos.y == -1) {
                        screen_vertices.clear();
                        break;
                    }
                    screen_vertices.push_back(ImVec2(screen_pos.x, screen_pos.y));
                }

                if (screen_vertices.size() < 3)
                    continue;

                
                ImU32 glowboy = ImGui::ColorConvertFloat4ToU32(ImVec4(globals::color_1[0], globals::color_1[1], globals::color_1[2], globals::glow_opacity));
                ImU32 glow_color = glowboy;

               
                float glow_size = 40.0f;  

                
                std::vector<ImVec2> glow_hull = convexHull(screen_vertices);
                ImVec2 glow_offset = { 0, 0 };

                draw->AddConvexPolyFilled(glow_hull.data(), glow_hull.size(), glow_color);

              
                if (globals::autoparry) {
                    draw->Flags = ImDrawListFlags_AntiAliasedFill;
                }

                std::vector<ImVec2> hull = convexHull(screen_vertices);
                ImU32 fillColor = ImGui::GetColorU32(ImVec4(138.0f / 255.0f, 157.0f / 255.0f, 198.0f / 255.0f, 0.5f));
                ImU32 box_color = ImGui::ColorConvertFloat4ToU32(ImVec4(globals::color_4[0], globals::color_4[1], globals::color_4[2], globals::chams_opacity));

                draw->AddConvexPolyFilled(hull.data(), hull.size(), box_color);
            }
        }

        if (globals::name_esp) {

            std::string player_name = player.name;

            auto top_text_sz = ImGui::CalcTextSize(player_name.c_str());

            ImVec2 box_top_center((top_left.x + bottom_right.x) / 2, top_left.y);

            ImVec2 text_position = { box_top_center.x - top_text_sz.x / 2.f, box_top_center.y - top_text_sz.y - 3.5f };
            ImU32 glowboy = ImGui::ColorConvertFloat4ToU32(ImVec4(globals::color_1[0], globals::color_1[1], globals::color_1[2], globals::glow_opacity));
            ImU32 glow_color = glowboy;

            ImVec2 glow_offset = { 0, 0 };


            //     draw->AddShadowRect(text_position, ImVec2(text_position.x + top_text_sz.x, text_position.y + top_text_sz.y), glow_color, globals::glow_size, glow_offset, ImDrawFlags_ShadowCutOutShapeBackground, 0.0f);
            ImU32 sigma = ImGui::ColorConvertFloat4ToU32(ImVec4(globals::name_color[0], globals::name_color[1], globals::name_color[2], globals::alpha));
            ImU32 name_color = sigma;
            if (globals::name_outline) {
                Visualize.text_shadowed(text_position, player_name.c_str(), name_color, verdana_12);
            }
            else {
                Visualize.text(text_position, player_name.c_str(), name_color, verdana_12);
            }

        }

    }
}