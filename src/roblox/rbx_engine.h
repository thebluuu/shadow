#pragma once
#include <string>
#include <vector>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <queue>
#include "types/Cframe/Cframe.h"
#include "types/Matrix3x3/Matrix3x3.h"
#include "types/Matrix4x4/Matrix4x4.h"
#include "types/Vector2/Vector2.h"
#include "types/Vector3/Vector3.h"
#include "types/Vector4/Vector4.h"
#define M_PI 3.14159265358979323846

#ifndef PARTSTATES_H
#define PARTSTATES_H

struct PartState {
    std::string name;
    bool ignore;
};

extern std::vector<PartState> partStates;

#endif
class SmoothPing {
public:
    SmoothPing() {
        // Initial random target for ping
        targetPing = rand() % 100 + 30; // Random value between 30 and 130 ms
        currentPing = targetPing;
    }

    // Update ping gradually towards the target value
    int getUpdatedPing() {
        // Smoothly transition towards the targetPing value
        if (currentPing < targetPing) {
            currentPing += 1; // Increase the current ping towards the target
        }
        else if (currentPing > targetPing) {
            currentPing -= 1; // Decrease the current ping towards the target
        }

        // Occasionally pick a new random target ping value
        if (rand() % 100 < 5) { // 5% chance of changing the target
            targetPing = rand() % 100 + 30;
        }
     //   std::this_thread::sleep_for(std::chrono::seconds(2));
        return currentPing;
    }

private:
    int targetPing;  // Target ping value (randomly chosen)
    int currentPing; // The current ping value, updated gradually
};
namespace engine
{
    class roblox
    {
    public:
     
    //  static  void hook_exploits();
    };

    extern roblox rbx;
}
#include <windows.h>

namespace RBX
{
   
   
   
    
   




    struct MousePosition {
        float x, y;
    };

    class CircularBuffer {
    public:
        CircularBuffer(size_t size) : buffer(size), head(0), tail(0), full(false) {}

        bool push(const MousePosition& pos) {
            buffer[head] = pos;
            head = (head + 1) % buffer.size();
            if (full) { tail = (tail + 1) % buffer.size(); }
            full = head == tail;
            return true;
        }

        bool pop(MousePosition& pos) {
            if (isEmpty()) return false;
            pos = buffer[tail];
            tail = (tail + 1) % buffer.size();
            full = false;
            return true;
        }

        bool isEmpty() const { return (!full && (head == tail)); }

    private:
        std::vector<MousePosition> buffer;
        size_t head, tail;
        bool full;
    };

    extern CircularBuffer buffer;
    bool Initializer();
    bool load_game_data();

    bool load_Threads();

    bool checkdriver();

    void getRobloxId();


    class Instance final
    {
    public:
     
        // General Information
        std::uint64_t address = 0;
        std::string GetName() const;
        std::string GetClass() const;
        std::uint64_t GetRenderView();
        std::uint64_t GetCameraMatrix();
   //     RBX::Instance SetCurrentCamera(static_cast<int16_t>());
        std::pair<int, int> GetViewportSize();
        bool IsRenderViewValid();
        void SetRenderFeature(std::string featureName, bool enable);
        RBX::Vector2 GetDimensions();
        RBX::Matrix4x4 GetViewMatrix();
        // Children / Parent Operations
        std::vector<RBX::Instance> GetChildren(bool isHumanoid) const;
        RBX::Instance GetParent();
        RBX::Instance FindFirstChild(std::string child, bool isCharChild, std::vector<RBX::Instance> loadedChildren) const;
        bool IsA(const std::string& className) const;
        bool IsDescendantOf(const RBX::Instance& ancestor) const;
        std::vector<RBX::Instance> GetDescendants() const;
        RBX::Instance FindFirstDescendant(const std::string& name) const;
        RBX::Instance GetService(std::string child);

        // Destruction and Property Handling
        bool Destroy();
        bool SetProperty(const std::string& propertyName, std::uint64_t value);
        std::uint64_t GetProperty(const std::string& propertyName) const;
        bool Clone(RBX::Instance& newInstance) const;
        bool MoveTo(RBX::Instance& parent);
        std::vector<RBX::Instance> FindAllChildrenByName(const std::string& childName, bool isCharChild) const;
        std::vector<RBX::Instance> FindAllDescendantsByName(const std::string& name) const;

        // PlayerInstance Handling
        int_fast64_t GetGameId();
        RBX::Instance GetDataModelPTR();
        RBX::Instance GetDataModel();
        int_fast64_t GetCurrentPlaceId();
        std::vector<RBX::Instance> GetPlayerList();
        bool IsInGame();
        int GetPlayerCount();
        std::string GetJobId();
        RBX::Instance GetLocalPlayer() const;
        RBX::Instance GetModelInstance() const;
        RBX::Instance GetModelPrimaryPart();
        std::int32_t GetRigType();
        RBX::Instance GetTeam() const;
        void SetPartPos(RBX::Vector3 argument);
        void writePositionMultipleTimes(std::uint64_t primitive, RBX::Vector3 position);
        void SetPartVelocity(RBX::Vector3 velocity);
        std::uint64_t setFramePosX(uint64_t position);
        std::uint64_t setFramePosY(uint64_t position);
        float GetHealth() const;
        std::string getPlayerDisplayName();
        void SetHumanoidWalkSpeed(float walkspeed);
        void SetHumanoidJumpPower(float JumpPower);
        float GetMaxHealth() const;
        int getHumanoidState() const;
        bool isHumanoidInIdleState() const;
        bool isHumanoidInRunningState() const;
        bool isHumanoidInSwimmingState() const;
        RBX::CFrame GetCframe();

        // Camera Operations
        RBX::Instance GetCameraInstance();
        RBX::Vector3 GetCameraPosition();
        RBX::Instance GetLighting() const;
        void setFogEnd(float value);
        void setFogStart(float value);
        void setAmbientColor(RBX::Vector3 color);
        void setOutdoorAmbient(RBX::Vector3 color);
        void setBrightness(float value);
        void setGlobalShadows(bool enabled);
        void setBackDoorAmbienceColor(RBX::Vector3 color);
        void setColorShiftBottom(RBX::Vector3 color);
        void setColorShiftTop(RBX::Vector3 color);
        void setFogColor(RBX::Vector3 color);
        int GetColor3();
        void setColor3(RBX::Vector3 color);
        RBX::Instance Spectate(RBX::Instance stringhere);
        RBX::Instance UnSpectate();
        void SetCameraMaxZoom(int zoom);
        void SetCameraMinZoom(int zoom);
        RBX::Matrix3x3 GetCameraRotation();
        float GetFov();
        void SetCameraRotation(RBX::Matrix3x3 Rotation);
        static void initialize_mouse_service(std::uint64_t address);

        void write_mouse_position(std::uint64_t address, float x, float y);

        void SetColor(const std::string& part_name, RBX::Vector3 color);

        Instance SetAllColor(RBX::Vector3 color);

        void SetMaterial(const std::string& part_name);

        Instance SetAllMaterial();

        // Part Handling
        std::uint64_t GetPart() const;
        int GetPartMaterial() const;
        bool setCanCollide(bool value);
        RBX::Vector3 GetPosition() const;
        RBX::Vector3 GetVelocity() const;
        RBX::Matrix3x3 GetRotation() const;
        RBX::Vector3 GetSize() const;

        Vector3 SetSize(Vector3 size) const;

        // Value Handling
        void SetBoolFromValue(bool value) const;
        bool getBoolFromValue() const;
        void SetIntValue(int value);
        int getIntFromValue() const;
        RBX::Vector3 getVec3FromValue() const;
        void SetVec3FromValue(RBX::Vector3 Value);
        void SetFloatValue(float value) const;
        float GetFloatFromValue() const;
        void SetStringValue(const std::string& value);
        std::string getStringFromValue() const;

        // Miscellaneous
        int getPlayerArmor();
        RBX::Instance GetPlayersService();
        int getPlayerTeamColor();
        bool isPlayerVisible(Matrix4x4 viewMatrix);
        RBX::Instance GetVisualEngine();
        static void cache_mouse_service();
        int fetchPlayer(std::uint64_t address) const;
        static void cacheParts();
        static void updatePlayers();
        RBX::Instance GetWorkspace();
        RBX::Instance GetWorkspaceByOffset();
        RBX::Instance GetCurrentCamera();
        uint64_t GetScriptContext();
        uint64_t GetDataModelViaScriptContext();

        // Validation
        bool isValid() const noexcept {
            return address != 0;
        }

        bool operator==(const RBX::Instance& other) const {
            return this->address == other.address;
        }

    private:
        static std::uint64_t get_current_input_object(std::uint64_t base_address);
        static std::uint64_t cached_input_object;
        static std::uint64_t consistent_input_object_pointer;
       

};
class TaskScheduler final {
public:


    std::uint64_t address = 0;
    unsigned int GetTargetFPS();

    void SetTargetFPS(unsigned int targetFps);

    void PauseTask(uintptr_t jobAddress);

    uintptr_t GetScheduler();

    std::vector<RBX::Instance> GetJobs();

    std::string GetJobName(RBX::Instance instance);

    void ResumeTask(uintptr_t jobAddress);

    bool RemoveTaskByName(const std::string& targetName);

    unsigned int GetJobCount();

    void GetJobDetails(uintptr_t jobAddress);

    void QueueTask(uintptr_t taskAddress);

    void ClearScheduler();

    bool IsTaskRunning(uintptr_t jobAddress);

    void UpdateJobPriority(uintptr_t jobAddress, unsigned int newPriority);
 
   
 
    uint64_t GetJobByName(const std::string& targetName);

    uint64_t GetRenderJob();

private:
     // jobs structure
    // job.name
    //job.address
   


};
class Part final {
public:
    uintptr_t address;
private:
};

class Workspace final {
public:
    uintptr_t address;
private:

};
class Humanoid final {
public:
    uintptr_t address;
private:
};

class Player final {
public:
    uintptr_t address;
private:
};

class Lighting final {
public:

    uintptr_t address;
private:
};

class Camera final {
public:
    uintptr_t address;
  //  std::string getType() const override { return "Camera"; };
   
private:
  
};
class Graphics final {
public:
    uintptr_t address;
    class RenderView final {
    public:
        uintptr_t address;
    private:
    };

    class VisualEngine final  {
    public:
        uintptr_t address;
    private:

    };
private:
};

class DataModel final {
public:
    uintptr_t address;

private:
};


    class PlayerInstance final
    {
    public:
        bool operator==(const PlayerInstance& other) const {
            return address == other.address;
        }

        RBX::Vector3 pos;

        std::uint64_t address;
        std::vector<RBX::Instance> children;

        // Informations
        std::string name;
        RBX::Instance team;
        RBX::Instance character;
        Vector3 Position;
        Matrix3x3 Rotation;
        Vector3 Size;
        int r15;
        int shield;
        // Da Hood Exclusive
        RBX::Instance bodyEffects;
        RBX::Instance knockedOut;
        float health;
        float maxhealth;
        RBX::Instance ifGrabbed;
        RBX::Instance reloadcheckaimbot;
        RBX::Instance mousePosition;
        RBX::Instance mousePos;
        RBX::Instance currentTool;
        RBX::Instance aim;
        RBX::Instance  flame_obj;
        RBX::Instance tool;
        std::string currentToolName;
        RBX::Instance hc_aim;
        RBX::Instance armor_obj;
        bool hasGunEquipped = false;

        // Parts
        RBX::Instance humanoid;
        RBX::Instance head;
        RBX::Instance  rootJoint;
        RBX::Instance  neck;
        RBX::Instance rootPart;
        RBX::Instance pf_localplr;
        RBX::Instance upperTorso;
        RBX::Instance lowerTorso;
        RBX::Instance leftUpperLeg;
        RBX::Instance leftFoot;
        RBX::Instance rightUpperLeg;
        RBX::Instance rightFoot;
        RBX::Instance leftUpperArm;
        RBX::Instance leftHand;
        RBX::Instance rightUpperArm;
        RBX::Instance leftLowerLeg;
        RBX::Instance leftLowerArm;
        RBX::Instance rightLowerArm;
        RBX::Instance rightLowerLeg;
        RBX::Instance rightHand;
        RBX::Instance leftLeg;        // For R6 characters
        RBX::Instance rightLeg;       // For R6 characters
        RBX::Instance leftArm;        // For R6 characters
        RBX::Instance rightArm;       // For R6 characters
        RBX::Instance Vehicles;
    };
    class WorkSpace final
    {
    public:
        bool operator==(const PlayerInstance& other) const {
            return address == other.address;
        }

        RBX::Instance lighting;
        RBX::Instance workspace;
        RBX::Instance datamodel;
        RBX::Instance players;

        std::uint64_t address;
        std::vector<RBX::Instance> children;

        // Informations
        std::string name;
   
    };

    RBX::Vector2 WorldToScreen(RBX::Vector3 world, RBX::Vector2 dimensions, RBX::Matrix4x4 viewmatrix);

    extern CircularBuffer buffer;
}
