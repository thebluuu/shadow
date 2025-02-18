#pragma once
#include <cstdint>
#include <Windows.h>

namespace Offsets
{
    constexpr signed short int DataModel = 0x118; 
    constexpr signed short int Game = 0x1A8; 
    constexpr signed short int JobId = 0x110;
    constexpr signed short int Workspace = 0x150;
    constexpr signed short int GameID = 0x168;

    constexpr signed short int Visual_Engine = 0x10;
    constexpr signed short int Dimensions = 0x730; 
    constexpr signed short int View_Matrix = 0x4C0; 

    constexpr signed short int Size = 0x8;
    constexpr signed short int Children = 0x70;
    constexpr signed short int Parent = 0x50;
    constexpr signed short int  Lighting_FogEnd = 0x128; 
    constexpr  signed short int Lighting_FogStart = 0x12C; 
    constexpr signed short int Lighting_AmbientColor = 0xD0; 
    constexpr signed short int Lighting_OutdoorAmbient = 0x1A4; 
    constexpr signed short int Lighting_Brightness = 0x118; 
    constexpr signed short int Local_Entity = 0x118; 
    constexpr signed short int Model_Instance = 0x2A0;
    constexpr signed short int Humanoid_RigType = 0x1C0; 
    constexpr signed short int CanCollide = 0x313;
    constexpr signed short int Primary_Part = 0x230;
    constexpr signed short int Move_Direction = 0x150;
    constexpr signed short int CFrame = 0x11C;
    constexpr signed short int WalkSpeed1 = 0x1C8;
    constexpr signed short int WalkSpeed2 = 0x3A0;
    constexpr signed short int JumpPower1 = 0x1A8;
    constexpr signed short int HipHeight = 0x08C;
    constexpr signed short int HipBoxes = 0x184;

    constexpr signed short int Name = 0x68;
    constexpr signed short int Class_Name = 0x18;
    constexpr signed short int Team = 0x1D8;
    constexpr signed short int Value = 0xC8;

    constexpr signed short int MousePosition = 0xC8;

    constexpr signed short int Camera = 0x3E8; 
    constexpr signed short int Camera_Position = 0xFC;
    constexpr signed short int Camera_Rotation = 0xD8;
    constexpr signed short int Camera_FieldOfView = 0x140;

    constexpr signed short int Part_Primitive = 0x160;
    constexpr signed short int Part_Position = 0x140;
    constexpr signed short int Part_Velocity = 0x14C;
    constexpr signed short int Part_Rotation = 0x11C;
    constexpr signed short int Part_Size = 0x2B0;

    constexpr signed short int Health = 0x18C;
    constexpr signed short int Max_Health = 0x1AC;

    namespace World {
        constexpr signed short int Ambience = 0xC8; 
        constexpr signed short int Brightness = 0x114;
    }
}