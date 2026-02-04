#pragma once
#include <string>
#include "../Imgui/imgui.h"
#include <WinUser.h>
bool showplayerSlaplines = false;
bool misc_noclip{ false };
void SaveConfig(const std::string& filename, int playerESPDistance, int infectedESPDistance, int carESPDistance, int animalESPDistance);
void SaveConfig();
void LoadConfig();
void SaveSettings(const char* filename);
void LoadSettings(const char* filename);
void EnsureDirectoryExists(const std::string& path);
void UpdateConfigList(const std::string& configDir);
void SaveNewConfig(const std::string& configDir, const std::string& configName);
float aimfov = 100;
#define ENTITY_CONFIGNAME_WEAPON "Weapon"
#define ENTITY_CONFIGNAME_CLOTHING "clothing"
#define ENTITY_CONFIGNAME_INVENTORY "inventoryItem"
#define ENTITY_CONFIGNAME_OPTICS "itemoptics"
#define ENTITY_CONFIGNAME_MAGAZINES "ProxyMagazines"
int num1 = 24;
int num2 = 21;
int num3 = 21;
int num4 = 61;
int num5 = 61;
int num6 = 63;
int num7 = 63;
int num8 = 65;
int num9 = 21;
int num10 = 94;
int num11 = 94;
int num12 = 97;
int num13 = 97;
int num14 = 99;
int num15 = 21;
int num16 = 18;
int num17 = 18;
int num18 = 1;
int num19 = 1;
int num20 = 4;
int num21 = 4;
int num22 = 6;
int num23 = 18;
int num24 = 9;
int num25 = 9;
int num26 = 11;
int num27 = 11;
int num28 = 14;
bool hideLocalPlayer = true;
int addr;
bool fovs = false;
float width = 800.0f;  // or whatever your intended width is
float height = 600.0f; // or whatever your intended height is
static bool EnableFovRGB = false;
float crosshairSize = 8.0f;
float crosshairGap = 2.5f;
float crosshairThickness = 2.0f;
bool EnableCrossRGB = false;  // Controle para ativar o RGB animado


float centerX = width / 2.0f;
float centerY = height / 2.0f;

inline float radius = 60.0f;
bool DrawFov = true;

float eyeAccomValue = 5.0f;
float Claridade = 1.0f;

ImGuiStyle        _style;



void SetCustomStyle() {
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowPadding = ImVec2(11.19999980926514f, 0.0f);
    style.WindowRounding = 0.0f;
    style.WindowBorderSize = 1.0f;
    style.WindowMinSize = ImVec2(32.0f, 32.0f);
    style.WindowTitleAlign = ImVec2(0.0f, 0.4000000059604645f);
    style.ChildRounding = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 0.0f;
    style.PopupBorderSize = 1.0f;
    style.FramePadding = ImVec2(0.0f, 0.0f);
    style.FrameRounding = 1.100000023841858f;
    style.FrameBorderSize = 1.0f;
    style.ItemSpacing = ImVec2(0.0f, 2.5f);
    style.ItemInnerSpacing = ImVec2(1.0f, 16.70000076293945f);
    style.IndentSpacing = 0.0f;
    style.ColumnsMinSpacing = 0.0f;
    style.ScrollbarSize = 0.1000000014901161f;
    style.ScrollbarRounding = 20.0f;
    style.GrabMinSize = 2.599999904632568f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 0.300000011920929f;
    style.TabBorderSize = 0.0f;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);

    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 0.9399999976158142f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(9.999899930335232e-07f, 9.999946541938698e-07f, 9.999999974752427e-07f, 0.5400000214576721f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2532163560390472f, 0.2532175481319427f, 0.2532188892364502f, 0.4000000059604645f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.1416294723749161f, 0.141630157828331f, 0.1416308879852295f, 0.6700000166893005f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.03921568766236305f, 0.03921568766236305f, 0.03921568766236305f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.1568627506494522f, 0.2862745225429535f, 0.47843137383461f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.5299999713897705f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.407843142747879f, 0.407843142747879f, 0.407843142747879f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.9999899864196777f, 0.9999948143959045f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.3991376161575317f, 0.3991395831108093f, 0.3991416096687317f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.3347606062889099f, 0.3347622454166412f, 0.3347639441490173f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.1287540942430496f, 0.1287547051906586f, 0.1287553906440735f, 0.4000000059604645f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.09871145337820053f, 0.09871193021535873f, 0.09871244430541992f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06866887211799622f, 0.06866920739412308f, 0.06866955757141113f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.1373376846313477f, 0.137338325381279f, 0.1373390555381775f, 0.3100000023841858f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.184547483921051f, 0.1845483928918839f, 0.1845493316650391f, 0.800000011920929f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.06866887211799622f, 0.06866919994354248f, 0.06866955757141113f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.09803921729326248f, 0.4000000059604645f, 0.7490196228027344f, 0.7799999713897705f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.1587966829538345f, 0.1587974727153778f, 0.1587982773780823f, 1.0f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.3004261553287506f, 0.3004276156425476f, 0.3004291653633118f, 0.2000000029802322f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.1373376846313477f, 0.1373384445905685f, 0.1373390555381775f, 0.6700000166893005f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.1373376846313477f, 0.137338325381279f, 0.1373390555381775f, 0.949999988079071f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.3133015930652618f, 0.3133034408092499f, 0.3133047223091125f, 0.8619999885559082f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1502131074666977f, 0.1502138078212738f, 0.1502146124839783f, 0.800000011920929f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.1888412237167358f, 0.1888397783041f, 0.1888393312692642f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.1019607856869698f, 0.1450980454683304f, 0.9724000096321106f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1333333402872086f, 0.2588235437870026f, 0.4235294163227081f, 1.0f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.09012787789106369f, 0.09012831002473831f, 0.09012877941131592f, 0.3499999940395355f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}





/*floats*/
//float width = GetSystemMetrics(SM_CXSCREEN);
//float height = GetSystemMetrics(SM_CYSCREEN);
static int playerESPDistance = 1500; // Unique variable for Player ESP Distance
static int infectedESPDistance = 25; // Unique variable for Infected ESP Distance
static int carESPDistance = 1500; // Unique variable for Car ESP Distance
static int animalESPDistance = 1200; // Unique variable for Animal ESP Distance
static int maxloot = 1500;
static std::string selectedConfig;

// Estrutura para salvar configurações
struct GameVars {
    // ESP Distances
    int playerESPDistance = 1500;
    int infectedESPDistance = 25;
    int carESPDistance = 1500;
    int animalESPDistance = 1200;
    int maxloot = 1500;
    int LimiteDoSilent = 1500;
    
    // ESP Toggles
    bool enablePlayerESP = true;
    bool enableInfectedESP = false;
    bool enableCarESP = true;
    bool enableInventoryESP = true;
    bool enableAnimalESP = false;
    bool PlayeEsp = true;
    bool Playerbox = false;
    bool Playername = false;
    bool playerDistance = false;
    bool PlayerSkeleton = false;
    bool espline = false;
    bool carespline = false;
    
    // Aimbot
    bool ActiveSilentAim = false;
    float aimfov = 100.0f;
    float fovThickness = 2.0f;
    bool ToggleFov = false;
    bool EnableFovRGB = false;
    bool AmmoSpeedChange = false;
    int bonechoose = 0;
    
    // Loot ESP
    bool Iemtelist = false;
    bool weapon = false;
    bool comida = false;
    bool ProxyMagazines = false;
    bool Containers = false;
    bool Miras = false;
    bool drinks = false;
    bool rodas = false;
    bool BackPack = false;
    bool inventoryItem = false;
    bool medicine = false;
    bool clothing = false;
    
    // Misc
    bool ToggleCross = false;
    bool removeGrass = false;
    float Claridade = 1.0f;
    float eyeAccomValue = 5.0f;
    float Horizontal = 1.333f;
    float Vertical = 0.75f;
    float overcast = 0.0f;
    float fog = 100.0f;
    float rain = 0.0f;
    float snowfall = 0.0f;
    
    // Crosshair
    float crosshairSize = 8.0f;
    float crosshairGap = 2.5f;
    float crosshairThickness = 2.0f;
    bool EnableCrossRGB = false;
    float crosshairColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
    
    // ESP Colors
    float playerESPColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    float infectedESPColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    float carESPColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    float animalESPColor[4] = { 1.0f, 0.733f, 0.0f, 1.0f };
    float TesTESP[4] = { 255.0f, 255.0f, 255.0f, 255.0f };
    
    // Flags
    bool loadConfigRequested = false;
    bool streamproof = true;
    int espzDistance = 1500;
};

extern struct GameVars g_GameVars;

/*entitys esp*/
bool enablePlayerESP = true;
bool enableInfectedESP = false;
bool enableCarESP = true;
bool enableInventoryESP = true;
float inventoryESPFOV = 30.0f;
bool enableAnimalESP = false;
bool renderCorpse = false;
//bool render_skeleton = true;
bool zoomActive = false;
bool Playerbox = false;
bool deadESP = false;
bool PlayeEsp = true;
bool carespline = false;
bool showItemInHands = true;
bool showCorpseTag = true;
inline bool zombieShowName = true;
inline bool zombieShowDistance = true;
bool showHealthBar = true;
bool Playerlist = false;
bool  cornerBox = true;
bool Playername = false;
bool playerDistance = false;
bool PlayerSkeleton = false;
bool zombieskeleton = false;
bool infectdname = false;
bool ZombieJoyts = true;
bool Zombiebox = true;
bool PlayerJoyts = false;
bool removeGrass = false;
bool hvhamigos = true;
bool espline = false;
bool espline2 = false;
ImVec4 playerNameColor = ImVec4(60, 250, 2,  0);      // Cor inicial para o nome do jogador
ImVec4 playerDistanceColor = ImVec4(1, 1, 0, 1);   // Cor inicial para a dist�ncia
ImVec4 playerBoxColor = ImVec4(1, 0, 0, 1); 
ImVec4 ZombieboxColor = ImVec4(1, 0, 0, 1); // Cor inicial para a caixa do jogador
ImVec4 infectedColor = ImVec4(1, 0, 0, 1);         // Cor inicial para infectados
ImVec4 carColor = ImVec4(0, 0, 1, 1);               // Cor inicial para carros
ImVec4 animalColor = ImVec4(0, 0.39f, 0, 1);       // Cor inicial para animais
ImVec4 playerskeleton = ImVec4(0, 1, 0, 1);
bool ToggleFov = 0;
//bool ToggleCross = 1;
float TesTESP[] = { 255, 255, 255, 255.f };
bool Changefov = true;
bool Fov = true;
float GameFov = 1.f;
float Horizontal = 1.333;
float Vertical = 0.75;
float fovcross = 2;
int LimiteDoSilent = 1500;
bool ToggleCross = false;
float overcast = 0.f;
float fog = 100.f;
float rain = 0.f;
float snowfall = 0.f;

bool waitingForKey = false;
int freecamKey = 0;
std::string keyMode = "Press";
std::string KeyToString(int key) {
    if (key == 0) return "Auto Keybind"; // Nome inicial
    // Adicione aqui os c�digos de tecla que voc� quer tratar
    if (key == VK_LBUTTON) return "Left Mouse Button";
    if (key == VK_RBUTTON) return "Right Mouse Button";
    if (key == VK_MBUTTON) return "Middle Mouse Button";
    if (key >= 'A' && key <= 'Z') return std::string(1, (char)key); // Letras de A a Z
    if (key >= '0' && key <= '9') return std::string(1, (char)key); // N�meros de 0 a 9
    // Outros mapeamentos de teclas podem ser adicionados aqui
    return "Unknown Key";
}

float INSVeh[] = { 255, 0, 255, 255.f };
float INSVeh1[] = { 255, 122, 31, 255.f };
float INSVeh2[] = { 255, 0, 0, 255.f };

float DistanceItems = 1500;
bool freecamActive = false;

bool Weapon_Or_Ammo_Esp = false;
bool QualityItems = true;
bool Items = false;
bool Barrel_Or_Wooden_Chest_Esp = false;
bool Car_Esp = false;
bool Building_Esp = false;
bool Food_Medical_Esp = false;
float  maxplayer = 10000, maxzombie = 10000, maxanimal = 10000, maxcar = 10000;

bool weapon = false;
bool weaponToggle = false;
int weaponkey = false;
bool ProxyMagazines = false;
bool ProxyMagazines1 = false;
bool Miras = false;
bool rodas = false;
bool inventoryItem12 = false;
bool inventoryItem132 = false;
bool inventoryItem2 = false;
bool inventoryItem22 = false;

bool inventoryItem222 = false;
bool inventoryItem2222 = false;
bool inventoryItem22221 = false;
bool cerca = false;
float fovThickness = 2.0f;
bool Playerbox2 = false;
bool comida = false;
bool Containers = false;
bool BackPack = false;
bool comida1 = false;
bool SkeletonZm = false;
bool medicine = false;
bool medicine1 = false;
bool drinks = false;
bool drinks1 = false;
bool clothing = false;
bool clothing1 = false;
bool inventoryItem = false;
bool inventoryItemsss = false;
bool misc = false;
bool Iemtelist = false;
bool LootTeleport = false;
float BRTVeh[] = { 233, 255, 0, 255.f };
float RUSVeh[] = { 255, 0, 255, 255.f };
float FOVRADIO[] = { 0, 255, 255, 255.f };
float HAB[] = { 0, 0, 255, 255.f };
float ammocratee[] = { 0, 0, 255, 255.f };
float RUSVeh1[] = { 255, 0, 255, 255.f };
float RUSVeh11[] = { 0, 0, 255, 255.f };