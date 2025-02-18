#include "../features.h"
#include "../../misc/globals/globals.hpp"
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include "../../roblox/rbx_engine.h"

#define M_PI 3.14159265358979323846
RBX::Instance  character{};
RBX::Instance  hrp{};
RBX::Instance camera{};

static RBX::Vector3 looknigga(const RBX::Matrix3x3& rotationMatrix) {
    return rotationMatrix.getColumn(2);
}

static RBX::Vector3 looknigga1(const RBX::Matrix3x3& rotationMatrix) {
    return rotationMatrix.getColumn(0);
}

void CFrameFly() {
    bool aircheck = false;
    while (true) {
        globals::fly_bind.update();
        if (!globals::fly || !globals::fly_bind.enabled) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            continue;
        }

        auto character = globals::players.GetLocalPlayer().GetModelInstance();
        auto hrp = character.FindFirstChild("HumanoidRootPart", false, {});
        if (!hrp.isValid()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            continue;
        }

        RBX::Instance camera = globals::camera;
        RBX::Matrix3x3 rotation_matrix = camera.GetCameraRotation();
        RBX::Vector3 look_vector = looknigga(rotation_matrix);
        RBX::Vector3 right_vector = looknigga1(rotation_matrix);
        RBX::Vector3 direction = { 0.0f, 0.0f, 0.0f };

        if (GetAsyncKeyState('W') & 0x8000) {
            direction = direction - look_vector;
            aircheck = true;
        }
        if (GetAsyncKeyState('S') & 0x8000) {
            direction = direction + look_vector;
            aircheck = true;
        }
        if (GetAsyncKeyState('A') & 0x8000) {
            direction = direction - right_vector;
            aircheck = true;
        }
        if (GetAsyncKeyState('D') & 0x8000) {
            direction = direction + right_vector;
            aircheck = true;
        }

        if (direction.magnitude() > 0) {
            direction = direction.normalize();
        }

        if (!aircheck) {
            hrp.SetPartVelocity({ 0.0f, 0.0f, 0.0f });
        }
        else {
            hrp.SetPartVelocity(direction * globals::fly_speed * 10);
        }

        aircheck = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
}



void RBX::hook_exploits()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        RBX::Instance character = globals::players.GetLocalPlayer().GetModelInstance();
        RBX::Instance humanoid = character.FindFirstChild("Humanoid", true, {});
        globals::silent_Aim_Bind.update();
        if (globals::walkspeed_amount != 16) {
            if (globals::silent_Aim_Bind.enabled) {
                humanoid.SetHumanoidWalkSpeed(globals::walkspeed_amount);
            }
        }
        CFrameFly();
        
		
	}
}


