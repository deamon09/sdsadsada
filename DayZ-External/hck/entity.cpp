#define _CRT_SECURE_NO_WARNINGS
#include "../overlay/overlay.h"
#include "../game/SDK.h"
#include <thread>
#include "settings.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <cfloat>
#include <ctime>
#include <vector>
#include <string>
#include "configs.h"
#include "../game/xorstr.h"
#include <mutex>
#include <map>
#include "../game/xorstr.h"
#include "../game/settings.h"
#include "hotkeys.h"
#include <Windows.h> 
#include <unordered_map>
#include <DirectXMath.h>
#include <wininet.h>
#include <iphlpapi.h>
#include "iconfont.h"
#include "fa.h"
#include <random>
#include <sstream>

#if !defined(ImDrawCornerFlags_All) && defined(ImDrawFlags_RoundCornersAll)
#define ImDrawCornerFlags_All ImDrawFlags_RoundCornersAll
#define ImDrawCornerFlags_Top ImDrawFlags_RoundCornersTop
#endif

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "iphlpapi.lib")
#define UENGINE __stdcall

std::vector<player_t> entities = {};
std::vector<item_t> items = {};
bool streamproof = true;
bool qualidade;
bool showESPMenu = false; // ESP Menu toggle variable

// Variáveis globais para configs
std::vector<std::string> configFiles;
std::string newConfigName;
GameVars g_GameVars;
int isnpc = 0;

uintptr_t* resultant_target_entity = nullptr;

class USQAnimInstanceSoldier1P1 {
public:
    void UENGINE RecoilCameraOffsetFactor(int zoom) {
        *(int*)(this) = zoom;
    }
};

std::vector<uintptr_t*> valid_players;
std::vector<uintptr_t*> valid_players1;
std::vector<uintptr_t*> valid_players2;
std::vector<uintptr_t*> valid_Zombies;

namespace BasePlayer {
    float Sensitivity = 0.2;
    int FOV = 120;
    bool Enabled = true;
    bool ESP = false;
    bool FullBox = false;
    bool CornerBox = false;
    bool Skeleton = false;
    bool PName = false;
    bool PHealth = false;
    bool PWeapon = false;
    bool PDistance = false;
    bool Chams = true;
    bool NoSpread = false;
    bool NoRecoil = false;
    float NoRecoilValue = 0;
    float NoSpreadValue = 0;
    bool Aimbot = false;
    bool SuperFlick = false;
    bool FOVCircle = false;
    float FOVSize = false;
    bool Visible = false;
    bool TeamCheck = false;
    bool Smooth = false;
    float SmoothValue = 5.0f;
}

int fovShape = 0; // 0: Circle, 1: Square, 2: Hexagon
float fovShapeFloat = 0.0f; // Float to control shape (0.0–3.0 for 4 shapes)

bool ActiveZombieSilentAim = false;
float ZombieAimFov = 100.0f;
int ZombieLimiteDoSilent = 300;
int ZombieBoneChoose = 0;
ImVec4 ZombieFovColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

bool PerfectWeather = false;
bool AmmoSpeedChange = false;

namespace Variables {
    int ActiveTab = 1;
}

int DrawString(char* String, int x, int y, int r, int g, int b, ID3DXFont* ifont) {
    RECT FontPos;
    FontPos.left = x;
    FontPos.top = y;
    ifont->DrawTextA(0, String, -1, &FontPos, DT_NOCLIP, D3DCOLOR_ARGB(255, r, g, b));
    return 0;
}

void test_offset(const std::string& offset_name, uint64_t address) {
    try {
        uint64_t value = read<uint64_t>(address);
        std::cout << offset_name << " at 0x" << std::hex << address << " is working." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << offset_name << " at 0x" << std::hex << address << " failed: " << e.what() << std::endl;
    }
}

void waitForGame() {
    while (true) {
        std::cout << "Game started! Initializing cheat..." << std::endl;
        break;
        std::cout << "Waiting for game to start..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

std::string generateRandomTitle(int length) {
    const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::string title;

    for (int i = 0; i < length; ++i) {
        title += chars[rand() % chars.size()];
    }

    return title;
}

void setConsoleTitle() {
    std::string randomTitle = generateRandomTitle(10);
    SetConsoleTitleA(randomTitle.c_str());
}

void enableAnsi() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hConsole, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hConsole, dwMode);
    }
}

const char* green = "\033[1;32m";
const char* red = "\033[1;31m";
const char* yellow = "\033[1;33m";
const char* reset = "\033[0m";
const char* bold = "\033[1m";

void setUtf8Console() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::ios::sync_with_stdio(false);
}

void init() {
    if (!Driver::Connect()) {
        system("color 4");
        std::cerr << red << "\n  Driver not loaded.\n" << reset;
        Sleep(2000);
        return;
    }

    system("color 2");
    std::cout << green << "\n  Driver loaded successfully!\n" << reset;
    Sleep(1000);
    setConsoleTitle();
    system("cls");

    while (true) {
        setConsoleTitle();
        Driver::process_id = Driver::FindProcess(L"DayZ_x64.exe");
        if (Driver::process_id != 0) break;

        system("color B");
        std::cerr << yellow << "\n Searching for process... Please wait..." << reset;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    system("color 2");
    std::cout << green << "\n  Process found!\n" << reset;
    Sleep(1000);

    globals.Base = Driver::GetBaseAddress();
    if (!globals.Base) {
        system("color 4");
        std::cerr << red << "\n  Module base not found.\n" << reset;
        Sleep(2000);
        return;
    }

    system("color 7");
    globals.World = Read<uint64_t>(globals.Base + WorldOffsets::OFF_World);
    std::thread(update_list).detach();
}

void waitForProcess() {
    while (true) {
        setConsoleTitle();
        Driver::process_id = Driver::FindProcess(L"DayZ_x64.exe");
        if (Driver::process_id != 0) {
            break;
        }

        system("color B");
        std::cerr << "Searching for process... Please wait." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    system("color 2");
    std::cout << "Process found!" << std::endl;
    Sleep(8000);
}

std::string tm_to_readable_time(tm ctx) {
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%a %m/%d/%y %H:%M:%S %Z", &ctx);
    return std::string(buffer);
}

static std::time_t string_to_timet(std::string timestamp) {
    auto cv = strtol(timestamp.c_str(), NULL, 10);
    return (time_t)cv;
}

static std::tm timet_to_tm(time_t timestamp) {
    std::tm context;
    localtime_s(&context, &timestamp);
    return context;
}


// Otimizado: Função de atualização de lista de entidades com melhor performance
void update_list() {
    while (true) {
        std::vector<player_t> tmp{};
        
        // Validação inicial do World
        if (!globals.World) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }

        // Ler tamanhos das tabelas uma vez
        const uint32_t nearTableSize = read<uint32_t>(globals.World + WorldOffsets::OFF_NEAR_ENTITY_TABLE + 0x8);
        const uint32_t farTableSize = read<uint32_t>(globals.World + WorldOffsets::OFF_FAR_ENTITY_TABLE + 0x8);
        const uint32_t slowTableSize = read<uint32_t>(globals.World + 0x2018);
        
        // Validar tamanhos para evitar loops infinitos
        if (nearTableSize > 10000 || farTableSize > 10000 || slowTableSize > 10000) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }

        // Reservar memória antecipadamente
        tmp.reserve(nearTableSize + farTableSize + slowTableSize);

        // Processar Near Table
        const uint64_t nearEntityTable = read<uint64_t>(globals.World + WorldOffsets::OFF_NEAR_ENTITY_TABLE);
        if (nearEntityTable) {
            for (uint32_t i = 0; i < nearTableSize; i++) {
                const uint64_t entity = read<uint64_t>(nearEntityTable + (i * 0x8));
                if (!entity) continue;

                player_t player{};
                player.NetworkID = read<uintptr_t>(entity + NetworkOffsets::off_entity_networkid);
                player.TableEntry = nearEntityTable;
                player.EntityPtr = entity;
                tmp.push_back(player);
            }
        }

        // Processar Far Table
        const uint64_t farEntityTable = read<uint64_t>(globals.World + WorldOffsets::OFF_FAR_ENTITY_TABLE);
        if (farEntityTable) {
            for (uint32_t i = 0; i < farTableSize; i++) {
                const uint64_t entity = read<uint64_t>(farEntityTable + (i * 0x8));
                if (!entity) continue;

                player_t player{};
                player.NetworkID = read<uintptr_t>(entity + NetworkOffsets::off_entity_networkid);
                player.TableEntry = farEntityTable;
                player.EntityPtr = entity;
                tmp.push_back(player);

            }
        }

        // Aplicar modificações de FOV (uma vez por ciclo)
        if (Changefov) {
            const uint64_t cameraPtr = read<uint64_t>(globals.World + 0x28);
            if (cameraPtr) {
                write<float>(cameraPtr + 0x6C, Horizontal);
                write<float>(cameraPtr + 0x70, Vertical);
            }
        }

        // Aplicar modificações de clima (otimizado)
        if (PerfectWeather) {
            const uintptr_t landscape = read<uintptr_t>(globals.Base + 0x41B6238);
            if (ISVALID(landscape)) {
                const uintptr_t worldWeather = read<uintptr_t>(landscape + 0x7458);
                if (ISVALID(worldWeather)) {
                    constexpr uintptr_t WEATHER_VALUE_OFFSET = 0x10;
                    
                    const uintptr_t overcastPtr = read<uintptr_t>(worldWeather + 0x10);
                    if (overcastPtr) write<float>(overcastPtr + WEATHER_VALUE_OFFSET, overcast);

                    const uintptr_t fogPtr = read<uintptr_t>(worldWeather + 0x18);
                    if (fogPtr) write<float>(fogPtr + WEATHER_VALUE_OFFSET, fog);

                    const uintptr_t rainPtr = read<uintptr_t>(worldWeather + 0x20);
                    if (rainPtr) write<float>(rainPtr + WEATHER_VALUE_OFFSET, rain);

                    const uintptr_t snowfallPtr = read<uintptr_t>(worldWeather + 0x28);
                    if (snowfallPtr) write<float>(snowfallPtr + WEATHER_VALUE_OFFSET, snowfall);
                }
            }
        }

        // Aplicar modificações de grama
        if (removeGrass) {
            Game::SetTerrainGrid(0);
        } else {
            Game::SetTerrainGrid(12);
        }

        // Processar Slow Table
        const uint64_t slowEntityTable = read<uint64_t>(globals.World + 0x2010);
        if (slowEntityTable) {
            for (uint32_t i = 0; i < slowTableSize; i++) {
                const uint64_t entity = read<uint64_t>(slowEntityTable + (i * 0x8));
                if (!entity) continue;

                player_t player{};
                player.NetworkID = read<uintptr_t>(entity + NetworkOffsets::off_entity_networkid);
                player.TableEntry = slowEntityTable;
                player.EntityPtr = entity;
                tmp.push_back(player);
            }
        }

        // Atualizar lista global de entidades
        entities = std::move(tmp);
        
        // Sleep adaptativo baseado no número de entidades
        const int sleepTime = (entities.size() > 100) ? 150 : 100;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
    }
}

void DrawEntityInfo(uintptr_t Entity, const ImVec2& screenPosition, const std::string& text, ImVec4 color) {
    auto* window = ImGui::GetOverlayDrawList();
    if (window) {
        window->AddText(screenPosition, ImGui::GetColorU32(color), text.c_str());
    }
}

std::string get_quality(uint64_t Entity) {
    auto quality = read<int>(Entity + 0x194);

    if (quality == 1) return _("Worn");
    else if (quality == 2) return _("Damaged");
    else if (quality == 3) return _("Badly Damaged");
    else if (quality == 4) return _("Ruined");
    else return _("Pristine");
}

void DrawBoxFilledmod(const DirectX::XMFLOAT2& from, const DirectX::XMFLOAT2& size, DirectX::XMFLOAT4 color, float rounding) {
    auto* window = ImGui::GetOverlayDrawList();
    window->AddRectFilled(ImVec2(from.x, from.y), ImVec2(size.x, size.y), ImGui::GetColorU32(ImVec4(color.x / 255, color.y / 255, color.z / 255, color.w / 255)), rounding, ImDrawCornerFlags_All);
}

void DrawInventoryList(std::vector<std::string> item, std::string name_str, DirectX::XMFLOAT2 screen_resolution, DirectX::XMFLOAT4 tag_color) {
    auto y_ = screen_resolution.y / 4.f;
    auto x_ = screen_resolution.x / 6.f;
    auto x__ = screen_resolution.x - x_;
    auto x__x = x__ + 200.f;
    auto mid_point = (x__x - x__) / 2;
    auto list_size = item.size() * 25.f;
    ImVec2 textSize = ImGui::CalcTextSize(name_str.c_str(), 0, false, -1.0f);
    auto x_text_ = textSize.x / 2;

    float totat_y_text_size = textSize.y + textSize.y + textSize.y + textSize.y;
    for (const auto& name_size : item) {
        ImVec2 textSizeitem = ImGui::CalcTextSize(name_size.c_str(), 0, false, -1.0f);
        totat_y_text_size += textSizeitem.y + 10;
    }

    DrawBoxFilledmod({ x__, y_ }, { x__x, y_ + totat_y_text_size }, { 0.f, 0.f, 0.f, 120.f }, 2.8f);
    DrawBoxFilledmod({ x__, y_ }, { x__x, y_ + 5.f }, tag_color, 2.8f);

    auto* window = ImGui::GetOverlayDrawList();
    window->AddText({ x__x - mid_point - x_text_, y_ + textSize.y + textSize.y }, ImGui::GetColorU32({ 255.f / tag_color.x, 255.f / tag_color.y, 255.f / tag_color.z, 255.f / tag_color.w }), name_str.c_str());

    float decrement = textSize.y + textSize.y + textSize.y + textSize.y + textSize.y;

    for (const auto& name : item) {
        ImVec2 textSizeitem = ImGui::CalcTextSize(name.c_str(), 0, false, -1.0f);
        auto x_text_item = textSizeitem.x / 2;
        window->AddText({ x__x - mid_point - x_text_item, y_ + decrement }, ImGui::GetColorU32({ 255.f / tag_color.x, 255.f / tag_color.y, 255.f / tag_color.z, 255.f / tag_color.w }), name.c_str());
        decrement += textSizeitem.y;
    }
}

// Cache de nomes de jogadores para melhor performance
static std::unordered_map<uint32_t, std::pair<std::string, uint64_t>> playerNameCache;
static std::mutex playerNameCacheMutex;
constexpr uint64_t PLAYER_NAME_CACHE_TIMEOUT = 30000; // 30 segundos

// Otimizado: Obter nome do jogador com cache
std::string get_player_name(uint64_t Entity) {
    if (!Entity) return "Unknown";

    const uint32_t entityNetworkID = read<uint32_t>(Entity + NetworkOffsets::off_entity_networkid);
    if (entityNetworkID == 0) return "Unknown";

    // Verificar cache primeiro
    {
        std::lock_guard<std::mutex> lock(playerNameCacheMutex);
        auto it = playerNameCache.find(entityNetworkID);
        if (it != playerNameCache.end()) {
            const uint64_t currentTime = GetTickCount64();
            if ((currentTime - it->second.second) < PLAYER_NAME_CACHE_TIMEOUT) {
                return it->second.first;
            }
            // Cache expirado, remover
            playerNameCache.erase(it);
        }
    }

    // Buscar nome do jogador
    std::string playerName = "Unknown";
    const uint64_t networkClient = read<uint64_t>(globals.Base + NetworkOffsets::off_networkmanager + NetworkOffsets::off_networkmanager_networkclient);
    if (!networkClient) return playerName;

    const uint64_t scoreboard = read<uint64_t>(networkClient + NetworkOffsets::off_networkclient_scoreboard);
    const uint32_t scoreboardSize = read<uint32_t>(networkClient + NetworkOffsets::off_networkclient_playeridentity_size);

    if (!scoreboard || scoreboardSize == 0 || scoreboardSize > 1000) return playerName;

    for (uint32_t i = 0; i < scoreboardSize; ++i) {
        const uint64_t currentIdentity = read<uint64_t>(scoreboard + (i * sizeof(uint64_t)));
        if (!currentIdentity) continue;

        const uint32_t currentID = read<uint32_t>(currentIdentity + NetworkOffsets::off_playeridentity_networkid);
        if (currentID != entityNetworkID) continue;

        const uint64_t namePtr = read<uint64_t>(currentIdentity + NetworkOffsets::off_playeridentity_playername);
        if (!namePtr) break;

        playerName = ReadArmaString(namePtr);
        
        // Adicionar ao cache
        {
            std::lock_guard<std::mutex> lock(playerNameCacheMutex);
            playerNameCache[entityNetworkID] = { playerName, GetTickCount64() };
        }
        break;
    }

    return playerName;
}

std::vector<std::string> GetPlayerInventoryItems(uint64_t playerEntity)
{
    std::vector<std::string> inventoryItems;

    if (!playerEntity || Game::is_dead(playerEntity))
        return inventoryItems;

    try
    {
        uint64_t inventory = read<uint64_t>(playerEntity + 0x658);
        if (!Game::IsValidPtr((void*)inventory))
            return inventoryItems;


        // 2. Roupas equipadas (Clothing array) - camisa, calça, colete, mochila, capacete, etc.
        uint64_t clothingArray = read<uint64_t>(inventory + 0x150);
        uint32_t clothingCount = read<uint32_t>(inventory + 0x158); // tamanho geralmente +0x8 do ponteiro (ajuste se necessário)

        if (Game::IsValidPtr((void*)clothingArray) && clothingCount > 0 && clothingCount < 40)
        {
            for (uint32_t i = 0; i < clothingCount; i++)
            {
                uint64_t clothingItem = read<uint64_t>(clothingArray + (i * 0x8));
                if (!Game::IsValidPtr((void*)clothingItem)) continue;

                // Nome do item de roupa
                uint64_t typePtr = read<uint64_t>(clothingItem + EntityOffsets::OFF_EntityTypePtr);
                if (Game::IsValidPtr((void*)typePtr))
                {
                    uint64_t namePtr = read<uint64_t>(typePtr + EntityOffsets::OFF_RealName);
                    if (namePtr)
                    {
                        std::string name = ReadArmaString(namePtr);
                        if (!name.empty())
                        {
                            inventoryItems.push_back("[Clothing] " + name);

                            // 3. Cargo dentro dessa roupa (mochila, calça, camisa, colete, etc.)
                            uint64_t itemInv = read<uint64_t>(clothingItem + 0x658); // ItemInventory
                            if (Game::IsValidPtr((void*)itemInv))
                            {
                                uint64_t cargoGrid = read<uint64_t>(itemInv + 0x148);
                                if (Game::IsValidPtr((void*)cargoGrid))
                                {
                                    uint64_t itemList = read<uint64_t>(cargoGrid + 0x38);
                                    uint32_t itemCount = read<uint32_t>(cargoGrid + 0x40); // geralmente +0x8 do ponteiro

                                    if (Game::IsValidPtr((void*)itemList) && itemCount > 0 && itemCount < 1500)
                                    {
                                        for (uint32_t j = 0; j < itemCount; j++)
                                        {
                                            uint64_t subItem = read<uint64_t>(itemList + (j * 0x8));
                                            if (!Game::IsValidPtr((void*)subItem)) continue;

                                            uint64_t subType = read<uint64_t>(subItem + EntityOffsets::OFF_EntityTypePtr);
                                            if (Game::IsValidPtr((void*)subType))
                                            {
                                                uint64_t subNamePtr = read<uint64_t>(subType + EntityOffsets::OFF_RealName);
                                                if (subNamePtr)
                                                {
                                                    std::string subName = ReadArmaString(subNamePtr);
                                                    if (!subName.empty())
                                                        inventoryItems.push_back("  └─ [In " + name + "] " + subName);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }


        uint64_t beltPtr = read<uint64_t>(inventory + 0xF8); // teste valores 0xF8, 0x100, 0x120
        if (Game::IsValidPtr((void*)beltPtr))
        {
        }
    }
    catch (...)
    {
        // fail silent
    }

    return inventoryItems;
}
bool IsPlayerInInventoryFOV(uint64_t playerEntity, float fovSize) {
    if (!playerEntity || Game::is_dead(playerEntity)) {
        return false;
    }

    try {
        uint64_t skeleton = read<uint64_t>(playerEntity + PlayerOffsets::OFF_PlayerSkeleton);
        if (!Game::IsValidPtr((void*)skeleton)) {
            return false;
        }

        auto visualState = Game::GeVisualState(playerEntity);
        if (!visualState) {
            return false;
        }

        Vector3 headBonePos;
        if (!Game::GetBonePosition(skeleton, visualState, 24, &headBonePos)) {
            return false;
        }

        Vector3 screenPos;
        if (!Game::WorldToScreen(headBonePos, screenPos) || screenPos.z < 0.01f) {
            return false;
        }

        ImGuiIO& io = ImGui::GetIO();
        float centerX = io.DisplaySize.x * 0.5f;
        float centerY = io.DisplaySize.y * 0.5f;

        float dx = screenPos.x - centerX;
        float dy = screenPos.y - centerY;
        float distance = sqrtf(dx * dx + dy * dy);

        return distance <= fovSize;

    }
    catch (...) {
        return false;
    }
}

void DrawPlayerInventoryESP() {
    if (!enableInventoryESP) {
        return;
    }

    ImDrawList* drawList = ImGui::GetOverlayDrawList();
    ImGuiIO& io = ImGui::GetIO();

    uint64_t localPlayer = Game::GetLocalPlayer();
    if (!localPlayer) {
        return;
    }

    uint64_t targetPlayer = 0;
    float closestDistance = FLT_MAX;

    for (const auto& entity : entities) {
        if (entity.EntityPtr == localPlayer) {
            continue;
        }

        std::string entityType = Game::GetEntityTypeName(entity.EntityPtr);
        if (entityType != "dayzplayer") {
            continue;
        }

        if (Game::is_dead(entity.EntityPtr)) {
            continue;
        }

        if (!IsPlayerInInventoryFOV(entity.EntityPtr, inventoryESPFOV)) {
            continue;
        }

        Vector3 worldPos = Game::GetCoordinate(entity.EntityPtr);
        float distance = Game::GetDistanceToMe(worldPos);

        if (distance < closestDistance) {
            closestDistance = distance;
            targetPlayer = entity.EntityPtr;
        }
    }

    if (!targetPlayer) {
        return;
    }

    std::vector<std::string> inventoryItems = GetPlayerInventoryItems(targetPlayer);

    if (inventoryItems.empty()) {
        return;
    }

    std::string playerName = get_player_name(targetPlayer);
    if (playerName.empty() || playerName == "") {
        playerName = "Survivor";
    }

    float padding = 10.0f;
    float startX = io.DisplaySize.x - 300.0f - padding;
    float startY = padding;

    float lineHeight = 18.0f;
    float headerHeight = 25.0f;
    float totalHeight = headerHeight + (inventoryItems.size() * lineHeight) + (padding * 2);

    ImVec4 bgColor = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);
    ImVec4 headerColor = ImVec4(0.2f, 0.6f, 1.0f, 1.0f);
    ImVec4 textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    drawList->AddRectFilled(
        ImVec2(startX, startY),
        ImVec2(startX + 300.0f, startY + totalHeight),
        ImGui::ColorConvertFloat4ToU32(bgColor),
        5.0f
    );

    drawList->AddRectFilled(
        ImVec2(startX, startY),
        ImVec2(startX + 300.0f, startY + headerHeight),
        ImGui::ColorConvertFloat4ToU32(headerColor),
        5.0f, ImDrawCornerFlags_Top
    );

    std::string headerText = playerName + " - Inventory";
    ImVec2 headerTextSize = ImGui::CalcTextSize(headerText.c_str());
    drawList->AddText(
        ImVec2(startX + (300.0f - headerTextSize.x) * 0.5f, startY + (headerHeight - headerTextSize.y) * 0.5f),
        ImGui::ColorConvertFloat4ToU32(textColor),
        headerText.c_str()
    );

    float currentY = startY + headerHeight + padding;
    for (size_t i = 0; i < inventoryItems.size(); i++) {
        const std::string& item = inventoryItems[i];
        ImVec2 itemTextSize = ImGui::CalcTextSize(item.c_str());

        std::string displayItem = item;
        if (itemTextSize.x > 280.0f) {
            displayItem = item.substr(0, 30) + "...";
        }

        drawList->AddText(
            ImVec2(startX + padding, currentY),
            ImGui::ColorConvertFloat4ToU32(textColor),
            displayItem.c_str()
        );

        currentY += lineHeight;
    }

    drawList->AddRect(
        ImVec2(startX, startY),
        ImVec2(startX + 300.0f, startY + totalHeight),
        ImGui::ColorConvertFloat4ToU32(ImVec4(0.3f, 0.3f, 0.3f, 1.0f)),
        5.0f, 0, 1.5f
    );
}

std::vector<uintptr_t> corpses;
bool bulletsToHead = true;
uintptr_t centerTargetEnt;
std::mutex bulletToHeadMutex;

void DrawDCornerBox(uintptr_t Entity, ImVec4 color, float lineThickness = 2.0f, float boxSize = 12.0f) {
    std::vector<int> boneIndices = { 49, 8, 75, 113 };
    std::vector<ImVec2> screenPoints;

    for (int boneIndex : boneIndices) {
        Vector3 bonePos;
        if (!Game::GetBonePosition(Game::GetSkeleton(Entity, 1), Game::GeVisualState(Entity), boneIndex, &bonePos))
            continue;

        Vector3 screenPos3D;
        if (Game::WorldToScreen(bonePos, screenPos3D)) {
            screenPoints.emplace_back(screenPos3D.x, screenPos3D.y);
        }
    }

    if (screenPoints.size() < 2)
        return;

    ImVec2 topLeft = screenPoints[0];
    ImVec2 bottomRight = screenPoints[0];

    for (const auto& point : screenPoints) {
        if (point.x < topLeft.x) topLeft.x = point.x;
        if (point.y < topLeft.y) topLeft.y = point.y;
        if (point.x > bottomRight.x) bottomRight.x = point.x;
        if (point.y > bottomRight.y) bottomRight.y = point.y;
    }

    float cornerLength = boxSize;
    auto* window = ImGui::GetOverlayDrawList();

    window->AddLine(ImVec2(topLeft.x, topLeft.y), ImVec2(topLeft.x + cornerLength, topLeft.y), ImGui::GetColorU32(color), lineThickness);
    window->AddLine(ImVec2(topLeft.x, topLeft.y), ImVec2(topLeft.x, topLeft.y + cornerLength), ImGui::GetColorU32(color), lineThickness);

    window->AddLine(ImVec2(topLeft.x, bottomRight.y), ImVec2(topLeft.x + cornerLength, bottomRight.y), ImGui::GetColorU32(color), lineThickness);
    window->AddLine(ImVec2(topLeft.x, bottomRight.y), ImVec2(topLeft.x, bottomRight.y - cornerLength), ImGui::GetColorU32(color), lineThickness);

    window->AddLine(ImVec2(bottomRight.x, topLeft.y), ImVec2(bottomRight.x - cornerLength, topLeft.y), ImGui::GetColorU32(color), lineThickness);
    window->AddLine(ImVec2(bottomRight.x, topLeft.y), ImVec2(bottomRight.x, topLeft.y + cornerLength), ImGui::GetColorU32(color), lineThickness);

    window->AddLine(ImVec2(bottomRight.x, bottomRight.y), ImVec2(bottomRight.x - cornerLength, bottomRight.y), ImGui::GetColorU32(color), lineThickness);
    window->AddLine(ImVec2(bottomRight.x, bottomRight.y), ImVec2(bottomRight.x, bottomRight.y - cornerLength), ImGui::GetColorU32(color), lineThickness);
}

void DrawTextWithOutline(ImDrawList* drawList, ImVec2 pos, ImU32 color, const std::string& text) {
    ImU32 outlineColor = ImColor(0, 0, 0);

    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx != 0 || dy != 0) {
                drawList->AddText(ImVec2(pos.x + dx, pos.y + dy), outlineColor, text.c_str());
            }
        }
    }
    drawList->AddText(pos, color, text.c_str());
}

void DrawCornerBox(ImDrawList* drawList, ImVec2 min, ImVec2 max, ImU32 color, float thickness, float cornerFraction) {
    float width = max.x - min.x;
    float height = max.y - min.y;
    float cornerW = width * cornerFraction;
    float cornerH = height * cornerFraction;

    ImVec2 corners[4] = {
        min,
        ImVec2(max.x, min.y),
        max,
        ImVec2(min.x, max.y)
    };

    for (int i = 0; i < 4; ++i) {
        ImVec2 start = corners[i];
        ImVec2 endH, endV;

        if (i == 0) {
            endH = ImVec2(start.x + cornerW, start.y);
            endV = ImVec2(start.x, start.y + cornerH);
        }
        else if (i == 1) {
            endH = ImVec2(start.x - cornerW, start.y);
            endV = ImVec2(start.x, start.y + cornerH);
        }
        else if (i == 2) {
            endH = ImVec2(start.x - cornerW, start.y);
            endV = ImVec2(start.x, start.y - cornerH);
        }
        else if (i == 3) {
            endH = ImVec2(start.x + cornerW, start.y);
            endV = ImVec2(start.x, start.y - cornerH);
        }

        drawList->AddLine(start, endH, color, thickness);
        drawList->AddLine(start, endV, color, thickness);
    }
}

void BoxInfo(uintptr_t Entity, const ImVec4& color, const std::string& playerName, int distance,
    float expansionFactor = 2.0f, bool pbox = true, bool filterByDistance = true, int maxloot = 1200) {
    if (!Entity) return;

    ImDrawList* drawList = ImGui::GetOverlayDrawList();
    bool isDead = Game::is_dead(Entity);
    auto skeleton = Game::GetSkeleton(Entity, 1);
    auto visualState = Game::GeVisualState(Entity);
    if (!skeleton || !visualState) return;

    if (filterByDistance && distance > maxloot) return;

    if (isDead && showCorpseTag && distance <= maxloot) {
        Vector3 bonePos;
        if (Game::GetBonePosition(skeleton, visualState, 2, &bonePos)) {
            Vector3 screenPos3D;
            int dist = Game::GetDistanceToMe(bonePos);
            if (dist <= 0) return;

            if (Game::WorldToScreen(bonePos, screenPos3D)) {
                char corpseText[64];
                snprintf(corpseText, sizeof(corpseText), "Corpse - [%dM]", dist);

                ImVec2 textSize = ImGui::CalcTextSize(corpseText);
                ImVec2 textPos = ImVec2(
                    screenPos3D.x - textSize.x * 0.5f,
                    screenPos3D.y - textSize.y - 5.0f
                );

                ImU32 textColor = ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f));

                DrawTextWithOutline(drawList, textPos, textColor, corpseText);
            }

        }
        return;
    }

    std::vector<ImVec2> screenPoints;
    screenPoints.reserve(30);

    for (int i = 0; i < 30; i++) {
        Vector3 bonePos;
        if (!Game::GetBonePosition(skeleton, visualState, i, &bonePos)) continue;

        Vector3 screenPos3D;
        if (Game::WorldToScreen(bonePos, screenPos3D)) {
            screenPoints.push_back(ImVec2(screenPos3D.x, screenPos3D.y));
        }
    }

    if (screenPoints.empty()) return;

    float min_x = screenPoints[0].x, max_x = screenPoints[0].x;
    float min_y = screenPoints[0].y, max_y = screenPoints[0].y;

    for (const auto& point : screenPoints) {
        if (point.x < min_x) min_x = point.x;
        if (point.x > max_x) max_x = point.x;
        if (point.y < min_y) min_y = point.y;
        if (point.y > max_y) max_y = point.y;
    }

    ImVec2 min = ImVec2(min_x, min_y);
    ImVec2 max = ImVec2(max_x, max_y);

    float center_x = (min.x + max.x) * 0.5f;
    float center_y = (min.y + max.y) * 0.5f;
    float width = (max.x - min.x) * expansionFactor;
    float height = (max.y - min.y) * 1.02f;

    min.x = center_x - width * 0.5f;
    min.y = center_y - height * 0.5f;
    max.x = center_x + width * 0.5f;
    max.y = center_y + height * 0.5f;

    if (Playerbox) {
        ImU32 borderColor = IM_COL32(0, 0, 0, 255);
        ImU32 mainColor = ImGui::ColorConvertFloat4ToU32(color);

        DrawCornerBox(drawList, min, max, borderColor, 2.5f, 0.2f);
        DrawCornerBox(drawList, min, max, mainColor, 1.5f, 0.2f);
    }

    float fontScale = 0.90f;
    ImFont* font = ImGui::GetFont();
    float oldScale = font->Scale;
    font->Scale = fontScale;
    ImGui::PushFont(font);

    if (Playername) {
        std::string player_name = get_player_name(Entity);
        if (!player_name.empty()) {
            ImVec2 textSize = ImGui::CalcTextSize(player_name.c_str());
            ImVec2 textPos = ImVec2((min.x + max.x) * 0.5f - textSize.x * 0.5f, min.y - textSize.y - 5.0f);
            DrawTextWithOutline(drawList, textPos, ImColor(255, 255, 255), player_name);
        }
    }

    float baseY = max.y + 5.0f;
    float offsetY = 0.0f;
    float spacing = 13.0f;

    if (playerDistance) {
        std::string distance_text = "[" + std::to_string(distance) + "m]";
        ImVec2 textSize = ImGui::CalcTextSize(distance_text.c_str());
        ImVec2 textPos = ImVec2((min.x + max.x) * 0.5f - textSize.x * 0.5f, baseY + offsetY);
        DrawTextWithOutline(drawList, textPos, ImColor(255, 255, 255), distance_text);
        offsetY += spacing;
    }

    if (showItemInHands) {
        std::string item = Game::GetItemInHands(Entity);
        if (!item.empty()) {
            ImVec2 textSize = ImGui::CalcTextSize(item.c_str());
            ImVec2 textPos = ImVec2((min.x + max.x) * 0.5f - textSize.x * 0.5f, baseY + offsetY);
            DrawTextWithOutline(drawList, textPos, ImColor(255, 255, 255), item.c_str());
            offsetY += spacing;
        }
    }

    ImGui::PopFont();
    font->Scale = oldScale;
}

float Game::GetLife(uintptr_t entity) {
    if (!entity)
        return 0.0f;

    constexpr uintptr_t LIFE_OFFSET = 0x1C0;

    __try {
        return *reinterpret_cast<float*>(entity + LIFE_OFFSET);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return 0.0f;
    }
}

int get_random_head_bone() {
    int headBones[] = { 28, 51, 52, 50, 41, 26, 27, 49, 47, 43, 42, 48 };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, sizeof(headBones) / sizeof(int) - 1);

    return headBones[dist(gen)];
}

// Otimizado: Validação inline de entidade
inline bool IsEntityValid(uint64_t entity) {
    return entity && Game::IsValidPtr(reinterpret_cast<void*>(entity));
}

// Otimizado: Validação de distância
inline bool IsWithinDistance(int distance, int maxDistance) {
    return distance > 0 && distance <= maxDistance;
}

// Otimizado: Desenhar skeleton com menos chamadas
void DrawSkeleton(uintptr_t Entity, ImVec4 color, int skeletonType) {
    if (!IsEntityValid(Entity)) return;

    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetClientRect(hDesktop, &desktop);
    const int screenWidth = desktop.right;
    const int screenHeight = desktop.bottom;

    // Definição dos ossos conectados
    constexpr int bonePairs[][2] = {
        {6, 5}, {5, 4}, {4, 2}, {2, 1}, {1, 17},
        {14, 13}, {13, 12}, {12, 10}, {10, 9}, {9, 17},
        {17, 21}, {21, 94}, {94, 97}, {97, 136},
        {21, 61}, {61, 63}, {63, 89}
    };

    auto* drawList = ImGui::GetOverlayDrawList();
    const uint64_t skeleton = Game::GetSkeleton(Entity, skeletonType);
    const auto visualState = Game::GeVisualState(Entity);
    
    if (!skeleton || !visualState) return;

    const ImU32 colorU32 = ImGui::GetColorU32(color);

    // Desenhar linhas do skeleton
    for (const auto& [boneA, boneB] : bonePairs) {
        Vector3 posA, posB;
        if (!Game::GetBonePosition(skeleton, visualState, boneA, &posA) ||
            !Game::GetBonePosition(skeleton, visualState, boneB, &posB)) {
            continue;
        }

        Vector3 screenA, screenB;
        if (Game::WorldToScreen(posA, screenA) && Game::WorldToScreen(posB, screenB)) {
            drawList->AddLine(
                ImVec2(screenA.x, screenA.y),
                ImVec2(screenB.x, screenB.y),
                colorU32,
                1.0f
            );
        }
    }

    // Desenhar índices dos ossos (debug)
    if (PlayerJoyts) {
        for (int i = 0; i < 200; ++i) {
            Vector3 bone, screen;
            if (Game::GetBonePosition(skeleton, visualState, i, &bone) &&
                Game::WorldToScreen(bone, screen) &&
                screen.x > 0 && screen.y > 0 &&
                screen.x < screenWidth && screen.y < screenHeight) {
                drawList->AddText(
                    ImVec2(screen.x, screen.y),
                    colorU32,
                    std::to_string(i).c_str()
                );
            }
        }
    }
}

// Otimizado: Estrutura para cache de corpses com informações
struct CorpseInfo {
    uint64_t entityPtr;
    Vector3 position;
    std::string playerName;
    int distance;
    uint64_t lastUpdateTime;
    bool isValid;
};

std::vector<CorpseInfo> cachedCorpses;
static std::mutex corpseMutex;
static constexpr int MAX_CORPSE_DISTANCE = 2000;
static constexpr uint64_t CORPSE_CACHE_TIMEOUT = 5000; // 5 segundos

// Otimizado: Função para limpar corpses inválidos
void CleanupInvalidCorpses() {
    std::lock_guard<std::mutex> lock(corpseMutex);
    const uint64_t currentTime = GetTickCount64();
    
    cachedCorpses.erase(
        std::remove_if(cachedCorpses.begin(), cachedCorpses.end(),
            [currentTime](const CorpseInfo& corpse) {
                return !corpse.isValid || 
                       !Game::IsValidPtr(reinterpret_cast<void*>(corpse.entityPtr)) ||
                       !Game::is_dead(corpse.entityPtr) ||
                       (currentTime - corpse.lastUpdateTime) > CORPSE_CACHE_TIMEOUT;
            }),
        cachedCorpses.end()
    );
}

// Otimizado: Função para atualizar cache de corpses
void UpdateCorpses() {
    if (!globals.World) return;
    
    CleanupInvalidCorpses();
    
    const uint32_t slowTableSize = Game::SlowTableSize();
    if (slowTableSize == 0 || slowTableSize > 10000) return; // Proteção contra valores inválidos
    
    const uint64_t slowTable = Game::SlowTable();
    if (!Game::IsValidPtr(reinterpret_cast<void*>(slowTable))) return;
    
    const uint64_t localPlayer = Game::GetLocalPlayer();
    if (!localPlayer) return;
    
    const Vector3 localPos = Game::GetCoordinate(localPlayer);
    
    std::lock_guard<std::mutex> lock(corpseMutex);
    std::unordered_map<uint64_t, bool> existingCorpses;
    
    // Marcar corpses existentes
    for (auto& corpse : cachedCorpses) {
        existingCorpses[corpse.entityPtr] = true;
    }
    
    // Processar até 500 entidades por frame para performance
    const uint32_t maxProcess = (slowTableSize > 500u) ? 500u : slowTableSize;
    
    for (uint32_t i = 0; i < maxProcess; ++i) {
        const uint64_t entity = read<uint64_t>(slowTable + (i << 3));
        if (!Game::IsValidPtr(reinterpret_cast<void*>(entity))) continue;
        
        if (!Game::is_dead(entity)) continue;
        
        // Verificar se já está no cache
        if (existingCorpses.find(entity) != existingCorpses.end()) {
            // Atualizar informações do corpse existente
            for (auto& corpse : cachedCorpses) {
                if (corpse.entityPtr == entity) {
                    corpse.position = Game::GetCoordinate(entity);
                    corpse.distance = static_cast<int>(corpse.position.Distance(localPos));
                    corpse.lastUpdateTime = GetTickCount64();
                    corpse.isValid = true;
                    
                    // Tentar obter nome do jogador
                    if (corpse.playerName.empty()) {
                        try {
                            corpse.playerName = Game::GetPlayerName(entity);
                            if (corpse.playerName.empty()) {
                                corpse.playerName = "Unknown";
                            }
                        } catch (...) {
                            corpse.playerName = "Unknown";
                        }
                    }
                    break;
                }
            }
            continue;
        }
        
        // Novo corpse - adicionar ao cache
        const Vector3 entityPos = Game::GetCoordinate(entity);
        const int distance = static_cast<int>(entityPos.Distance(localPos));
        
        if (distance > MAX_CORPSE_DISTANCE) continue;
        
        CorpseInfo newCorpse;
        newCorpse.entityPtr = entity;
        newCorpse.position = entityPos;
        newCorpse.distance = distance;
        newCorpse.lastUpdateTime = GetTickCount64();
        newCorpse.isValid = true;
        
        // Tentar obter nome do jogador
        try {
            newCorpse.playerName = Game::GetPlayerName(entity);
            if (newCorpse.playerName.empty()) {
                newCorpse.playerName = "Corpse";
            }
        } catch (...) {
            newCorpse.playerName = "Corpse";
        }
        
        cachedCorpses.push_back(newCorpse);
    }
}

bool showCorpse = true;
ImVec4 corpseESPColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
bool showCorpseName = true;
bool showCorpseDistance = true;
bool showCorpseSkeleton = true;
bool showCorpseBox = true;
int maxCorpseDistance = 2000;

// Otimizado: Função super melhorada para desenhar corpses
void DrawCorpses() {
    if (!showCorpse || !globals.World) return;
    
    ImDrawList* drawList = ImGui::GetOverlayDrawList();
    if (!drawList) return;
    
    std::lock_guard<std::mutex> lock(corpseMutex);
    
    // Desenhar todos os corpses válidos do cache
    for (const auto& corpse : cachedCorpses) {
        if (!corpse.isValid || !Game::IsValidPtr(reinterpret_cast<void*>(corpse.entityPtr))) {
            continue;
        }
        
        // Verificar se ainda está morto
        if (!Game::is_dead(corpse.entityPtr)) {
            continue;
        }
        
        // Verificar distância
        if (corpse.distance > maxCorpseDistance) {
            continue;
        }
        
        Vector3 screenPos;
        if (!Game::WorldToScreen(corpse.position, screenPos)) {
            continue;
        }
  
        
        if (showCorpseSkeleton) {
            DrawSkeleton(corpse.entityPtr, corpseESPColor, 1);
        }
        
        if (screenPos.z > 0.01f) {
            float yOffset = 0.0f;
            
            if (showCorpseName && !corpse.playerName.empty()) {
                char nameText[256];
                snprintf(nameText, sizeof(nameText), "%s", corpse.playerName.c_str());
                ImVec2 nameSize = ImGui::CalcTextSize(nameText);
                ImVec2 namePos = ImVec2(screenPos.x - nameSize.x * 0.5f, screenPos.y - nameSize.y - yOffset);
                
                DrawTextWithOutline(drawList, namePos, ImGui::GetColorU32(corpseESPColor), nameText);
                yOffset += nameSize.y + 2.0f;
            }
            
            if (showCorpseDistance) {
                char distText[32];
                snprintf(distText, sizeof(distText), "[%dm]", corpse.distance);
                ImVec2 distSize = ImGui::CalcTextSize(distText);
                ImVec2 distPos = ImVec2(screenPos.x - distSize.x * 0.5f, screenPos.y - distSize.y - yOffset);
                
                DrawTextWithOutline(drawList, distPos, ImGui::GetColorU32(corpseESPColor), distText);
                yOffset += distSize.y + 2.0f;
            }
            
            const char* corpseTag = "CORPSE";
            ImVec2 tagSize = ImGui::CalcTextSize(corpseTag);
            ImVec2 tagPos = ImVec2(screenPos.x - tagSize.x * 0.5f, screenPos.y - tagSize.y - yOffset);
            
            ImVec4 tagColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            DrawTextWithOutline(drawList, tagPos, ImGui::GetColorU32(tagColor), corpseTag);
        }
    }
}

void DrawHumanBones(uintptr_t Entity, ImVec4 color, int s2) {
    auto* window = ImGui::GetOverlayDrawList();
    Vector3 bonePos;

    for (int boneIndex = 0; boneIndex < 130; ++boneIndex) {
        if (Game::GetBonePosition(Game::GetSkeleton(Entity, s2), Game::GeVisualState(Entity), boneIndex, &bonePos)) {
            Vector3 screenPos;
            if (Game::WorldToScreen(bonePos, screenPos)) {
                window->AddText(ImVec2(screenPos.x, screenPos.y), ImGui::GetColorU32(color), std::to_string(boneIndex).c_str());
            }
        }
    }
}

void DrawZombieSkeleton(uintptr_t Entity, ImVec4 color, int s2) {
    int vBone[][4] = {
        {19, 56}, {19, 24}, {56, 57}, {57, 58}, {58, 59}, {59, 86}, {86, 85}, {85, 60}, {60, 84}, {84, 61}, {61, 62},
        {24, 54}, {54, 25}, {25, 26}, {26, 53}, {53, 52}, {52, 27}, {27, 51}, {51, 29},
        {19, 18}, {18, 17}, {17, 16}, {16, 15},
        {15, 87}, {87, 9}, {9, 11}, {11, 12}, {12, 13},
        {15, 88}, {88, 2}, {2, 4}, {4, 5}, {5, 6}
    };

    auto* window = ImGui::GetOverlayDrawList();
    Vector3 previous = { 0, 0, 0 };
    Vector3 current;

    for (auto& part : vBone) {
        for (int i = 0; i < 2; ++i) {
            int boneIndex = part[i];
            if (boneIndex == 0) continue;

            if (!Game::GetBonePosition(Game::GetSkeleton(Entity, s2), Game::GeVisualState(Entity), boneIndex, &current))
                continue;

            Vector3 p1, c1;
            if (previous.x != 0.f && Game::WorldToScreen(previous, p1) && Game::WorldToScreen(current, c1)) {
                window->AddLine(ImVec2(p1.x, p1.y), ImVec2(c1.x, c1.y), ImGui::GetColorU32(color), 1.0f);
            }

            previous = current;
        }

        previous = { 0, 0, 0 };
    }
}

void ZombieBoxInfo(uintptr_t Entity, ImVec4 color) {
    if (!Entity)
        return;

    std::vector<ImVec2> screenPoints;

    for (int i = 0; i < 30; i++) {
        Vector3 bonePos;
        if (!Game::GetBonePosition(Game::GetSkeleton(Entity, 1), Game::GeVisualState(Entity), i, &bonePos))
            continue;

        Vector3 screenPos;
        if (Game::WorldToScreen(bonePos, screenPos))
            screenPoints.emplace_back(screenPos.x, screenPos.y);
    }

    if (screenPoints.empty())
        return;

    ImVec2 min = screenPoints[0], max = screenPoints[0];
    for (const auto& point : screenPoints) {
        if (point.x < min.x) min.x = point.x;
        if (point.y < min.y) min.y = point.y;
        if (point.x > max.x) max.x = point.x;
        if (point.y > max.y) max.y = point.y;
    }

    ImDrawList* draw = ImGui::GetOverlayDrawList();
    float thickness = 1.5f;
    ImU32 colMain = ImGui::ColorConvertFloat4ToU32(color);
    ImU32 colBorder = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1));

    draw->AddRect(min, max, colMain, 0.0f, 0, thickness);
    draw->AddRect(ImVec2(min.x - 1, min.y - 1), ImVec2(max.x + 1, max.y + 1), colBorder, 0.0f, 0, thickness);
    draw->AddRect(ImVec2(min.x + 1, min.y + 1), ImVec2(max.x - 1, max.y - 1), colBorder, 0.0f, 0, thickness);

    std::vector<std::pair<int, int>> zombieSkeleton = {
        {19, 56}, {19, 24}, {56, 57}, {57, 58}, {58, 59}, {59, 86}, {86, 85}, {85, 60}, {60, 84}, {84, 61}, {61, 62},
        {24, 54}, {54, 25}, {25, 26}, {26, 53}, {53, 52}, {52, 27}, {27, 51}, {51, 29},
        {19, 18}, {18, 17}, {17, 16}, {16, 15},
        {15, 87}, {87, 9}, {9, 11}, {11, 12}, {12, 13},
        {15, 88}, {88, 2}, {2, 4}, {4, 5}, {5, 6}
    };

    for (const auto& [a, b] : zombieSkeleton) {
        Vector3 p1, p2;
        if (Game::GetBonePosition(Game::GetSkeleton(Entity, 1), Game::GeVisualState(Entity), a, &p1) &&
            Game::GetBonePosition(Game::GetSkeleton(Entity, 1), Game::GeVisualState(Entity), b, &p2)) {
            Vector3 s1, s2;
            if (Game::WorldToScreen(p1, s1) && Game::WorldToScreen(p2, s2)) {
                draw->AddLine(ImVec2(s1.x, s1.y), ImVec2(s2.x, s2.y), colMain, 2.0f);
            }
        }
    }
}

ImU32 GetQualityColor(uint64_t entity) {
    static const std::array<ImU32, 5> qualityColors = {
        ImGui::ColorConvertFloat4ToU32(ImVec4(0, 255, 0, 255)),
        ImGui::ColorConvertFloat4ToU32(ImVec4(196, 201, 34, 255)),
        ImGui::ColorConvertFloat4ToU32(ImVec4(247, 95, 0, 255)),
        ImGui::ColorConvertFloat4ToU32(ImVec4(252, 185, 0, 255)),
        ImGui::ColorConvertFloat4ToU32(ImVec4(255, 0, 0, 255))
    };

    int quality = read<int>(entity + 0x180);
    return qualityColors[std::clamp(quality, 0, 4)];
}

std::string GetQuality(uint64_t entity) {
    int quality = read<int>(entity + 0x180);
    switch (quality) {
    case 1: return "Worn";
    case 2: return "Damaged";
    case 3: return "Good";
    case 4: return "Pristine";
    default: return "";
    }
}

struct NameId {
    UINT64 pt1;
    UINT64 pt2;

    bool operator<(const NameId& other) const {
        return std::tie(pt1, pt2) < std::tie(other.pt1, other.pt2);
    }
};

std::map<NameId, std::string> nameCache;

const std::array<const char*, 11> FILTERED_NAMES = {
    "Animal", "Zmb", "Firewood", "Barrel", "Watchtower",
    "Wood Pillar", "Roof", "Wall", "Floor", "Fireplace",
    "Wire Mesh Barrier"
};

// Otimizado: Obter nome de entidade com cache limitado
std::string GetNameFromId(uintptr_t namePointer) {
    if (!namePointer) return "";

    const NameId id = read<NameId>(namePointer + 0x10);

    // Verificar cache
    auto it = nameCache.find(id);
    if (it != nameCache.end()) {
        return it->second;
    }

    // Limitar tamanho do cache para evitar memory leak
    if (nameCache.size() > 5000) {
        nameCache.clear();
    }

    const int size = read<int>(namePointer + 0x8);
    if (size < 1 || size > 256) return "";

    std::string name = ReadArmaString(namePointer);
    if (name.empty()) return "";

    // Verificar filtros
    for (const auto& filter : FILTERED_NAMES) {
        if (name.find(filter) != std::string::npos) {
            nameCache[id] = "";
            return "";
        }
    }

    nameCache[id] = name;
    return name;
}

void DrawEntityLabel(ImDrawList* window, const ImVec2& pos, const std::string& name,
    int distance, ImU32 color, bool showQuality, uint64_t entity) {

    // Formatação melhorada sem background e sem linha
    std::string label = name;
    std::string distanceStr = "[" + std::to_string(distance) + "m]";
    
    ImVec2 textSize = ImGui::CalcTextSize(label.c_str());
    float padding = 4.0f;
    
    // Texto principal com sombra (sem fundo preto e sem linha)
    ImVec2 shadowPos = ImVec2(pos.x + 1, pos.y + 1);
    window->AddText(shadowPos, IM_COL32(0, 0, 0, 255), label.c_str());
    window->AddText(pos, color, label.c_str());
    
    // Distância ao lado do nome
    ImVec2 distPos = ImVec2(pos.x + textSize.x + padding * 2, pos.y);
    ImVec2 distShadowPos = ImVec2(distPos.x + 1, distPos.y + 1);
    window->AddText(distShadowPos, IM_COL32(0, 0, 0, 255), distanceStr.c_str());
    window->AddText(distPos, IM_COL32(255, 255, 255, 255), distanceStr.c_str());

    if (showQuality) {
        ImVec2 qualityPos = ImVec2(pos.x, pos.y + textSize.y + 2);
        std::string quality = GetQuality(entity);
        ImU32 qualityColor = GetQualityColor(entity);
        
        ImVec2 qualityShadowPos = ImVec2(qualityPos.x + 1, qualityPos.y + 1);
        window->AddText(qualityShadowPos, IM_COL32(0, 0, 0, 255), quality.c_str());
        window->AddText(qualityPos, qualityColor, quality.c_str());
    }
}

void IterateAll(uintptr_t worldptr, uintptr_t offset) {
    auto* window = ImGui::GetOverlayDrawList();
    ImGuiIO& io = ImGui::GetIO();

    int objectTableSz = read<int>(worldptr + offset + 0x8);
    if (objectTableSz < 1) return;

    uintptr_t entityTable = read<uintptr_t>(worldptr + offset);
    if (!Game::IsValidPtr((void*)entityTable)) return;

    Vector3 outPos;
    std::map<std::pair<float, float>, bool> textPositions;

    const int maxDistance = 1200;

    for (size_t i = 0; i < objectTableSz; i++) {
        uint64_t entity = read<uint64_t>(entityTable + i * 0x8);
        if (!Game::IsValidPtr((void*)entity)) continue;

        Vector3 worldPosition = Game::GetCoordinate(entity);
        Vector3 visualPosition = Game::GetObjectVisualState(entity);

        if (!Game::WorldToScreen(visualPosition, outPos)) continue;

        int distance = Game::GetDistanceToMe(worldPosition);
        if (distance <= 0 || distance > maxDistance) continue;

        uintptr_t objectBase = read<uintptr_t>(entity + EntityOffsets::OFF_EntityTypePtr);
        if (!Game::IsValidPtr((void*)objectBase)) continue;

        uintptr_t cleanNamePtr = read<uintptr_t>(objectBase + EntityOffsets::OFF_RealName);
        if (!Game::IsValidPtr((void*)cleanNamePtr)) continue;

        std::string text = GetNameFromId(cleanNamePtr);
        if (text.empty()) continue;

        ImVec2 textPos(outPos.x, outPos.y);
        while (textPositions[{ (int)textPos.x, (int)textPos.y }]) {
            textPos.y += 20.0f;
        }
        textPositions[{ (int)textPos.x, (int)textPos.y }] = true;

        DrawEntityLabel(window, textPos, text, distance,
            ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)),
            false, entity);
    }
}

ImVec4 colorWeapon = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
ImVec4 colorFood = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
ImVec4 colorAmmo = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
ImVec4 colorContainers = ImVec4(0.6f, 0.0f, 0.6f, 0.8f);
ImVec4 colorOptics = ImVec4(0.5f, 1.0f, 1.0f, 0.8f);
ImVec4 colorDrinks = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
ImVec4 colorCarWheels = ImVec4(0.0f, 0.0f, 1.0f, 0.8f);
ImVec4 colorBackpacks = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
ImVec4 colorOthers = ImVec4(0.7f, 0.7f, 0.7f, 0.8f);
ImVec4 colorMedicines = ImVec4(1.0f, 0.0f, 1.0f, 0.8f);
ImVec4 colorClothing = ImVec4(1.0f, 0.08f, 0.58f, 0.8f);

void IterateItems(uintptr_t pUWorld) {
    auto* window = ImGui::GetOverlayDrawList();
    const int maxDistance = maxloot; // Usa a distância configurável do menu
    const float spacing = 25.0f; // Espaçamento aumentado para melhor leitura

    int itemCount = read<int>(pUWorld + WorldOffsets::OFF_ItemTable + 0x8);
    uintptr_t itemTable = read<uintptr_t>(pUWorld + WorldOffsets::OFF_ItemTable);
    if (!Game::IsValidPtr((void*)itemTable)) return;

    std::map<std::pair<int, int>, bool> textPositions;
    valid_players2.clear();
    valid_players2.reserve(itemCount);

    for (size_t i = 0; i < itemCount; i++) {
        uintptr_t entity = read<uintptr_t>(itemTable + i * 0x18 + 0x8);
        uint32_t flag = read<uint32_t>(itemTable + i * 0x18);
        if (!Game::IsValidPtr((void*)entity) || flag != 1) continue;

        Vector3 worldPos = Game::GetCoordinate(entity);
        Vector3 visualPos = Game::GetObjectVisualState(entity);
        Vector3 screenPos;
        if (!Game::WorldToScreen(visualPos, screenPos)) continue;

        int distance = Game::GetDistanceToMe(worldPos);
        if (distance <= 0 || distance > maxDistance) continue;

        uintptr_t base = read<uintptr_t>(entity + EntityOffsets::OFF_EntityTypePtr);
        if (!Game::IsValidPtr((void*)base)) continue;

        uintptr_t namePtr = read<uintptr_t>(base + EntityOffsets::OFF_RealName);
        uintptr_t typePtr = read<uintptr_t>(base + EntityOffsets::OFF_EntityTypeName);
        uintptr_t modelPtr = read<uintptr_t>(base + EntityOffsets::OFF_ModelName);
        if (!Game::IsValidPtr((void*)namePtr) || !Game::IsValidPtr((void*)typePtr) || !Game::IsValidPtr((void*)modelPtr)) continue;

        std::string name = GetNameFromId(namePtr);
        std::string type = GetNameFromId(typePtr);
        std::string model = GetNameFromId(modelPtr);
        if (name.empty() || type.empty() || model.empty()) continue;

        ImVec2 textPos(screenPos.x, screenPos.y);
        while (textPositions[{ (int)textPos.x, (int)textPos.y }]) textPos.y += spacing;
        textPositions[{ (int)textPos.x, (int)textPos.y }] = true;

        ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1));
        bool draw = false;
        bool showQualityHere = false;

        if (weapon && type.find("Weapon") != std::string::npos) {
            color = ImGui::ColorConvertFloat4ToU32(colorWeapon);
            draw = true;
            showQualityHere = qualidade;
        }
        else if (BackPack && model.find("backpacks") != std::string::npos) {
            color = ImGui::ColorConvertFloat4ToU32(colorBackpacks);
            draw = true;
            showQualityHere = qualidade;
        }
        else if (Containers && (model.find("containers") != std::string::npos || model.find("cooking") != std::string::npos || model.find("fence") != std::string::npos || name.find("Fence") != std::string::npos || name.find("Cerca") != std::string::npos)) {
            color = ImGui::ColorConvertFloat4ToU32(colorContainers);
            draw = true;
        }
        else if (comida && model.find("food") != std::string::npos) {
            color = ImGui::ColorConvertFloat4ToU32(colorFood);
            draw = true;
            showQualityHere = qualidade;
        }
        else if (medicine && model.find("medical") != std::string::npos) {
            color = ImGui::ColorConvertFloat4ToU32(colorMedicines);
            draw = true;
            showQualityHere = qualidade;
        }
        else if (drinks && model.find("drinks") != std::string::npos) {
            color = ImGui::ColorConvertFloat4ToU32(colorDrinks);
            draw = true;
            showQualityHere = qualidade;
        }
        else if (clothing && type.find("clothing") != std::string::npos) {
            color = ImGui::ColorConvertFloat4ToU32(colorClothing);
            draw = true;
            showQualityHere = qualidade;
        }
        else if (inventoryItem && type.find("inventoryItem") != std::string::npos) {
            if (model.find("food") == std::string::npos &&
                model.find("medical") == std::string::npos &&
                model.find("drinks") == std::string::npos &&
                model.find("Fogueira") == std::string::npos &&
                name.find("Cerca") == std::string::npos) {
                color = ImGui::ColorConvertFloat4ToU32(colorOthers);
                draw = true;
                showQualityHere = qualidade;
            }
        }
        else if (ProxyMagazines && (type.find("ProxyMagazines") != std::string::npos || model.find("ammunition") != std::string::npos)) {
            color = ImGui::ColorConvertFloat4ToU32(colorAmmo);
            draw = true;
        }

        if (draw) {
            DrawEntityLabel(window, textPos, name, distance, color, showQualityHere, entity);
        }

        valid_players2.push_back((uintptr_t*)entity);
    }
}

void DrawZombieBones(uintptr_t Entity, ImVec4 color, int s2) {
    auto* window = ImGui::GetOverlayDrawList();
    Vector3 bonePos;

    for (int boneIndex = 0; boneIndex < 125; ++boneIndex) {
        if (Game::GetBonePosition(Game::GetSkeleton(Entity, s2), Game::GeVisualState(Entity), boneIndex, &bonePos)) {
            Vector3 screenPos;
            if (Game::WorldToScreen(bonePos, screenPos)) {
                window->AddText(ImVec2(screenPos.x, screenPos.y), ImGui::GetColorU32(color), std::to_string(boneIndex).c_str());
            }
        }
    }
}

void DrawEntityInfo(uintptr_t Entity, const Vector3& screenPosition, const std::string& text, ImVec4 color) {
    auto* window = ImGui::GetOverlayDrawList();
    window->AddText(ImVec2(screenPosition.x, screenPosition.y), ImGui::GetColorU32(color), text.c_str());
}

int bonechoose = 0;

float sqrtf1(float _X) {
    return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(_X)));
}

int desynckey;

uintptr_t* get_closest_Item(std::vector<uintptr_t*> list, float field_of_view) {
    ImGuiIO& io = ImGui::GetIO();

    uintptr_t* resultant_target_entity_temp = nullptr;
    float closestTocenter = FLT_MAX;

    for (auto curr_entity : list) {
        Vector3 currentworld;
        Vector3 Pos = Game::GetObjectVisualState((uintptr_t)curr_entity);
        Game::WorldToScreen(Pos, currentworld);
        auto dx = currentworld.x - (io.DisplaySize.x / 2);

        auto dy = currentworld.y - (io.DisplaySize.y / 2);
        auto dist = sqrtf1(dx * dx + dy * dy);
        if (dist < field_of_view && dist < closestTocenter) {
            closestTocenter = dist;
            resultant_target_entity_temp = curr_entity;
        }
    }
    return resultant_target_entity_temp;
}

// Otimizado: Encontrar jogador mais próximo do centro da tela
uintptr_t* get_closest_player(std::vector<uintptr_t*> list, float field_of_view) {
    if (list.empty()) return nullptr;

    ImGuiIO& io = ImGui::GetIO();
    const float centerX = io.DisplaySize.x * 0.5f;
    const float centerY = io.DisplaySize.y * 0.5f;
    const float fovSquared = field_of_view * field_of_view;

    uintptr_t* closest_player = nullptr;
    float closest_distance_squared = FLT_MAX;

    for (auto currentEntity : list) {
        if (!Game::IsValidPtr(currentEntity)) continue;
        
        // Verificar se está morto
        if (Game::is_dead(reinterpret_cast<uint64_t>(currentEntity))) continue;

        const uint64_t skeleton = read<uint64_t>(reinterpret_cast<uint64_t>(currentEntity) + PlayerOffsets::OFF_PlayerSkeleton);
        if (!Game::IsValidPtr(reinterpret_cast<void*>(skeleton))) continue;

        // Usar osso da cabeça (24) para consistência
        Vector3 bonePosition, screenPosition;
        if (!Game::GetBonePosition(skeleton, Game::GeVisualState(reinterpret_cast<uint64_t>(currentEntity)), 24, &bonePosition))
            continue;

        if (!Game::WorldToScreen(bonePosition, screenPosition) || screenPosition.z <= 0.01f)
            continue;

        // Usar distância ao quadrado para evitar sqrt
        const float dx = screenPosition.x - centerX;
        const float dy = screenPosition.y - centerY;
        const float distanceSquared = dx * dx + dy * dy;

        if (distanceSquared < fovSquared && distanceSquared < closest_distance_squared) {
            closest_distance_squared = distanceSquared;
            closest_player = currentEntity;
        }
    }

    return closest_player;
}


bool ActiveSilentAim = false;

void DrawBoneMarker(const Vector3& screenPos) {
    auto* window = ImGui::GetOverlayDrawList();
    ImVec4 corTexto = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    window->AddText(ImVec2{ screenPos.x - 5, screenPos.y - 5 }, ImGui::ColorConvertFloat4ToU32(corTexto), "");
}

void RedirectBulletsToBone(const Vector3& bonePosition, int distancia) {
    const auto pUWorld = read<uintptr_t>(globals.Base + WorldOffsets::OFF_World);
    if (!Game::IsValidPtr((void*)pUWorld)) return;

    const uintptr_t entityTable = read<uintptr_t>(pUWorld + MiscOffsets::OFF_Bullets);
    const int objectTableSz = read<int>(pUWorld + MiscOffsets::OFF_Bulletsize);
    if (!Game::IsValidPtr((void*)entityTable) || objectTableSz < 1) return;

    for (int i = 0; i < objectTableSz; i++) {
        const uint64_t bullet = read<uint64_t>(entityTable + (i * 0x8));
        if (!Game::IsValidPtr((void*)bullet)) continue;

        if (ActiveSilentAim) {
            Game::SetPosition(bullet, bonePosition);
            write<int>(bullet + 0x360, 12000);
            write<int>(bullet + 0x364, 12000);
        }
    }
}

// Otimizado: Silent Aim com validações melhoradas
void SilentAim1(uint64_t Entity, int skeletonType) {
    if (!IsEntityValid(Entity) || Game::is_dead(Entity)) return;

    auto* window = ImGui::GetOverlayDrawList();
    ImGuiIO& io = ImGui::GetIO();

    // Validar distância máxima
    const Vector3 worldPosition = Game::GetCoordinate(Entity);
    const int distance = Game::GetDistanceToMe(worldPosition);
    if (!IsWithinDistance(distance, LimiteDoSilent)) return;

    // Mapeamento de ossos
    constexpr int boneMap[] = { 24, 31, 17, 15, 5, 14 };
    constexpr int maxBoneIndex = sizeof(boneMap) / sizeof(int) - 1;
    
    // Selecionar osso com validação de bounds
    int selectedBone = 15; // Padrão
    if (bonechoose >= 0 && bonechoose <= maxBoneIndex) {
        selectedBone = boneMap[bonechoose];
    }
    if (skeletonType == 2) selectedBone = 15;

    // Validar posição na tela
    Vector3 visualPos = Game::GetObjectVisualState(Entity);
    Vector3 screenPos;
    if (!Game::WorldToScreen(visualPos, screenPos)) return;

    // Validar skeleton
    const uintptr_t skeleton = read<uint64_t>(Entity + PlayerOffsets::OFF_PlayerSkeleton);
    if (!Game::IsValidPtr(reinterpret_cast<void*>(skeleton))) return;

    // Obter posição do osso
    Vector3 bonePos, boneScreenPos;
    if (!Game::GetBonePosition(skeleton, Game::GeVisualState(Entity), selectedBone, &bonePos))
        return;

    if (bonePos.isZ0()) return;

    // Modificar velocidade da munição se ativado
    if (AmmoSpeedChange) {
        const uintptr_t dayZPlayer = Game::GetLocalPlayer();
        if (ISVALID(dayZPlayer)) {
            const uintptr_t playerInventory = read<uintptr_t>(dayZPlayer + 0x658);
            if (ISVALID(playerInventory)) {
                const uintptr_t inventoryHands = read<uintptr_t>(playerInventory + 0x1B0);
                if (ISVALID(inventoryHands)) {
                    const uintptr_t weaponInfoTable = read<uintptr_t>(inventoryHands + 0x6B0);
                    if (ISVALID(weaponInfoTable)) {
                        const uintptr_t ammoType = read<uintptr_t>(weaponInfoTable + 0x20);
                        if (IsValidPtr2(reinterpret_cast<void*>(ammoType + 0x364))) {
                            const float newSpeed = static_cast<float>(distance * 100.f);
                            write<float>(ammoType + 0x364, newSpeed);
                        }
                    }
                }
            }
        }
    }

    // Desenhar marcador e linha
    if (Game::WorldToScreen(bonePos, boneScreenPos) && boneScreenPos.z > 0.01f) {
        const ImU32 redColor = IM_COL32(255, 0, 0, 255);
        
        // Desenhar linha do centro para o osso
        window->AddLine(
            ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
            ImVec2(boneScreenPos.x, boneScreenPos.y),
            redColor,
            0.2f
        );

        // Redirecionar balas
        RedirectBulletsToBone(bonePos, distance);
    }
}

// Otimizado: Aimbot principal com validações
inline void __stdcall main_aimboo2(int skeletonType) {
    ImGuiIO& io = ImGui::GetIO();

    // Verificar se a tecla está pressionada
    if (!GetAsyncKeyState(desynckey)) {
        resultant_target_entity = nullptr;
        return;
    }

    // Buscar novo alvo se não houver um
    if (!resultant_target_entity) {
        resultant_target_entity = get_closest_player(valid_players, aimfov);
    }
    
    if (!resultant_target_entity) return;

    // Validar se o alvo ainda é válido
    if (!IsEntityValid(reinterpret_cast<uint64_t>(resultant_target_entity))) {
        resultant_target_entity = nullptr;
        return;
    }

    // Validar se o alvo não está morto
    if (Game::is_dead(reinterpret_cast<uint64_t>(resultant_target_entity))) {
        resultant_target_entity = nullptr;
        return;
    }

    // Validar distância máxima
    const Vector3 worldPosition = Game::GetCoordinate(reinterpret_cast<uint64_t>(resultant_target_entity));
    const int distance = Game::GetDistanceToMe(worldPosition);
    
    if (!IsWithinDistance(distance, LimiteDoSilent)) {
        resultant_target_entity = nullptr;
        return;
    }
}

uintptr_t* get_closest_Item1(std::vector<uintptr_t*> list, float field_of_view) {
    ImGuiIO& io = ImGui::GetIO();
    uintptr_t* closest_item = nullptr;
    float closest_screen_dist = FLT_MAX;
    Vector3 my_pos = Game::GetCoordinate(Game::GetLocalPlayer());

    for (auto curr_entity : list) {
        if (!Game::IsValidPtr(curr_entity))
            continue;

        Vector3 world_pos = Game::GetObjectVisualState((uintptr_t)curr_entity);
        float distance = my_pos.Distance(world_pos);
        if (distance > 10.0f)
            continue;

        Vector3 screen_pos;
        if (!Game::WorldToScreen(world_pos, screen_pos))
            continue;

        float dx = screen_pos.x - (io.DisplaySize.x / 2);
        float dy = screen_pos.y - (io.DisplaySize.y / 2);
        float screen_dist = sqrtf1(dx * dx + dy * dy);

        if (screen_dist < field_of_view && screen_dist < closest_screen_dist) {
            closest_screen_dist = screen_dist;
            closest_item = curr_entity;
        }
    }

    return closest_item;
}

uintptr_t* get_closest_Item2(std::vector<uintptr_t*> list, float field_of_view) {
    ImGuiIO& io = ImGui::GetIO();
    uintptr_t* resultant_target_entity_temp = nullptr;

    for (auto curr_entity : list) {
        Vector3 currentworld;
        Vector3 Pos = Game::GetObjectVisualState((uintptr_t)curr_entity);
        Game::WorldToScreen(Pos, currentworld);

        float pythag_result = sqrtf(
            pow(((float)io.DisplaySize.x / 2.0f - currentworld.x), 2.0f) +
            pow(((float)io.DisplaySize.y / 2.0f - currentworld.y), 2.0f)
        );


        auto closest_fov = aimfov;

        if (pythag_result < aimfov && closest_fov > pythag_result) {
            resultant_target_entity_temp = curr_entity;
            closest_fov = pythag_result;
        }
    }

    return resultant_target_entity_temp;
}

int LootTP = VK_HOME;

void lootteleport() {
    ImGuiIO& io = ImGui::GetIO();
    auto* window = ImGui::GetOverlayDrawList();

    uint64_t ff = (uint64_t)get_closest_Item1(valid_players2, aimfov);
    if (!ff)
        return;
    Vector3 currentworld;
    Vector3 Pos = Game::GetObjectVisualState(ff);
    Game::WorldToScreen(Pos, currentworld);
    float lineWidth = 1.0f;
    window->AddLine(
        ImVec2{ (float)io.DisplaySize.x / 2, (float)io.DisplaySize.y / 2 },
        ImVec2{ currentworld.x, currentworld.y },
        ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 1.0f, 1.0f))
    );

    window->AddLine(
        ImVec2{ (float)io.DisplaySize.x / 2 + 1.0f, (float)io.DisplaySize.y / 2 + 1.0f },
        ImVec2{ currentworld.x + 1.0f, currentworld.y + 1.0f },
        ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.5f, 1.0f, 1.0f))
    );

    if (GetAsyncKeyState(LootTP)) {
        auto MyPlayer = Game::GetLocalPlayer();
        if (!Game::IsValidPtr((void*)MyPlayer))
            return;
        Vector3 worldPosition2 = Game::GetCoordinate(MyPlayer);
        Game::SetPosition(ff, worldPosition2);
    }
}

void bodyteleport() {
    ImGuiIO& io = ImGui::GetIO();
    auto* window = ImGui::GetOverlayDrawList();

    uint64_t closestEntity = (uint64_t)get_closest_Item1(valid_players2, aimfov);
    if (!closestEntity || !Game::is_dead(closestEntity)) return;

    Vector3 currentworld;
    Vector3 Pos = Game::GetObjectVisualState(closestEntity);
    Game::WorldToScreen(Pos, currentworld);

    window->AddLine(ImVec2{ (float)io.DisplaySize.x / 2, (float)io.DisplaySize.y / 2 },
        ImVec2{ currentworld.x, currentworld.y },
        ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 1.0f, 1.0f)));

    window->AddLine(ImVec2{ (float)io.DisplaySize.x / 2 + 1.0f, (float)io.DisplaySize.y / 2 + 1.0f },
        ImVec2{ currentworld.x + 1.0f, currentworld.y + 1.0f },
        ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.5f, 1.0f, 1.0f)));

    if (GetAsyncKeyState(LootTP)) {
        auto MyPlayer = Game::GetLocalPlayer();
        if (!Game::IsValidPtr((void*)MyPlayer)) return;

        Vector3 playerPos = Game::GetCoordinate(MyPlayer);
        Game::SetPosition(closestEntity, playerPos);
    }
}

auto color = ImGui::ColorConvertFloat4ToU32(ImVec4(RUSVeh11[0], RUSVeh11[1], RUSVeh11[2], RUSVeh11[3]));
auto color1 = ImGui::ColorConvertFloat4ToU32(ImVec4(BRTVeh[0], BRTVeh[1], BRTVeh[2], BRTVeh[3]));
auto color2 = ImGui::ColorConvertFloat4ToU32(ImVec4(RUSVeh[0], RUSVeh[1], RUSVeh[2], RUSVeh[3]));
auto color3 = ImGui::ColorConvertFloat4ToU32(ImVec4(FOVRADIO[0], FOVRADIO[1], FOVRADIO[2], FOVRADIO[3]));
auto color4 = ImGui::ColorConvertFloat4ToU32(ImVec4(HAB[0], HAB[1], HAB[2], HAB[3]));

void CustomCheckBox(const char* label, bool* value) {
    ImGui::BeginGroup();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size(13.0f, 13.0f);

    ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), *value ? IM_COL32(255, 0, 0, 255) : IM_COL32(50, 50, 50, 255));
    ImGui::GetWindowDrawList()->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(200, 200, 200, 255));
    if (ImGui::IsMouseHoveringRect(pos, ImVec2(pos.x + size.x, pos.y + size.y))) {
        if (ImGui::IsMouseClicked(0)) {
            *value = !(*value);
        }
    }

    ImGui::Dummy(size);
    ImGui::SameLine();
    ImGui::Text(label);
    ImGui::EndGroup();
}

void CustomRadio(const char* label, bool* value) {
    ImGui::BeginGroup();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size(13.0f, 13.0f);

    ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), *value ? IM_COL32(255, 0, 0, 255) : IM_COL32(50, 50, 50, 255));
    ImGui::GetWindowDrawList()->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(200, 200, 200, 255));
    if (ImGui::IsMouseHoveringRect(pos, ImVec2(pos.x + size.x, pos.y + size.y))) {
        if (ImGui::IsMouseClicked(0)) {
            *value = !(*value);
        }
    }

    ImGui::Dummy(size);
    ImGui::SameLine();
    ImGui::Text(label);
    ImGui::EndGroup();
}

int menuKey = VK_INSERT;

bool Tab(const char* label, const ImVec2& size_arg, bool state) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);
    if (pressed)
        ImGui::MarkItemEdited(id);

    ImGui::RenderFrame(bb.Min, bb.Max, state ? ImColor(15, 15, 15) : ImColor(23, 23, 23), true, style.FrameRounding);
    window->DrawList->AddRect(bb.Min, bb.Max, ImColor(0, 0, 0));
    ImGui::RenderTextClipped(ImVec2(bb.Min.x + style.FramePadding.x, bb.Min.y + style.FramePadding.y),
        ImVec2(bb.Max.x - style.FramePadding.x, bb.Max.y - style.FramePadding.y), label, NULL, &label_size,
        style.ButtonTextAlign, &bb);

    if (state) {
        ImVec2 lineStart1(bb.Min.x + 9, bb.Min.y);
        ImVec2 lineStart2(bb.Min.x, bb.Min.y + 9);
        ImVec2 lineEnd1(bb.Max.x - 10, bb.Max.y - 1);
        ImVec2 lineEnd2(bb.Max.x - 1, bb.Max.y - 1);

        window->DrawList->AddLine(bb.Min, lineStart1, ImColor(255, 255, 255), 1);
        window->DrawList->AddLine(bb.Min, lineStart2, ImColor(255, 255, 255), 1);

        window->DrawList->AddLine(ImVec2(bb.Max.x, bb.Max.y - 1), lineEnd1, ImColor(255, 255, 255), 1);
        window->DrawList->AddLine(ImVec2(bb.Max.x - 1, bb.Max.y - 1), lineEnd2, ImColor(255, 255, 255), 1);
    }

    return pressed;
}

static LPDIRECT3D9 g_pD3D = NULL;
static LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS g_d3dpp = {};

auto s = ImVec2{}, p = ImVec2{}, gs = ImVec2{ 662, 460 };

IDirect3DTexture9* bgs = nullptr;
IDirect3DTexture9* logo = nullptr;
IDirect3DTexture9* circle = nullptr;
IDirect3DTexture9* foto = nullptr;
IDirect3DTexture9* merfthefurry = nullptr;
IDirect3DTexture9* ghoul = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();

static int tab;
static int enemy_tab;
bool the_bool;
int LootKey = VK_F3;
float valueF;

namespace ImGui {
    void CustomColor() {
        auto* colors = ImGui::GetStyle().Colors;
        ImGuiStyle& style = ImGui::GetStyle();

        style.FrameRounding = 8.0f;
        style.GrabRounding = 8.0f;
        style.WindowRounding = 10.0f;
        style.ScrollbarRounding = 8.0f;
        style.WindowBorderSize = 1.0f;

        colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.0f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.94f);
        colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.3f, 0.0f, 0.0f, 1.0f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.08f, 0.08f, 0.08f, 0.75f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);
        colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.6f, 0.1f, 0.1f, 1.0f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.9f, 0.0f, 0.0f, 1.0f);
        colors[ImGuiCol_Header] = ImVec4(0.2f, 0.0f, 0.0f, 0.7f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.0f, 0.0f, 1.0f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.5f, 0.0f, 0.0f, 1.0f);
        colors[ImGuiCol_Separator] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.6f, 0.1f, 0.1f, 1.0f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.9f, 0.0f, 0.0f, 1.0f);
        colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.4f, 0.0f, 0.0f, 1.0f);
        colors[ImGuiCol_TabActive] = ImVec4(0.6f, 0.0f, 0.0f, 1.0f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.15f, 0.15f, 0.7f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.3f, 0.0f, 0.0f, 0.8f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.6f, 0.0f, 0.0f, 0.5f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.7f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.7f, 0.0f, 0.0f, 1.0f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.7f, 0.7f, 0.7f, 0.4f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.9f, 0.1f, 0.1f, 0.7f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    }

    struct tab_anim
    {
        float hover = 0.0f;   // changed to float — makes more sense for lerp/clamp
        float active = 0.0f;
    };

    bool tab(const char* label, bool selected)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);
        ImVec2 pos = window->DC.CursorPos;

        // Tamanho fixo mais bonito
        ImVec2 size = ImVec2(label_size.x + 20.f, 26.f);

        // Calculate max position manually
        ImVec2 max_pos = ImVec2(pos.x + size.x, pos.y + size.y);
        ImRect rect(pos, max_pos);

        ImGui::ItemSize(size, style.FramePadding.y);
        if (!ImGui::ItemAdd(rect, id))
            return false;

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(rect, id, &hovered, &held);

        // =======================
        // Animations
        // =======================
        static std::map<ImGuiID, tab_anim> anim;
        auto& a = anim[id];

        float speed = 10.f * g.IO.DeltaTime;

        // Convert bool to float (1.0f for true, -1.0f for false)
        float hover_direction = hovered ? 1.0f : -1.0f;
        float active_direction = selected ? 1.0f : -1.0f;

        a.hover = ImClamp(a.hover + (speed * hover_direction), 0.0f, 1.0f);
        a.active = ImClamp(a.active + (speed * 1.5f * active_direction), 0.0f, 1.0f);

        // =======================
        // Colors
        // =======================
        ImVec4 base_text = ImVec4(1.f, 1.f, 1.f, 0.55f);
        ImVec4 hover_text = ImVec4(1.f, 1.f, 1.f, 0.80f);
        ImVec4 active_text = ImVec4(1.f, 1.f, 1.f, 1.00f);

        ImVec4 text_col = ImLerp(base_text, hover_text, a.hover);
        text_col = ImLerp(text_col, active_text, a.active);

        // =======================
        // Render
        // =======================
        ImVec2 text_pos = ImVec2(
            rect.Min.x + (size.x - label_size.x) * 0.5f,
            rect.Min.y + (size.y - label_size.y) * 0.5f
        );

        window->DrawList->AddText(
            text_pos,
            ImGui::GetColorU32(text_col),
            label
        );

        // Indicador ativo (underline)
        if (a.active > 0.01f)
        {
            float line_height = 2.f;
            window->DrawList->AddRectFilled(
                ImVec2(rect.Min.x + 6.f, rect.Max.y - line_height),
                ImVec2(rect.Max.x - 6.f, rect.Max.y),
                ImGui::GetColorU32(ImVec4(0.0f, 0.8f, 1.0f, a.active)),
                2.f
            );
        }

        return pressed;
    }
}

ImVec4 animalESPColor = ImVec4(1.0f, 0.733f, 0.0f, 1.0f);
ImVec4 playerESPColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
ImVec4 carESPColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
ImVec4 infectedESPColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
ImVec4 fovcolorcc = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
ImVec4 crosshairColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

float ImVec2DistanceSqr(const ImVec2& a, const ImVec2& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

void SanitizeValues() {
    aimfov = std::clamp(aimfov, 0.0f, 300.0f);
    eyeAccomValue = std::clamp(eyeAccomValue, 5.0f, 100.0f);
    Claridade = std::clamp(Claridade, 1.0f, 90.0f);
    Horizontal = std::clamp(Horizontal, 0.1f, 5.0f);
    Vertical = std::clamp(Vertical, -1.0f, 10.0f);
    fovThickness = std::clamp(fovThickness, 1.0f, 10.0f); // Add thickness clamping
    fovShape = std::clamp(fovShape, 0, 1); // Ensure shape is valid
}

// Funções de Config
void EnsureDirectoryExists(const std::string& path) {
    std::filesystem::path dirPath(path);
    if (!std::filesystem::exists(dirPath)) {
        try {
            std::filesystem::create_directories(dirPath);
            std::cout << "Directory created: " << path << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error creating directory: " << e.what() << std::endl;
        }
    }
}

void LoadSettings(const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file for reading: " << filename << std::endl;
        return;
    }

    // Ler todas as configurações
    file.read(reinterpret_cast<char*>(&g_GameVars.playerESPDistance), sizeof(int));
    file.read(reinterpret_cast<char*>(&g_GameVars.infectedESPDistance), sizeof(int));
    file.read(reinterpret_cast<char*>(&g_GameVars.carESPDistance), sizeof(int));
    file.read(reinterpret_cast<char*>(&g_GameVars.animalESPDistance), sizeof(int));
    file.read(reinterpret_cast<char*>(&g_GameVars.maxloot), sizeof(int));
    file.read(reinterpret_cast<char*>(&g_GameVars.LimiteDoSilent), sizeof(int));
    file.read(reinterpret_cast<char*>(&g_GameVars.enablePlayerESP), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.enableInfectedESP), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.enableCarESP), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.enableInventoryESP), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.enableAnimalESP), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.PlayeEsp), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.Playerbox), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.Playername), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.playerDistance), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.PlayerSkeleton), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.espline), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.carespline), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.ActiveSilentAim), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.aimfov), sizeof(float));
    file.read(reinterpret_cast<char*>(&g_GameVars.fovThickness), sizeof(float));
    file.read(reinterpret_cast<char*>(&g_GameVars.ToggleFov), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.EnableFovRGB), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.AmmoSpeedChange), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.bonechoose), sizeof(int));
    file.read(reinterpret_cast<char*>(&g_GameVars.Iemtelist), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.weapon), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.comida), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.ProxyMagazines), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.Containers), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.Miras), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.drinks), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.rodas), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.BackPack), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.inventoryItem), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.medicine), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.clothing), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.ToggleCross), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.removeGrass), sizeof(bool));
    file.read(reinterpret_cast<char*>(&g_GameVars.Claridade), sizeof(float));
    file.read(reinterpret_cast<char*>(&g_GameVars.eyeAccomValue), sizeof(float));
    file.read(reinterpret_cast<char*>(&g_GameVars.Horizontal), sizeof(float));
    file.read(reinterpret_cast<char*>(&g_GameVars.Vertical), sizeof(float));
    file.read(reinterpret_cast<char*>(&g_GameVars.overcast), sizeof(float));
    file.read(reinterpret_cast<char*>(&g_GameVars.fog), sizeof(float));
    file.read(reinterpret_cast<char*>(&g_GameVars.rain), sizeof(float));
    file.read(reinterpret_cast<char*>(&g_GameVars.snowfall), sizeof(float));
    //file.read(reinterpret_cast<char*>(&g_GameVars.streamproof), sizeof(bool));
    //file.read(reinterpret_cast<char*>(&g_GameVars.espzDistance), sizeof(int));
    //
    // Crosshair settings
    file.read(reinterpret_cast<char*>(&g_GameVars.crosshairSize), sizeof(float));
    file.read(reinterpret_cast<char*>(&g_GameVars.crosshairGap), sizeof(float));
    file.read(reinterpret_cast<char*>(&g_GameVars.crosshairThickness), sizeof(float));
    //file.read(reinterpret_cast<char*>(&g_GameVars.EnableCrossRGB), sizeof(bool));
    //file.read(reinterpret_cast<char*>(&g_GameVars.crosshairColor), sizeof(float) * 4);
    //
    //// ESP Colors
    //file.read(reinterpret_cast<char*>(&g_GameVars.playerESPColor), sizeof(float) * 4);
    //file.read(reinterpret_cast<char*>(&g_GameVars.infectedESPColor), sizeof(float) * 4);
    //file.read(reinterpret_cast<char*>(&g_GameVars.carESPColor), sizeof(float) * 4);
    //file.read(reinterpret_cast<char*>(&g_GameVars.animalESPColor), sizeof(float) * 4);
    //file.read(reinterpret_cast<char*>(&g_GameVars.TesTESP), sizeof(float) * 4);

    file.close();

    // Aplicar as configurações carregadas às variáveis globais
    playerESPDistance = g_GameVars.playerESPDistance;
    infectedESPDistance = g_GameVars.infectedESPDistance;
    carESPDistance = g_GameVars.carESPDistance;
    animalESPDistance = g_GameVars.animalESPDistance;
    maxloot = g_GameVars.maxloot;
    LimiteDoSilent = g_GameVars.LimiteDoSilent;
    enablePlayerESP = g_GameVars.enablePlayerESP;
    enableInfectedESP = g_GameVars.enableInfectedESP;
    enableCarESP = g_GameVars.enableCarESP;
    enableInventoryESP = g_GameVars.enableInventoryESP;
    enableAnimalESP = g_GameVars.enableAnimalESP;
    PlayeEsp = g_GameVars.PlayeEsp;
    Playerbox = g_GameVars.Playerbox;
    Playername = g_GameVars.Playername;
    playerDistance = g_GameVars.playerDistance;
    PlayerSkeleton = g_GameVars.PlayerSkeleton;
    espline = g_GameVars.espline;
    carespline = g_GameVars.carespline;
    ActiveSilentAim = g_GameVars.ActiveSilentAim;
    aimfov = g_GameVars.aimfov;
    fovThickness = g_GameVars.fovThickness;
    ToggleFov = g_GameVars.ToggleFov;
    EnableFovRGB = g_GameVars.EnableFovRGB;
    AmmoSpeedChange = g_GameVars.AmmoSpeedChange;
    bonechoose = g_GameVars.bonechoose;
    Iemtelist = g_GameVars.Iemtelist;
    weapon = g_GameVars.weapon;
    comida = g_GameVars.comida;
    ProxyMagazines = g_GameVars.ProxyMagazines;
    Containers = g_GameVars.Containers;
    Miras = g_GameVars.Miras;
    drinks = g_GameVars.drinks;
    rodas = g_GameVars.rodas;
    BackPack = g_GameVars.BackPack;
    inventoryItem = g_GameVars.inventoryItem;
    medicine = g_GameVars.medicine;
    clothing = g_GameVars.clothing;
    ToggleCross = g_GameVars.ToggleCross;
    removeGrass = g_GameVars.removeGrass;
    Claridade = g_GameVars.Claridade;
    eyeAccomValue = g_GameVars.eyeAccomValue;
    Horizontal = g_GameVars.Horizontal;
    Vertical = g_GameVars.Vertical;
    overcast = g_GameVars.overcast;
    fog = g_GameVars.fog;
    rain = g_GameVars.rain;
    snowfall = g_GameVars.snowfall;
    //streamproof = g_GameVars.streamproof;
    //
    //// Crosshair settings
    //crosshairSize = g_GameVars.crosshairSize;
    //crosshairGap = g_GameVars.crosshairGap;
    //crosshairThickness = g_GameVars.crosshairThickness;
    //EnableCrossRGB = g_GameVars.EnableCrossRGB;
    //crosshairColor = ImVec4(g_GameVars.crosshairColor[0], g_GameVars.crosshairColor[1], g_GameVars.crosshairColor[2], g_GameVars.crosshairColor[3]);
    //
    //// ESP Colors
    //playerESPColor = ImVec4(g_GameVars.playerESPColor[0], g_GameVars.playerESPColor[1], g_GameVars.playerESPColor[2], g_GameVars.playerESPColor[3]);
    //infectedESPColor = ImVec4(g_GameVars.infectedESPColor[0], g_GameVars.infectedESPColor[1], g_GameVars.infectedESPColor[2], g_GameVars.infectedESPColor[3]);
    //carESPColor = ImVec4(g_GameVars.carESPColor[0], g_GameVars.carESPColor[1], g_GameVars.carESPColor[2], g_GameVars.carESPColor[3]);
    //animalESPColor = ImVec4(g_GameVars.animalESPColor[0], g_GameVars.animalESPColor[1], g_GameVars.animalESPColor[2], g_GameVars.animalESPColor[3]);
    //TesTESP[0] = g_GameVars.TesTESP[0];
    //TesTESP[1] = g_GameVars.TesTESP[1];
    //TesTESP[2] = g_GameVars.TesTESP[2];
    //TesTESP[3] = g_GameVars.TesTESP[3];
}

void SaveSettings(const char* filename) {
    // Atualizar GameVars com valores atuais
    g_GameVars.playerESPDistance = playerESPDistance;
    g_GameVars.infectedESPDistance = infectedESPDistance;
    g_GameVars.carESPDistance = carESPDistance;
    g_GameVars.animalESPDistance = animalESPDistance;
    g_GameVars.maxloot = maxloot;
    g_GameVars.LimiteDoSilent = LimiteDoSilent;
    g_GameVars.enablePlayerESP = enablePlayerESP;
    g_GameVars.enableInfectedESP = enableInfectedESP;
    g_GameVars.enableCarESP = enableCarESP;
    g_GameVars.enableInventoryESP = enableInventoryESP;
    g_GameVars.enableAnimalESP = enableAnimalESP;
    g_GameVars.PlayeEsp = PlayeEsp;
    g_GameVars.Playerbox = Playerbox;
    g_GameVars.Playername = Playername;
    g_GameVars.playerDistance = playerDistance;
    g_GameVars.PlayerSkeleton = PlayerSkeleton;
    g_GameVars.espline = espline;
    g_GameVars.carespline = carespline;
    g_GameVars.ActiveSilentAim = ActiveSilentAim;
    g_GameVars.aimfov = aimfov;
    g_GameVars.fovThickness = fovThickness;
    g_GameVars.ToggleFov = ToggleFov;
    g_GameVars.EnableFovRGB = EnableFovRGB;
    g_GameVars.AmmoSpeedChange = AmmoSpeedChange;
    g_GameVars.bonechoose = bonechoose;
    g_GameVars.Iemtelist = Iemtelist;
    g_GameVars.weapon = weapon;
    g_GameVars.comida = comida;
    g_GameVars.ProxyMagazines = ProxyMagazines;
    g_GameVars.Containers = Containers;
    g_GameVars.Miras = Miras;
    g_GameVars.drinks = drinks;
    g_GameVars.rodas = rodas;
    g_GameVars.BackPack = BackPack;
    g_GameVars.inventoryItem = inventoryItem;
    g_GameVars.medicine = medicine;
    g_GameVars.clothing = clothing;
    g_GameVars.ToggleCross = ToggleCross;
    g_GameVars.removeGrass = removeGrass;
    g_GameVars.Claridade = Claridade;
    g_GameVars.eyeAccomValue = eyeAccomValue;
    g_GameVars.Horizontal = Horizontal;
    g_GameVars.Vertical = Vertical;
    g_GameVars.overcast = overcast;
    g_GameVars.fog = fog;
    g_GameVars.rain = rain;
    g_GameVars.snowfall = snowfall;
    //g_GameVars.streamproof = streamproof;
    //g_GameVars.espzDistance = playerESPDistance;
    //
    // Crosshair settings
    g_GameVars.crosshairSize = crosshairSize;
    g_GameVars.crosshairGap = crosshairGap;
    g_GameVars.crosshairThickness = crosshairThickness;
    //g_GameVars.EnableCrossRGB = EnableCrossRGB;
    //g_GameVars.crosshairColor[0] = crosshairColor.x;
    //g_GameVars.crosshairColor[1] = crosshairColor.y;
    //g_GameVars.crosshairColor[2] = crosshairColor.z;
    //g_GameVars.crosshairColor[3] = crosshairColor.w;
    //
    //// ESP Colors
    //g_GameVars.playerESPColor[0] = playerESPColor.x;
    //g_GameVars.playerESPColor[1] = playerESPColor.y;
    //g_GameVars.playerESPColor[2] = playerESPColor.z;
    //g_GameVars.playerESPColor[3] = playerESPColor.w;
    //g_GameVars.infectedESPColor[0] = infectedESPColor.x;
    //g_GameVars.infectedESPColor[1] = infectedESPColor.y;
    //g_GameVars.infectedESPColor[2] = infectedESPColor.z;
    //g_GameVars.infectedESPColor[3] = infectedESPColor.w;
    //g_GameVars.carESPColor[0] = carESPColor.x;
    //g_GameVars.carESPColor[1] = carESPColor.y;
    //g_GameVars.carESPColor[2] = carESPColor.z;
    //g_GameVars.carESPColor[3] = carESPColor.w;
    //g_GameVars.animalESPColor[0] = animalESPColor.x;
    //g_GameVars.animalESPColor[1] = animalESPColor.y;
    //g_GameVars.animalESPColor[2] = animalESPColor.z;
    //g_GameVars.animalESPColor[3] = animalESPColor.w;
    //g_GameVars.TesTESP[0] = TesTESP[0];
    //g_GameVars.TesTESP[1] = TesTESP[1];
    //g_GameVars.TesTESP[2] = TesTESP[2];
    //g_GameVars.TesTESP[3] = TesTESP[3];

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    // Salvar todas as configurações
    file.write(reinterpret_cast<const char*>(&g_GameVars.playerESPDistance), sizeof(int));
    file.write(reinterpret_cast<const char*>(&g_GameVars.infectedESPDistance), sizeof(int));
    file.write(reinterpret_cast<const char*>(&g_GameVars.carESPDistance), sizeof(int));
    file.write(reinterpret_cast<const char*>(&g_GameVars.animalESPDistance), sizeof(int));
    file.write(reinterpret_cast<const char*>(&g_GameVars.maxloot), sizeof(int));
    file.write(reinterpret_cast<const char*>(&g_GameVars.LimiteDoSilent), sizeof(int));
    file.write(reinterpret_cast<const char*>(&g_GameVars.enablePlayerESP), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.enableInfectedESP), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.enableCarESP), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.enableInventoryESP), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.enableAnimalESP), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.PlayeEsp), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.Playerbox), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.Playername), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.playerDistance), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.PlayerSkeleton), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.espline), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.carespline), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.ActiveSilentAim), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.aimfov), sizeof(float));
    file.write(reinterpret_cast<const char*>(&g_GameVars.fovThickness), sizeof(float));
    file.write(reinterpret_cast<const char*>(&g_GameVars.ToggleFov), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.EnableFovRGB), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.AmmoSpeedChange), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.bonechoose), sizeof(int));
    file.write(reinterpret_cast<const char*>(&g_GameVars.Iemtelist), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.weapon), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.comida), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.ProxyMagazines), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.Containers), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.Miras), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.drinks), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.rodas), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.BackPack), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.inventoryItem), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.medicine), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.clothing), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.ToggleCross), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.removeGrass), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&g_GameVars.Claridade), sizeof(float));
    file.write(reinterpret_cast<const char*>(&g_GameVars.eyeAccomValue), sizeof(float));
    file.write(reinterpret_cast<const char*>(&g_GameVars.Horizontal), sizeof(float));
    file.write(reinterpret_cast<const char*>(&g_GameVars.Vertical), sizeof(float));
    file.write(reinterpret_cast<const char*>(&g_GameVars.overcast), sizeof(float));
    file.write(reinterpret_cast<const char*>(&g_GameVars.fog), sizeof(float));
    file.write(reinterpret_cast<const char*>(&g_GameVars.rain), sizeof(float));
    file.write(reinterpret_cast<const char*>(&g_GameVars.snowfall), sizeof(float));
    //file.write(reinterpret_cast<const char*>(&g_GameVars.streamproof), sizeof(bool));
    //file.write(reinterpret_cast<const char*>(&g_GameVars.espzDistance), sizeof(int));
    //
    // Crosshair settings
    file.write(reinterpret_cast<const char*>(&g_GameVars.crosshairSize), sizeof(float));
    file.write(reinterpret_cast<const char*>(&g_GameVars.crosshairGap), sizeof(float));
    file.write(reinterpret_cast<const char*>(&g_GameVars.crosshairThickness), sizeof(float));
    //file.write(reinterpret_cast<const char*>(&g_GameVars.EnableCrossRGB), sizeof(bool));
    //file.write(reinterpret_cast<const char*>(&g_GameVars.crosshairColor), sizeof(float) * 4);
    //
    //// ESP Colors
    //file.write(reinterpret_cast<const char*>(&g_GameVars.playerESPColor), sizeof(float) * 4);
    //file.write(reinterpret_cast<const char*>(&g_GameVars.infectedESPColor), sizeof(float) * 4);
    //file.write(reinterpret_cast<const char*>(&g_GameVars.carESPColor), sizeof(float) * 4);
    //file.write(reinterpret_cast<const char*>(&g_GameVars.animalESPColor), sizeof(float) * 4);
    //file.write(reinterpret_cast<const char*>(&g_GameVars.TesTESP), sizeof(float) * 4);

    file.close();
}

void SaveConfig() {
    const std::string configDir = "C:\\SaturnDayZ\\";
    const std::string configPath = configDir + "config.dat";

    EnsureDirectoryExists(configDir);
    SaveSettings(configPath.c_str());
}

void LoadConfig() {
    const std::string configPath = "C:\\SaturnDayZ\\config.dat";

    LoadSettings(configPath.c_str());
}

void UpdateConfigList(const std::string& configDir) {
    configFiles.clear();
    try {
        for (const auto& entry : std::filesystem::directory_iterator(configDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".dat") {
                configFiles.push_back(entry.path().filename().string());
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error updating config list: " << e.what() << std::endl;
    }
}

void SaveNewConfig(const std::string& configDir, const std::string& configName) {
    if (configName.empty()) {
        std::cerr << "Error: Config name cannot be empty." << std::endl;
        return;
    }

    const std::string filePath = configDir + configName + ".dat";
    SaveSettings(filePath.c_str());
    UpdateConfigList(configDir);
}

void ImGui_menu() {
    SanitizeValues();
    
    // Processar load config request
    //if (g_GameVars.loadConfigRequested) {
    //    LoadConfig();
    //    g_GameVars.loadConfigRequested = false;
    //}

    auto s = ImVec2{}, p = ImVec2{}, gs = ImVec2(620, 448);
    ImGui::SetNextWindowSize(ImVec2(gs));
    ImGui::Begin("##GUI", NULL, ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground);
    {
        {//draw
            s = ImVec2(ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2, ImGui::GetWindowSize().y - ImGui::GetStyle().WindowPadding.y * 2);
            p = ImVec2(ImGui::GetWindowPos().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetWindowPos().y + ImGui::GetStyle().WindowPadding.y);
            auto draw = ImGui::GetWindowDrawList();
            draw->AddRectFilled(p, ImVec2(p.x + s.x, p.y + s.y), ImColor(33, 33, 33)); //tabs
            draw->AddRectFilled(ImVec2(p.x, p.y + 25), ImVec2(p.x + s.x, p.y + s.y - 25), ImColor(25, 25, 25));

            draw->AddLine(ImVec2(p.x, p.y + 25), ImVec2(p.x + s.x, p.y + 25), ImColor(89, 113, 162)); // tab seperator

            draw->AddLine(ImVec2(p.x, p.y + s.y - 25), ImVec2(p.x + s.x, p.y + s.y - 25), ImColor(89, 113, 162)); // bottom seperator
        }
        {//tabs
            ImGui::SetCursorPosX(1);
            ImGui::SetCursorPosY(1);
            ImGui::BeginGroup();
            if (ImGui::tab("Aimbot", Variables::ActiveTab == 0)) Variables::ActiveTab = 0; ImGui::SameLine();
            if (ImGui::tab("Visuals", Variables::ActiveTab == 2)) Variables::ActiveTab = 2; ImGui::SameLine();
            if (ImGui::tab("Misc", Variables::ActiveTab == 1)) Variables::ActiveTab = 1; ImGui::SameLine();
            if (ImGui::tab("Crosshair", Variables::ActiveTab == 3)) Variables::ActiveTab = 3; ImGui::SameLine();
            if (ImGui::tab("Config", Variables::ActiveTab == 4)) Variables::ActiveTab = 4;
            ImGui::EndGroup();
        }
        {//content
            // Aimbot Tab
            if (Variables::ActiveTab == 0) {
                ImGui::SetCursorPosY(35);
                ImGui::BeginGroup();
                ImGui::SetCursorPosX(15);
                ImGui::MenuChild("General", ImVec2(300, 205), false);
                {
                    ImGui::Checkbox("Enable Silent Aim", &ActiveSilentAim);
                    ImGui::Checkbox("Show Fov", &ToggleFov);
                    ImGui::Checkbox("FOV RGB", &EnableFovRGB);
                    ImGui::Checkbox("Speed", &AmmoSpeedChange);

                    if (ToggleFov) {
                        ImGui::SliderFloat("FOV Size", &aimfov, 0.0f, 300.0f, "%.1f");
                        ImGui::SliderFloat("FOV Thickness", &fovThickness, 1.0f, 10.0f, "%.1f");
                    }

                    ImGui::SliderInt("MAX Distance", &LimiteDoSilent, 0, 1200);
                }
                ImGui::EndChild();
                ImGui::EndGroup();

                ImGui::SetCursorPosY(240);
                ImGui::BeginGroup();
                ImGui::SetCursorPosX(15);
                ImGui::MenuChild("Other", ImVec2(300, 180), false);
                {
                    const char* bones[] = { "Head", "Neck", "Chest", "Pelvis", "Left Leg", "Right Leg" };
                    ImGui::Combo("Target Bone:", &bonechoose, bones, IM_ARRAYSIZE(bones));

                    ImGui::Text("Silent Key: ");
                    ImGui::SameLine();
                    ImGuiCustom::HotkeyButtonEx("Select Silent Key", &desynckey, { 57.f, 22.f }, 0);
                    ImGui::ColorEdit4("Fov color", (float*)&fovcolorcc);
                }
                ImGui::EndChild();
                ImGui::EndGroup();

                ImGui::SetCursorPosY(35);
                ImGui::BeginGroup();
                ImGui::SetCursorPosX(320);
                ImGui::MenuChild("Settings", ImVec2(285, 385), false);
                {
                    // Additional settings can go here
                }
                ImGui::EndChild();
                ImGui::EndGroup();
            }

            // Misc Tab
            if (Variables::ActiveTab == 1) {
                ImGui::SetCursorPosY(35);
                ImGui::BeginGroup();
                ImGui::SetCursorPosX(15);
                ImGui::MenuChild("Ambiente", ImVec2(300, 205), false);
                {
                    if (ImGui::SliderFloat("Claridade", &Claridade, 1.0f, 90.0f))
                        Game::SetClariddade(Claridade);
                    if (ImGui::SliderFloat("Eye Accom", &eyeAccomValue, 5.0f, 15.0f))
                        Game::SetEyeAccom(eyeAccomValue);
                    ImGui::Checkbox("Clima Perfeito", &PerfectWeather);
                    ImGui::SliderFloat("Aspect Ratio", &Horizontal, 0.1f, 5.0f, "%.2f");
                    ImGui::SliderFloat("Camera FOV", &Vertical, -1.f, 10.f, "%.1f");
                }
                ImGui::EndChild();
                ImGui::EndGroup();

                ImGui::SetCursorPosY(240);
                ImGui::BeginGroup();
                ImGui::SetCursorPosX(15);
                ImGui::MenuChild("Clima", ImVec2(300, 180), false);
                {
                    ImGui::SliderFloat("Overcast", &overcast, 0.0f, 100);
                    ImGui::SliderFloat("Fog", &fog, 0.0f, 100);
                    ImGui::SliderFloat("Rain", &rain, 0.0f, 100);
                    ImGui::SliderFloat("Snowfall", &snowfall, 0.0f, 100);
                }
                ImGui::EndChild();
                ImGui::EndGroup();

                ImGui::SetCursorPosY(35);
                ImGui::BeginGroup();
                ImGui::SetCursorPosX(320);
                ImGui::MenuChild("Extras", ImVec2(285, 385), false);
                {
                    ImGui::Checkbox("Remover Grama", &removeGrass);
                    ImGui::Checkbox("Streamproof", &streamproof);
                    if (globals.OverlayWnd) {
                        SetWindowDisplayAffinity(globals.OverlayWnd, streamproof ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
                    }
                    ImGui::Text("Teclas");
                    ImGuiCustom::HotkeyButtonEx("Key TP", &LootTP, ImVec2(50, 50), 0);
                    ImGui::Checkbox("Loot Teleport", &LootTeleport);
                    ImGuiCustom::HotkeyButtonEx("Key Menu", &menuKey, ImVec2(60, 22), 0);
                }
                ImGui::EndChild();
                ImGui::EndGroup();
            }

            // Crosshair Tab
            if (Variables::ActiveTab == 3) {
                ImGui::SetCursorPosY(35);
                ImGui::BeginGroup();
                ImGui::SetCursorPosX(15);
                ImGui::MenuChild("Crosshair Settings", ImVec2(590, 410), false);
                {
                    ImGui::Checkbox("Enable Crosshair", &ToggleCross);
                    ImGui::Checkbox("Enable Crosshair RGB", &EnableCrossRGB);

                    if (ToggleCross) {
                        ImGui::SliderFloat("Size", &crosshairSize, 2.0f, 30.0f, "%.1f");
                        ImGui::SliderFloat("Gap", &crosshairGap, 0.0f, 10.0f, "%.1f");
                        ImGui::SliderFloat("Thickness", &crosshairThickness, 1.0f, 5.0f, "%.1f");
                        ImGui::ColorEdit4("Crosshair Color", (float*)&crosshairColor);
                    }
                }
                ImGui::EndChild();
                ImGui::EndGroup();
            }

            // Visuals Tab
            if (Variables::ActiveTab == 2) {
                ImGui::SetCursorPosY(35);
                ImGui::BeginGroup();
                ImGui::SetCursorPosX(15);
                ImGui::MenuChild("Player ESP", ImVec2(300, 250), false);
                {
                    ImGui::Checkbox("Enable Player ESP", &PlayeEsp);
                    ImGui::Checkbox("ESP Skeleton", &PlayerSkeleton);
                    ImGui::Checkbox("ESP Box", &Playerbox);
                    ImGui::Checkbox("ESP Line", &espline);
                    ImGui::Checkbox("ESP Name", &Playername);
                    ImGui::Checkbox("ESP Distance", &playerDistance);
                    ImGui::SliderInt("Max Distance", &playerESPDistance, 0, 1500);

                    ImGui::Separator();
                    ImGui::Checkbox("Enable Inventory ESP", &enableInventoryESP);
                    if (enableInventoryESP) {
                        ImGui::SliderFloat("Inventory FOV", &inventoryESPFOV, 5.0f, 100.0f, "%.1f");
                    }
                }
                ImGui::EndChild();
                ImGui::EndGroup();

                ImGui::SetCursorPosY(240);
                ImGui::BeginGroup();
                ImGui::SetCursorPosX(15);
                ImGui::MenuChild("Other ESP", ImVec2(300, 180), false);
                {
                    ImGui::Checkbox("Enable Zombie ESP", &enableInfectedESP);
                    if (enableInfectedESP) {
                        ImGui::Checkbox("ESP Skeleton", &zombieskeleton);
                        ImGui::Checkbox("ESP Line", &espline2);
                        ImGui::SliderInt("MAX Distance", &infectedESPDistance, 0, 1200);
                    }
                    ImGui::Checkbox("ESP Car", &Car_Esp);
                    ImGui::Checkbox("Line", &carespline);
                    ImGui::SliderInt("Max Distance", &carESPDistance, 0, 1200);
                    ImGui::Checkbox("Enable Animal ESP", &enableAnimalESP);
                    ImGui::SliderInt("Max Distance", &animalESPDistance, 0, 1200);
                }
                ImGui::EndChild();
                ImGui::EndGroup();

                ImGui::SetCursorPosY(35);
                ImGui::BeginGroup();
                ImGui::SetCursorPosX(320);
                ImGui::MenuChild("Loot ESP", ImVec2(285, 385), false);
                {
                    ImGui::SliderInt("MAX Distance", &maxloot, 0, 1200);
                    ImGui::Checkbox("Enable Loot ESP", &Iemtelist);
                    ImGui::Separator();
                    ImGui::Text("Loot Categories:");
                    ImGui::Checkbox("ESP WEAPON", &weapon);
                    ImGui::Checkbox("ESP FOOD", &comida);
                    ImGui::Checkbox("ESP AMMO", &ProxyMagazines);
                    ImGui::Checkbox("ESP CONTAINERS", &Containers);
                    ImGui::Checkbox("ESP OPTICS", &Miras);
                    ImGui::Checkbox("ESP DRINKS", &drinks);
                    ImGui::Checkbox("ESP CAR WHEELS", &rodas);
                    ImGui::Checkbox("ESP BACKPACKS", &BackPack);
                    ImGui::Checkbox("ESP OTHERS", &inventoryItem);
                    ImGui::Checkbox("ESP MEDICINES", &medicine);
                    ImGui::Checkbox("ESP CLOTHING", &clothing);
                    ImGui::Separator();
                    ImGui::Text("Loot Colors:");
                    ImGui::ColorEdit4("Weapon", (float*)&colorWeapon, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
                    ImGui::ColorEdit4("Food", (float*)&colorFood, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
                    ImGui::ColorEdit4("Ammo", (float*)&colorAmmo, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
                    ImGui::ColorEdit4("Containers", (float*)&colorContainers, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
                    ImGui::ColorEdit4("Optics", (float*)&colorOptics, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
                    ImGui::ColorEdit4("Drinks", (float*)&colorDrinks, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
                    ImGui::ColorEdit4("Backpacks", (float*)&colorBackpacks, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
                    ImGui::ColorEdit4("Others", (float*)&colorOthers, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
                    ImGui::ColorEdit4("Medicines", (float*)&colorMedicines, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
                    ImGui::ColorEdit4("Clothing", (float*)&colorClothing, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs);
                }
                ImGui::EndChild();
                ImGui::EndGroup();
            }

            // Config Tab
            if (Variables::ActiveTab == 4) {
                const std::string configDir = "C:\\SaturnDayZ\\";
                EnsureDirectoryExists(configDir);
                static bool saveConfigOpen = false;

                // Atualiza a lista de configurações quando a interface é aberta
                if (configFiles.empty()) {
                    UpdateConfigList(configDir);
                }

                ImGui::SetCursorPosY(35);
                ImGui::BeginGroup();
                ImGui::SetCursorPosX(15);
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(13.0f / 255.0f, 13.0f / 255.0f, 13.0f / 255.0f, 240.0f / 255.0f));
                ImGui::BeginChild("##container", ImVec2(230, 275), false, ImGuiWindowFlags_NoScrollbar);
                {
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.6f), "Configs");
                    ImGui::Separator();
                    ImGui::Spacing();

                    for (const auto& file : configFiles) {
                        if (ImGui::Selectable(file.c_str())) {
                            LoadSettings((configDir + file).c_str());
                        }
                    }
                }
                ImGui::EndChild();
                ImGui::PopStyleColor();

                ImGui::SetCursorPosX(260);
                ImGui::SetCursorPosY(35);
                ImGui::BeginGroup();
                ImGui::MenuChild("Config Manager", ImVec2(320, 410), false);
                {
                    if (ImGui::Button("Load Config", ImVec2(140, 30))) {
                        //g_GameVars.loadConfigRequested = true;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Save Config", ImVec2(140, 30))) {
                        SaveConfig();
                    }

                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();

                    ImGui::Text("Create New Config:");
                    static char configNameBuffer[128] = "";
                    ImGui::InputText("Config Name", configNameBuffer, sizeof(configNameBuffer));
                    
                    if (ImGui::Button("Save New Config", ImVec2(280, 30))) {
                        if (strlen(configNameBuffer) > 0) {
                            SaveNewConfig(configDir, std::string(configNameBuffer));
                            memset(configNameBuffer, 0, sizeof(configNameBuffer));
                        }
                    }

                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();

                    ImGui::Text("Current Config: config.dat");
                    ImGui::Text("Config Directory: C:\\SaturnDayZ\\");
                }
                ImGui::EndChild();
                ImGui::EndGroup();
            }
        }
    }
    ImGui::End();
}


void DrawESPMenu() {
    if (!showESPMenu) return;

    if (!ImGui::GetCurrentContext()) {
        showESPMenu = false;
        return;
    }

    try {
        ImGui_menu();
    }
    catch (const std::exception& e) {
        showESPMenu = false;
    }
    catch (...) {
        showESPMenu = false;
    }
}

void toggleESPMenu() {
    if (GetAsyncKeyState(menuKey) & 1) {
        showESPMenu = !showESPMenu;

        if (!globals.OverlayWnd) return;

        try {
            if (showESPMenu) {
                LONG exStyle = GetWindowLong(globals.OverlayWnd, GWL_EXSTYLE);
                exStyle &= ~WS_EX_TRANSPARENT;
                exStyle |= WS_EX_TOOLWINDOW;
                SetWindowLong(globals.OverlayWnd, GWL_EXSTYLE, exStyle);
            }
            else {
                LONG exStyle = GetWindowLong(globals.OverlayWnd, GWL_EXSTYLE);
                exStyle |= WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW;
                SetWindowLong(globals.OverlayWnd, GWL_EXSTYLE, exStyle);
            }
        }
        catch (...) {
        }
    }
}
struct DeadBody {
    uint64_t entityPtr;
    Vector3 position;
    std::string playerName;
};
std::vector<DeadBody> deadBodies;

uint32_t GetPlayersOnlineCount() {
    uint64_t network_client = read<uint64_t>(globals.Base + NetworkOffsets::off_networkmanager + NetworkOffsets::off_networkmanager_networkclient);
    if (!network_client) return 0;

    return read<uint32_t>(network_client + 0x24);
}

std::string get_server_name() {
    uint64_t network_client = read<uint64_t>(globals.Base + NetworkOffsets::off_networkmanager + NetworkOffsets::off_networkmanager_networkclient);
    if (!network_client) return "Unknown";

    uint64_t server_name_ptr = read<uint64_t>(network_client + 0x340);
    if (!server_name_ptr) return "Unknown";

    return ReadArmaString(server_name_ptr);
}

std::unordered_map<uintptr_t, Vector3> last_bullet_positions;
struct trail_segment_t {
    Vector3 from;
    Vector3 to;
    float time_created;
};

std::map<uintptr_t, Vector3> last_bullet_pos;
std::map<uintptr_t, Vector3> bullet_last_known_pos;
std::vector<trail_segment_t> fading_trails;


void DrawFovCircle(ImDrawList* drawList, const ImVec2& center, float radius)
{
    ImU32 color;

    if (EnableFovRGB)
    {
        float time = ImGui::GetTime();
        int r = (int)((sinf(time * 2.0f) * 0.5f + 0.5f) * 255);
        int g = (int)((sinf(time * 2.0f + 2.0f) * 0.5f + 0.5f) * 255);
        int b = (int)((sinf(time * 2.0f + 4.0f) * 0.5f + 0.5f) * 255);
        color = IM_COL32(r, g, b, 255);
    }
    else
    {
        color = ImColor(fovcolorcc);
    }

    drawList->AddCircle(
        center,
        radius,
        color,
        100,          // segmentos (quanto maior, mais redondo)
        fovThickness  // espessura
    );
}

void DrawBulletTracers() {
    const uintptr_t world = read<uintptr_t>(globals.Base + WorldOffsets::OFF_World);
    if (!world) return;

    const uintptr_t bulletTable = read<uintptr_t>(world + MiscOffsets::OFF_Bullets);
    if (!bulletTable) return;

    const int bulletCount = read<int>(world + MiscOffsets::OFF_Bulletsize);
    if (bulletCount <= 0 || bulletCount > 500) return;

    ImDrawList* drawList = ImGui::GetOverlayDrawList();
    const float timeNow = ImGui::GetTime();
    const float trailLifetime = 1.1f;

    for (int i = 0; i < bulletCount; ++i) {
        const uintptr_t bullet = read<uintptr_t>(bulletTable + i * sizeof(uintptr_t));
        if (!bullet) continue;

        const uintptr_t visPtr = read<uintptr_t>(bullet + EntityOffsets::OFF_VisualState);
        if (!visPtr) continue;

        const Vector3 bulletPos = read<Vector3>(visPtr + EntityOffsets::OFF_GetCoordinate);

        if (last_bullet_pos.count(bullet)) {
            fading_trails.push_back({
                last_bullet_pos[bullet],
                bulletPos,
                timeNow
                });
        }

        last_bullet_pos[bullet] = bulletPos;
    }

    fading_trails.erase(
        std::remove_if(fading_trails.begin(), fading_trails.end(),
            [&](const trail_segment_t& seg) {
                return (timeNow - seg.time_created) > trailLifetime;
            }),
        fading_trails.end()
    );

    for (const auto& seg : fading_trails) {
        const float age = timeNow - seg.time_created;
        const float alpha = std::clamp(1.0f - (age / trailLifetime), 0.0f, 1.0f);

        Vector3 s1, s2;
        if (Game::WorldToScreen(seg.from, s1) && Game::WorldToScreen(seg.to, s2)) {
            drawList->AddLine(
                ImVec2(s1.x, s1.y),
                ImVec2(s2.x, s2.y),
                ImColor(1.f, 0.f, 0.f, alpha),
                1.f * alpha
            );
        }
    }
}

float ClampAngle(float angle) {
    while (angle < 0) angle += 360.f;
    while (angle >= 360.f) angle -= 360.f;
    return angle;
}

float GetAngleToEntity(Vector3 localPos, Vector3 targetPos, float cameraYaw) {
    float deltaX = targetPos.x - localPos.x;
    float deltaZ = targetPos.z - localPos.z;
    float angle = atan2f(deltaZ, deltaX) * (180.0f / 3.14159265f);
    angle = ClampAngle(angle - cameraYaw);
    return angle;
}

bool IsOutOfView(Vector3 screenPos, float screenWidth, float screenHeight) {
    return screenPos.x < 0 || screenPos.x > screenWidth || screenPos.y < 0 || screenPos.y > screenHeight;
}

void DrawOutOfViewArrow(Vector3 localPos, Vector3 targetPos, float cameraYaw, ImColor color) {
    ImGuiIO& io = ImGui::GetIO();
    float angle = GetAngleToEntity(localPos, targetPos, cameraYaw);
    float radius = 230.0f;
    float screenCenterX = io.DisplaySize.x / 2;
    float screenCenterY = io.DisplaySize.y / 2;

    float rad = angle * (3.14159265f / 180.0f);
    float x = screenCenterX + cosf(rad) * radius;
    float y = screenCenterY + sinf(rad) * radius;

    ImDrawList* drawList = ImGui::GetOverlayDrawList();
    drawList->AddTriangleFilled(
        ImVec2(x, y),
        ImVec2(x - 10 * cosf(rad - 0.5f), y - 10 * sinf(rad - 0.5f)),
        ImVec2(x - 10 * cosf(rad + 0.5f), y - 10 * sinf(rad + 0.5f)),
        color
    );
}

void DrawAnimalESP(uint64_t entityPtr, Vector3 screenPos, const std::string& name, int distance) {
    ImVec4 animalColor = animalESPColor;
    DrawEntityInfo(entityPtr, screenPos, "[" + std::to_string(distance) + "m] - " + name, animalColor);
}

void DrawZombieESP() {
    uint64_t localPlayer = Game::GetLocalPlayer();
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* drawList = ImGui::GetOverlayDrawList();

    const ImVec4 baseColor = infectedESPColor;
    const ImVec4 lineColor = ImVec4(TesTESP[0], TesTESP[1], TesTESP[2], TesTESP[3]);
    
    // Obter posição do local player uma vez (ou usar alternativa se morto)
    Vector3 zombieLocalPos(0, 0, 0);
    bool zombieHasValidLocalPos = false;
    if (localPlayer && Game::IsValidPtr((void*)localPlayer)) {
        zombieLocalPos = Game::GetCoordinate(localPlayer);
        zombieHasValidLocalPos = !zombieLocalPos.isZ0();
    }

    for (const auto& entity : entities) {
        if (entity.EntityPtr == localPlayer && hideLocalPlayer)
            continue;

        if (!enableInfectedESP)
            continue;

        std::string entityType = Game::GetEntityTypeName(entity.EntityPtr);
        if (entityType != "dayzinfected")
            continue;

        Vector3 worldPos = Game::GetCoordinate(entity.EntityPtr);
        
        // Calcular distância de forma segura mesmo quando jogador morre
        int distance = 0;
        if (zombieHasValidLocalPos) {
            distance = static_cast<int>(worldPos.Distance(zombieLocalPos));
        } else {
            // Se não temos posição válida do local player, usar GetDistanceToMe mas com fallback
            try {
                distance = Game::GetDistanceToMe(worldPos);
            } catch (...) {
                // Se falhar, usar distância baseada na tela como fallback
                Vector3 screenPos;
                if (Game::WorldToScreen(worldPos, screenPos)) {
                    ImVec2 screenCenter(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
                    float screenDist = sqrtf((screenPos.x - screenCenter.x) * (screenPos.x - screenCenter.x) + 
                                             (screenPos.y - screenCenter.y) * (screenPos.y - screenCenter.y));
                    distance = static_cast<int>(screenDist * 0.1f); // Aproximação grosseira
                } else {
                    continue; // Se não conseguir converter para tela, pular
                }
            }
        }
        
        if (distance > infectedESPDistance)
            continue;

        Vector3 screenPos;
        if (!Game::WorldToScreen(worldPos, screenPos) || screenPos.z < 1.0f)
            continue;

        float alpha = std::clamp(1.0f - (distance / (float)infectedESPDistance), 0.25f, 1.0f);
        ImVec4 fadedColor = ImVec4(baseColor.x, baseColor.y, baseColor.z, alpha);

        if (zombieskeleton)
            DrawZombieSkeleton(entity.EntityPtr, fadedColor, 1.0f);

        std::string label = "Infected [" + std::to_string(distance) + "m]";
        ImVec2 textSize = ImGui::CalcTextSize(label.c_str());
        ImVec2 textPos = ImVec2(screenPos.x - textSize.x / 2, screenPos.y - 25.0f);

        drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 200), label.c_str());
        drawList->AddText(textPos, ImGui::ColorConvertFloat4ToU32(fadedColor), label.c_str());

        ZombieBoxInfo(entity.EntityPtr, fadedColor);

        if (espline2) {
            drawList->AddLine(
                ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y),
                ImVec2(screenPos.x, screenPos.y),
                ImGui::ColorConvertFloat4ToU32(lineColor),
                1.5f
            );
        }
    }
}

const char* check_player_status(uint64_t entityPtr) {
    if (Game::is_dead(entityPtr)) {
        playerESPColor = ImVec4(255, 255, 255, 255);
        Vector3 screenPos;
        Vector3 WorldPos = Game::GetCoordinate(entityPtr);

        if (Game::WorldToScreen(WorldPos, screenPos)) {
            DrawStringA(15.f, (int)screenPos.x, (int)screenPos.y, &Col.white, false, true, "");
        }

        return "";
    }
    return "alive";
}

void DrawPlayerESP(uintptr_t entityPtr, int distance, const Vector3& screenPos) {
    if (screenPos.z <= 0.01f)
        return;
    ImVec4 color = playerESPColor;

    if (std::find(valid_players.begin(), valid_players.end(), (uintptr_t*)entityPtr) == valid_players.end())
        valid_players.push_back((uintptr_t*)entityPtr);

    if (std::find(valid_players1.begin(), valid_players1.end(), (uintptr_t*)entityPtr) == valid_players1.end())
        valid_players1.push_back((uintptr_t*)entityPtr);

    check_player_status(entityPtr);

    if (PlayerSkeleton)
        DrawSkeleton(entityPtr, color, 1);

    BoxInfo(entityPtr, color, get_player_name(entityPtr), distance);

    if (espline) {
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 startPos(io.DisplaySize.x * 0.5f, io.DisplaySize.y);
        ImVec2 endPos(screenPos.x, screenPos.y);

        ImGui::GetOverlayDrawList()->AddLine(
            startPos,
            endPos,
            ImGui::ColorConvertFloat4ToU32(color),
            1.0f
        );
    }
}

void DrawCrosshair(ImDrawList* drawList, const ImVec2& center, float size, float gap, float thickness, ImVec4 color, bool enableRGB) {
    float outlineThickness = thickness + 2.0f;
    ImU32 outlineColor = IM_COL32(0, 0, 0, 255);

    ImVec2 left1(center.x - gap - size, center.y);
    ImVec2 left2(center.x - gap, center.y);
    ImVec2 right1(center.x + gap, center.y);
    ImVec2 right2(center.x + gap + size, center.y);
    ImVec2 top1(center.x, center.y - gap - size);
    ImVec2 top2(center.x, center.y - gap);
    ImVec2 bot1(center.x, center.y + gap);
    ImVec2 bot2(center.x, center.y + gap + size);

    drawList->AddLine(left1, left2, outlineColor, outlineThickness);
    drawList->AddLine(right1, right2, outlineColor, outlineThickness);
    drawList->AddLine(top1, top2, outlineColor, outlineThickness);
    drawList->AddLine(bot1, bot2, outlineColor, outlineThickness);

    ImU32 innerColor;
    if (enableRGB) {
        float time = ImGui::GetTime();
        int r = static_cast<int>((sin(time * 2.0f) * 0.5f + 0.5f) * 255);
        int g = static_cast<int>((sin(time * 2.0f + 2.0f) * 0.5f + 0.5f) * 255);
        int b = static_cast<int>((sin(time * 2.0f + 4.0f) * 0.5f + 0.5f) * 255);
        innerColor = IM_COL32(r, g, b, 255);
    }
    else {
        innerColor = ImColor(color);
    }

    drawList->AddLine(left1, left2, innerColor, thickness);
    drawList->AddLine(right1, right2, innerColor, thickness);
    drawList->AddLine(top1, top2, innerColor, thickness);
    drawList->AddLine(bot1, bot2, innerColor, thickness);
}

void draw_esp_car() {
    ImDrawList* drawList = ImGui::GetOverlayDrawList();
    uint64_t localPlayer = Game::GetLocalPlayer();
    
    // Obter posição do local player uma vez (ou usar alternativa se morto)
    Vector3 carLocalPos(0, 0, 0);
    bool carHasValidLocalPos = false;
    if (localPlayer && Game::IsValidPtr((void*)localPlayer)) {
        carLocalPos = Game::GetCoordinate(localPlayer);
        carHasValidLocalPos = !carLocalPos.isZ0();
    }

    for (const auto& entity : entities) {
        if (entity.EntityPtr == localPlayer && hideLocalPlayer)
            continue;

        Vector3 worldPos = Game::GetCoordinate(entity.EntityPtr);
        Vector3 screenPos;
        if (!Game::WorldToScreen(worldPos, screenPos))
            continue;

        if (screenPos.z < 0.01f)
            continue;

        // Calcular distância de forma segura mesmo quando jogador morre
        int distance = 0;
        if (carHasValidLocalPos) {
            distance = static_cast<int>(worldPos.Distance(carLocalPos));
        } else {
            // Se não temos posição válida do local player, usar GetDistanceToMe mas com fallback
            try {
                distance = Game::GetDistanceToMe(worldPos);
            } catch (...) {
                // Se falhar, usar distância baseada na tela como fallback
                ImGuiIO& io = ImGui::GetIO();
                ImVec2 screenCenter(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
                float screenDist = sqrtf((screenPos.x - screenCenter.x) * (screenPos.x - screenCenter.x) + 
                                         (screenPos.y - screenCenter.y) * (screenPos.y - screenCenter.y));
                distance = static_cast<int>(screenDist * 0.1f); // Aproximação grosseira
            }
        }
        
        std::string type = Game::GetEntityTypeName(entity.EntityPtr);
        std::string name = Game::GetEntityRealName(entity.EntityPtr);

        if (type == "car" && enableCarESP && distance <= carESPDistance) {
            ImVec4 carColor = carESPColor;

            DrawStringNoBackground(
                globals.Width / 2,
                globals.Height / 2 - 500,
                &Col.red,
                "There are cars nearby"
            );

            DrawEntityInfo(
                entity.EntityPtr,
                screenPos,
                "[" + std::to_string(distance) + "m] - " + name,
                carColor
            );

            if (carespline) {
                ImVec2 lineStart(950.0f, 2.0f);
                drawList->AddLine(
                    lineStart,
                    ImVec2(screenPos.x, screenPos.y),
                    ImColor(carColor),
                    1.5f
                );
            }
        }
    }
}



void draw_esp() {
    ImDrawList* drawList = ImGui::GetOverlayDrawList();
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 screenCenter(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);

    draw_esp_car();
    DrawCorpses();
    DrawBulletTracers();
    // toggleESPMenu(); // Old menu - disabled
    DrawPlayerInventoryESP();
    DrawZombieESP();
    // DrawESPMenu(); // Old menu - disabled, using DrawMenuUI() in overlay.cpp instead
    DrawStringA(15.f, 10, 1, &Col.white, false, true, "Serve Name: %s", get_server_name().c_str());
    DrawStringA(15.f, 10, 15, &Col.white, false, true, "CherryWave");
    DrawStringA(15.f, 10, 28, &Col.white, false, true, "Players: %d", GetPlayersOnlineCount());

    uint64_t localPlayer = Game::GetLocalPlayer();
    uintptr_t pUWorld = read<uintptr_t>(globals.Base + WorldOffsets::OFF_World);

    if (Iemtelist) IterateItems(pUWorld);
    if (LootTeleport) lootteleport();

    static bool fovToggled = false;
    static float previousFov = 0.0f;

    if (ToggleFov) {
        DrawFovCircle(drawList, screenCenter, aimfov);
    }


    if (ToggleCross) {
        DrawCrosshair(drawList, screenCenter, crosshairSize, crosshairGap, crosshairThickness, crosshairColor, EnableCrossRGB);
    }

    valid_players.clear();
    valid_players1.clear();
    valid_players2.clear();

    // Primeiro, popular valid_players com todos os players válidos
    // Obter posição do local player uma vez (ou usar alternativa se morto)
    Vector3 localPos(0, 0, 0);
    bool hasValidLocalPos = false;
    if (localPlayer && Game::IsValidPtr((void*)localPlayer)) {
        localPos = Game::GetCoordinate(localPlayer);
        hasValidLocalPos = !localPos.isZ0();
    }
    
    for (const auto& entity : entities) {
        if (entity.EntityPtr == localPlayer && hideLocalPlayer) continue;

        Vector3 worldPos = Game::GetCoordinate(entity.EntityPtr);
        Vector3 screenPos;
        if (!Game::WorldToScreen(worldPos, screenPos)) continue;
        if (screenPos.z < 0.01f) continue;

        // Calcular distância de forma segura mesmo quando jogador morre
        int distance = 0;
        if (hasValidLocalPos) {
            distance = static_cast<int>(worldPos.Distance(localPos));
        } else {
            // Se não temos posição válida do local player, usar GetDistanceToMe mas com fallback
            try {
                distance = Game::GetDistanceToMe(worldPos);
            } catch (...) {
                // Se falhar, usar distância baseada na tela como fallback
                float screenDist = sqrtf((screenPos.x - screenCenter.x) * (screenPos.x - screenCenter.x) + 
                                         (screenPos.y - screenCenter.y) * (screenPos.y - screenCenter.y));
                distance = static_cast<int>(screenDist * 0.1f); // Aproximação grosseira
            }
        }
        
        std::string type = Game::GetEntityTypeName(entity.EntityPtr);
        std::string name = Game::GetEntityRealName(entity.EntityPtr);

        // Adicionar players válidos para o silent aim
        if (type == "dayzplayer" && enablePlayerESP && distance <= playerESPDistance) {
            if (std::find(valid_players.begin(), valid_players.end(), (uintptr_t*)entity.EntityPtr) == valid_players.end())
                valid_players.push_back((uintptr_t*)entity.EntityPtr);
        }
    }

    // Agora executar o silent aim com a lista populada
    if (ActiveSilentAim) {
        main_aimboo2(isnpc);
        if (resultant_target_entity) {
            SilentAim1((uint64_t)resultant_target_entity, isnpc);
        }
    }

    // Renderizar ESP (reutilizando localPos e hasValidLocalPos já definidos acima)
    
    for (const auto& entity : entities) {
        if (entity.EntityPtr == localPlayer && hideLocalPlayer) continue;

        Vector3 worldPos = Game::GetCoordinate(entity.EntityPtr);
        Vector3 screenPos;
        if (!Game::WorldToScreen(worldPos, screenPos)) continue;
        if (screenPos.z < 0.01f) continue;

        // Calcular distância de forma segura mesmo quando jogador morre
        int distance = 0;
        if (hasValidLocalPos) {
            distance = static_cast<int>(worldPos.Distance(localPos));
        } else {
            // Se não temos posição válida do local player, usar GetDistanceToMe mas com fallback
            try {
                distance = Game::GetDistanceToMe(worldPos);
            } catch (...) {
                // Se falhar, usar distância baseada na tela como fallback
                float screenDist = sqrtf((screenPos.x - screenCenter.x) * (screenPos.x - screenCenter.x) + 
                                         (screenPos.y - screenCenter.y) * (screenPos.y - screenCenter.y));
                distance = static_cast<int>(screenDist * 0.1f); // Aproximação grosseira
            }
        }
        
        std::string type = Game::GetEntityTypeName(entity.EntityPtr);
        std::string name = Game::GetEntityRealName(entity.EntityPtr);

        if (type == "dayzplayer" && enablePlayerESP && distance <= playerESPDistance) {
            DrawPlayerESP(entity.EntityPtr, distance, screenPos);
        }
        else if (type == "dayzanimal" && enableAnimalESP && distance <= animalESPDistance) {
            DrawAnimalESP(entity.EntityPtr, screenPos, name, distance);
        }
    }
}
