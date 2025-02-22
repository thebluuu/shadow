#pragma once

//<enums.hpp>
namespace utils
{
    //
    namespace enums
    {
        //
        enum class material
        {
            //
            Plastic = 256,
            SmoothPlastic = 272,
            Neon = 288,
            Wood = 512,
            WoodPlanks = 528,
            Marble = 784,
            Basalt = 788,
            Slate = 800,
            CrackedLava = 804,
            Concrete = 816,
            Limestone = 820,
            Granite = 832,
            Pavement = 836,
            Brick = 848,
            Pebble = 864,
            Cobblestone = 880,
            Rock = 896,
            Sandstone = 912,
            CorrodedMetal = 1040,
            DiamondPlate = 1056,
            Foil = 1072,
            Metal = 1088,
            Grass = 1280,
            LeafyGrass = 1284,
            Sand = 1296,
            Fabric = 1312,
            Snow = 1328,
            Mud = 1344,
            Ground = 1360,
            Asphalt = 1376,
            Salt = 1392,
            Ice = 1536,
            Glacier = 1552,
            Glass = 1568,
            ForceField = 1584,
            Air = 1792,
            Water = 2048,
            Cardboard = 2304,
            Carpet = 2305,
            CeramicTiles = 2306,
            ClayRoofTiles = 2307,
            RoofShingles = 2308,
            Leather = 2309,
            Plaster = 2310,
            Rubber = 2311
            //
        };
        //
        enum class camera_type : int
        {
            //
            Fixed = 0,
            Attach = 1,
            Watch = 2,
            Track = 3,
            Follow = 4,
            Custom = 5,
            Scriptable = 6,
            Orbital = 7
            //
        };
        //
    }
    //
}
//<Quiet>