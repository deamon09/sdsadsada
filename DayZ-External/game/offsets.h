#pragma once
#include <Windows.h>
#include <cstdint>

// Otimizado: Todos os offsets são constexpr para avaliação em tempo de compilação

class WorldOffsets {
public:
    static constexpr uint64_t OFF_World = 0xF4B050;
    static constexpr uint64_t OFF_NEAR_ENTITY_TABLE = 0xF48;
    static constexpr uint64_t OFF_FAR_ENTITY_TABLE = 0x1090;
    static constexpr uint64_t OFF_SLOW_ENTITY_TABLE = 0x2010;

    static constexpr uint64_t OFF_NearTableSize = 0xF50;     
    static constexpr uint64_t OFF_FarTableSize = 0x1098;     
    static constexpr uint64_t OFF_SlowTableSize = 0x2018;    

    static constexpr uint64_t OFF_ItemTable = 0x2060;
    static constexpr uint64_t OFF_ItemTableSize = 0x2068;
};

class NetworkOffsets {
public:
    static constexpr uint64_t off_networkmanager = 0xF5E190;
    static constexpr uint64_t off_networkmanager_networkclient = 0x50;

    static constexpr uint64_t off_entity_networkid = 0x6E4;   
    static constexpr uint64_t off_networkclient_scoreboard = 0x18;
    static constexpr uint64_t off_networkclient_playeridentity_size = 0x1c;

    static constexpr uint64_t off_playeridentity_networkid = 0x30;
    static constexpr uint64_t off_playeridentity_playername = 0xF8;

    static constexpr uint64_t off_tickness = 0xF19418;        
};

class LocalPlayerOffsets {
public:
    static constexpr uint64_t OFF_LocalPlayer1 = 0;
    static constexpr uint64_t OFF_LocalPlayer2 = 0;
    static constexpr uint64_t OFF_LocalPlayer3 = 0;
    static constexpr uint64_t OFF_LocalPlayer4 = 0;
};

class CameraOffsets {
public:
    static constexpr uint64_t OFF_Camera = 0x1B8;
    static constexpr uint64_t OFF_CameraOn = 0x2960;  

    static constexpr uint64_t off_cameraentry_movupdown = 0xa4;
    static constexpr uint64_t off_cameraentry_movforwardbackward = 0xc4;
    static constexpr uint64_t off_cameraentry_movleftright = 0xc8;
    static constexpr uint64_t off_cameraentry_movrotate = 0xd8;
};

class PlayerOffsets {
public:
    static constexpr uint64_t OFF_playerIsDead = 0xE2;
    static constexpr uint64_t OFF_playerNetworkID = 0x6E4;     // atualizado
    static constexpr uint64_t OFF_PlayerSkeleton = 0x7E8;      // atualizado
    static constexpr uint64_t OFF_ZmSkeleton = 0x678;          // atualizado

    static constexpr uint64_t OFF_Inhands = 0x1B0;
    static constexpr uint64_t OFF_PlayerName = 0xf8;
    static constexpr uint64_t OFF_PlayerCount = 0x24;

    static constexpr uint64_t OFF_PlayerHealthStruct = 0x188;
    static constexpr uint64_t OFF_PlayerHealthValue = 0x10;
};

class InventoryOffsets {
public:
    static constexpr uint64_t OFF_Inventory = 0x658;       // atualizado
    static constexpr uint64_t OFF_InventoryP = 0x150;      // mantido
    static constexpr uint64_t OFF_InventoryPsize = 0x15C;  // mantido

    static constexpr uint64_t OFF_InventoryI = 0x148;
    static constexpr uint64_t OFF_InventoryI2 = 0x38;
    static constexpr uint64_t OFF_InventoryIsize = 0x44;

    static constexpr uint64_t OFF_ItemList = 0xA0;
    static constexpr uint64_t OFF_ItemCount = 0xA8;

    static constexpr uint64_t OFF_HandItem = 0x7A0;     // mantido
};

class EntityOffsets {
public:
    static constexpr uint64_t OFF_EntityTypeName = 0xA8;
    static constexpr uint64_t OFF_EntityTypePtr = 0x180;
    static constexpr uint64_t OFF_ModelName = 0x88;
    static constexpr uint64_t OFF_RealName = 0x4f0;
    static constexpr uint64_t OFF_VisualState = 0x1C8;
    static constexpr uint64_t OFF_GetCoordinate = 0x2C;
};

class MiscOffsets {
public:
    static constexpr uint64_t OFF_MatrixClass = 0xBF0;
    static constexpr uint64_t OFF_Matrixb = 0x54;

    static constexpr uint64_t OFF_AnimClass = 0xB0;  

    static constexpr uint64_t OFF_EyeAccom = 0x2974;  

    static constexpr uint64_t OFF_grass = 0xC00;
    static constexpr uint64_t OFF_Bullets = 0xE00;
    static constexpr uint64_t Ammo = 0x6BC;
    static constexpr uint64_t OFF_Bulletsize = 0xE08;
};

class ItemOffsets {
public:
    static constexpr uint64_t off_itemquality = 0x194;
};

class TextOffsets {
public:
    static constexpr uintptr_t OFF_TEXT = 0x10;
    static constexpr uintptr_t OFF_ChamberAmmo = 0x68;
    static constexpr uintptr_t OFF_LENGTH = 0x8;
};

class WeaponOffsets {
public:
    static constexpr uint64_t OFF_WeaponIndex     = 0x6B0;

    static constexpr uint64_t OFF_WeaponInfoTable = 0x6B8;
    static constexpr uint64_t OFF_WeaponInfoSize  = 0x6BC;
    static constexpr uint64_t OFF_MuzzleCount     = 0x6BC;

    static constexpr uint64_t OFF_MagazineRef     = 0x150;
};

class MagazineOffsets {
public:
    static constexpr uint64_t OFF_AmmoCount   = 0x6B4;
    static constexpr uint64_t OFF_MagType     = 0x180;

    static constexpr uint64_t OFF_BulletList  = 0xE00;
    static constexpr uint64_t OFF_BulletList2 = 0x5A8;
};

class AmmoOffsets {
public:
    static constexpr uint64_t OFF_InitSpeed   = 0x364;
    static constexpr uint64_t OFF_AirFriction = 0x3B4;
};

class DayTimeOffsets {
public:
    static constexpr uint64_t OFF_DayTimeVar = 0xE70AB0; 
};