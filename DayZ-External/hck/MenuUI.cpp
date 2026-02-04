#include "ModernMenu.h"
#include "ConfigSystem.h"
#include "settings.h"
#include "../overlay/drawings_optimized.h"
#include "../overlay/globals.h"
#include "../memory_optimized.h"
#include "iconfont.h"
#include "fa.h"

extern bool g_showMenu;
static int currentTab = 0;
static float deltaTime = 0.0f;
static auto lastFrameTime = std::chrono::steady_clock::now();
static float menuAlpha = 0.0f;

void DrawMenuUI() {
    if (!g_showMenu) {
        menuAlpha = std::max(menuAlpha - deltaTime * 4.0f, 0.0f);
        if (menuAlpha <= 0.0f) return;
    } else {
        menuAlpha = std::min(menuAlpha + deltaTime * 4.0f, 1.0f);
    }
    
    auto currentTime = std::chrono::steady_clock::now();
    deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
    lastFrameTime = currentTime;
    
    ModernMenu::g_notifications.Update(deltaTime);
    
    ImGui::SetNextWindowSize(ImVec2(900, 700), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f - 450, ImGui::GetIO().DisplaySize.y * 0.5f - 350), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(menuAlpha * 0.98f);
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
    
    if (ImGui::Begin("##MainWindow", &g_showMenu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
        
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        
        // Renderizar partículas de fundo
        ModernMenu::g_particles.Update(deltaTime, windowSize);
        drawList->PushClipRect(windowPos, ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y));
        ModernMenu::g_particles.Render(drawList, windowPos, windowSize);
        drawList->PopClipRect();
        
        // Gradiente no header
        drawList->AddRectFilledMultiColor(
            windowPos,
            ImVec2(windowPos.x + windowSize.x, windowPos.y + 60),
            ImGui::ColorConvertFloat4ToU32(ImVec4(0.10f, 0.10f, 0.12f, 1.0f)),
            ImGui::ColorConvertFloat4ToU32(ImVec4(0.10f, 0.10f, 0.12f, 1.0f)),
            ImGui::ColorConvertFloat4ToU32(ImVec4(0.08f, 0.08f, 0.10f, 1.0f)),
            ImGui::ColorConvertFloat4ToU32(ImVec4(0.08f, 0.08f, 0.10f, 1.0f))
        );
        
        // Linha de acento no topo
        drawList->AddRectFilled(
            windowPos,
            ImVec2(windowPos.x + windowSize.x, windowPos.y + 3),
            ImGui::ColorConvertFloat4ToU32(ModernMenu::Colors::Accent)
        );
        
        // Título com ícone animado
        ImGui::SetCursorPos(ImVec2(25, 18));
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        
        float pulse = sinf(ImGui::GetTime() * 2.0f) * 0.2f + 0.8f;
        ImGui::TextColored(ImVec4(ModernMenu::Colors::Accent.x * pulse, 
                                  ModernMenu::Colors::Accent.y * pulse, 
                                  ModernMenu::Colors::Accent.z * pulse, 1.0f), 
                          ICON_FA_CROSSHAIRS);
        ImGui::SameLine();
        ImGui::TextColored(ModernMenu::Colors::Text, " DayZ ESP");
        ImGui::SameLine();
        ImGui::TextColored(ModernMenu::Colors::TextDisabled, " | Modern Edition");
        ImGui::PopFont();
        
        // Botão de fechar estilizado
        ImGui::SetCursorPos(ImVec2(windowSize.x - 45, 15));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0.2f, 0.3f, 0.3f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.3f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.2f, 0.3f, 0.7f));
        if (ImGui::Button(ICON_FA_TIMES, ImVec2(30, 30))) {
            g_showMenu = false;
        }
        ImGui::PopStyleColor(3);
        
        ImGui::SetCursorPosY(65);
        
        ImGui::BeginChild("MainLayout", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar);
        {
            // Sidebar com efeito de vidro
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.10f, 0.8f));
            ImGui::BeginChild("Sidebar", ImVec2(170, 0), true);
            {
                ImVec2 buttonSize = ImVec2(150, 60);
                ImGui::SetCursorPosY(15);
                ImGui::SetCursorPosX(10);
                
                if (ModernMenu::TabButton(ICON_FA_EYE, "ESP", currentTab == 0, buttonSize)) {
                    currentTab = 0;
                }
                
                ImGui::SetCursorPosX(10);
                if (ModernMenu::TabButton(ICON_FA_CROSSHAIRS, "Aimbot", currentTab == 1, buttonSize)) {
                    currentTab = 1;
                }
                
                ImGui::SetCursorPosX(10);
                if (ModernMenu::TabButton(ICON_FA_ARCHIVE, "Loot", currentTab == 2, buttonSize)) {
                    currentTab = 2;
                }
                
                ImGui::SetCursorPosX(10);
                if (ModernMenu::TabButton(ICON_FA_COG, "Misc", currentTab == 3, buttonSize)) {
                    currentTab = 3;
                }
                
                ImGui::SetCursorPosX(10);
                if (ModernMenu::TabButton(ICON_FA_TACHOMETER_ALT, "Stats", currentTab == 4, buttonSize)) {
                    currentTab = 4;
                }
                
                ImGui::SetCursorPosX(10);
                if (ModernMenu::TabButton(ICON_FA_SAVE, "Config", currentTab == 5, buttonSize)) {
                    currentTab = 5;
                }
                
                // Info no rodapé
                ImGui::SetCursorPos(ImVec2(10, windowSize.y - 100));
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.14f, 0.6f));
                ImGui::BeginChild("Info", ImVec2(150, 80), true);
                ImGui::TextColored(ModernMenu::Colors::TextDisabled, "Version");
                ImGui::TextColored(ModernMenu::Colors::Accent, "2.1");
                ImGui::Spacing();
                ImGui::TextColored(ModernMenu::Colors::TextDisabled, "FPS");
                ImGui::TextColored(ModernMenu::Colors::Success, "%.0f", ImGui::GetIO().Framerate);
                ImGui::EndChild();
                ImGui::PopStyleColor();
            }
            ImGui::EndChild();
            ImGui::PopStyleColor();
            
            ImGui::SameLine();
            
            // Conteúdo com scroll suave
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
            ImGui::BeginChild("Content", ImVec2(0, 0), false);
            {
                auto& config = g_configSystem.GetConfig();
                
                switch (currentTab) {
                    case 0: // ESP
                    {
                        ImGui::BeginChild("ESPScroll", ImVec2(0, 0), false);
                        
                        // Player ESP
                        if (ImGui::CollapsingHeader(ICON_FA_USER " Player ESP", ImGuiTreeNodeFlags_DefaultOpen)) {
                            ImGui::Indent(10);
                            ModernMenu::StyledCheckbox("Enable Player ESP", &enablePlayerESP);
                            ModernMenu::StyledSliderInt("Max Distance", &playerESPDistance, 100, 3000, "%d m");
                            ModernMenu::StyledCheckbox("Show Box", &Playerbox);
                            ModernMenu::StyledCheckbox("Show Name", &Playername);
                            ModernMenu::StyledCheckbox("Show Distance", &playerDistance);
                            ModernMenu::StyledCheckbox("Show Skeleton", &PlayerSkeleton);
                            ModernMenu::StyledCheckbox("Show Health Bar", &showHealthBar);
                            ModernMenu::StyledCheckbox("Show Item in Hands", &showItemInHands);
                            ModernMenu::StyledColorEdit4("Player Color", (float*)&playerBoxColor);
                            ImGui::Unindent(10);
                            ImGui::Spacing();
                        }
                        
                        // Zombie ESP
                        if (ImGui::CollapsingHeader(ICON_FA_SKULL " Zombie ESP")) {
                            ImGui::Indent(10);
                            ModernMenu::StyledCheckbox("Enable Zombie ESP", &enableInfectedESP);
                            ModernMenu::StyledSliderInt("Max Distance", &infectedESPDistance, 50, 1000, "%d m");
                            ModernMenu::StyledCheckbox("Show Box", &Zombiebox);
                            ModernMenu::StyledCheckbox("Show Name", &zombieShowName);
                            ModernMenu::StyledCheckbox("Show Distance", &zombieShowDistance);
                            ModernMenu::StyledCheckbox("Show Skeleton", &zombieskeleton);
                            ModernMenu::StyledColorEdit4("Zombie Color", (float*)&ZombieboxColor);
                            ImGui::Unindent(10);
                            ImGui::Spacing();
                        }
                        
                        // Corpse ESP
                        if (ImGui::CollapsingHeader(ICON_FA_HEARTBEAT " Corpse ESP")) {
                            ImGui::Indent(10);
                            ModernMenu::StyledCheckbox("Show Corpse Tag", &showCorpseTag);
                            ModernMenu::StyledCheckbox("Show Dead Players", &deadESP);
                            ImGui::Unindent(10);
                            ImGui::Spacing();
                        }
                        
                        // Vehicle ESP
                        if (ImGui::CollapsingHeader(ICON_FA_CAR " Vehicle ESP")) {
                            ImGui::Indent(10);
                            ModernMenu::StyledCheckbox("Enable Car ESP", &enableCarESP);
                            ModernMenu::StyledSliderInt("Max Distance", &carESPDistance, 100, 3000, "%d m");
                            ModernMenu::StyledCheckbox("Show Car Lines", &carespline);
                            ModernMenu::StyledColorEdit4("Car Color", (float*)&carColor);
                            ImGui::Unindent(10);
                        }
                        
                        ImGui::EndChild();
                        break;
                    }
                    
                    case 1: // Aimbot
                    {
                        ImGui::BeginChild("AimbotScroll", ImVec2(0, 0), false);
                        
                        if (ImGui::CollapsingHeader(ICON_FA_CROSSHAIRS " Aimbot Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
                            ImGui::Indent(10);
                            ModernMenu::StyledCheckbox("Enable Silent Aim", &ActiveSilentAim);
                            ModernMenu::StyledSliderFloat("FOV", &aimfov, 10.0f, 360.0f, "%.1f");
                            ModernMenu::StyledSliderInt("Max Distance", &LimiteDoSilent, 100, 3000, "%d m");
                            
                            ImGui::Text("Target Bone:");
                            const char* bones[] = {"Head", "Neck", "Chest", "Pelvis", "Left Hand", "Right Hand"};
                            ImGui::Combo("##BoneSelect", &bonechoose, bones, IM_ARRAYSIZE(bones));
                            
                            ModernMenu::StyledCheckbox("Draw FOV Circle", &ToggleFov);
                            ModernMenu::StyledSliderFloat("FOV Thickness", &fovThickness, 1.0f, 5.0f, "%.1f");
                            ModernMenu::StyledCheckbox("Ammo Speed Change", &AmmoSpeedChange);
                            ImGui::Unindent(10);
                        }
                        
                        ImGui::EndChild();
                        break;
                    }
                    
                    case 2: // Loot
                    {
                        ImGui::BeginChild("LootScroll", ImVec2(0, 0), false);
                        
                        if (ImGui::CollapsingHeader(ICON_FA_ARCHIVE " Loot ESP", ImGuiTreeNodeFlags_DefaultOpen)) {
                            ImGui::Indent(10);
                            ModernMenu::StyledCheckbox("Enable Item ESP", &enableInventoryESP);
                            ModernMenu::StyledSliderInt("Max Distance", &maxloot, 100, 2000, "%d m");
                            ModernMenu::StyledCheckbox("Show Quality", &qualidade);
                            ImGui::Unindent(10);
                            ImGui::Spacing();
                        }
                        
                        if (ImGui::CollapsingHeader(ICON_FA_FILTER " Item Filters")) {
                            ImGui::Indent(10);
                            
                            ImGui::Columns(2, nullptr, false);
                            
                            ModernMenu::StyledCheckbox("Weapons", &weapon);
                            ModernMenu::StyledCheckbox("Food", &comida);
                            ModernMenu::StyledCheckbox("Medical", &medicine);
                            ModernMenu::StyledCheckbox("Ammo", &ProxyMagazines);
                            
                            ImGui::NextColumn();
                            
                            ModernMenu::StyledCheckbox("Clothing", &clothing);
                            ModernMenu::StyledCheckbox("Backpacks", &BackPack);
                            ModernMenu::StyledCheckbox("Containers", &Containers);
                            ModernMenu::StyledCheckbox("Drinks", &drinks);
                            
                            ImGui::Columns(1);
                            
                            ImGui::Unindent(10);
                        }
                        
                        if (ImGui::CollapsingHeader(ICON_FA_PALETTE " Loot Colors")) {
                            ImGui::Indent(10);
                            ModernMenu::StyledColorEdit4("Weapon Color", (float*)&colorWeapon);
                            ModernMenu::StyledColorEdit4("Food Color", (float*)&colorFood);
                            ModernMenu::StyledColorEdit4("Medical Color", (float*)&colorMedicines);
                            ModernMenu::StyledColorEdit4("Ammo Color", (float*)&colorAmmo);
                            ModernMenu::StyledColorEdit4("Clothing Color", (float*)&colorClothing);
                            ModernMenu::StyledColorEdit4("Backpack Color", (float*)&colorBackpacks);
                            ImGui::Unindent(10);
                        }
                        
                        ImGui::EndChild();
                        break;
                    }
                    
                    case 3: // Misc
                    {
                        ImGui::BeginChild("MiscScroll", ImVec2(0, 0), false);
                        
                        if (ImGui::CollapsingHeader(ICON_FA_EYE " Visual", ImGuiTreeNodeFlags_DefaultOpen)) {
                            ImGui::Indent(10);
                            ModernMenu::StyledCheckbox("Remove Grass", &removeGrass);
                            ModernMenu::StyledCheckbox("Perfect Weather", &PerfectWeather);
                            ModernMenu::StyledSliderFloat("Brightness", &Claridade, 0.1f, 5.0f, "%.2f");
                            ModernMenu::StyledSliderFloat("Eye Accom", &eyeAccomValue, 0.0f, 10.0f, "%.1f");
                            ImGui::Unindent(10);
                            ImGui::Spacing();
                        }
                        
                        if (ImGui::CollapsingHeader(ICON_FA_EXPAND " FOV")) {
                            ImGui::Indent(10);
                            ModernMenu::StyledCheckbox("Change FOV", &Changefov);
                            ModernMenu::StyledSliderFloat("Horizontal FOV", &Horizontal, 0.5f, 2.0f, "%.3f");
                            ModernMenu::StyledSliderFloat("Vertical FOV", &Vertical, 0.5f, 2.0f, "%.3f");
                            ImGui::Unindent(10);
                            ImGui::Spacing();
                        }
                        
                        if (ImGui::CollapsingHeader(ICON_FA_CROSSHAIRS " Crosshair")) {
                            ImGui::Indent(10);
                            ModernMenu::StyledCheckbox("Show Crosshair", &ToggleCross);
                            ModernMenu::StyledSliderFloat("Size", &crosshairSize, 1.0f, 20.0f, "%.1f");
                            ModernMenu::StyledSliderFloat("Gap", &crosshairGap, 0.0f, 10.0f, "%.1f");
                            ModernMenu::StyledSliderFloat("Thickness", &crosshairThickness, 1.0f, 5.0f, "%.1f");
                            ModernMenu::StyledCheckbox("RGB Mode", &EnableCrossRGB);
                            ImGui::Unindent(10);
                            ImGui::Spacing();
                        }
                        
                        if (ImGui::CollapsingHeader(ICON_FA_CLOUD " Weather")) {
                            ImGui::Indent(10);
                            ModernMenu::StyledSliderFloat("Overcast", &overcast, 0.0f, 1.0f, "%.2f");
                            ModernMenu::StyledSliderFloat("Fog", &fog, 0.0f, 100.0f, "%.1f");
                            ModernMenu::StyledSliderFloat("Rain", &rain, 0.0f, 1.0f, "%.2f");
                            ModernMenu::StyledSliderFloat("Snowfall", &snowfall, 0.0f, 1.0f, "%.2f");
                            ImGui::Unindent(10);
                        }
                        
                        ImGui::EndChild();
                        break;
                    }
                    
                    case 4: // Stats
                    {
                        ImGui::BeginChild("StatsScroll", ImVec2(0, 0), false);
                        
                        if (ImGui::CollapsingHeader(ICON_FA_CHART_LINE " Performance", ImGuiTreeNodeFlags_DefaultOpen)) {
                            ImGui::Indent(10);
                            
                            ImGui::Text("FPS: %.1f", GetCurrentFPS());
                            ImGui::Text("Frame Time: %.2f ms", 1000.0f / GetCurrentFPS());
                            ImGui::Text("Entities: %d", GetEntityCount());
                            ImGui::Text("Draw Calls: %d", globals.metrics.drawCalls.load());
                            
                            ImGui::Spacing();
                            ImGui::Separator();
                            ImGui::Spacing();
                            
                            // Cache stats
                            size_t u64Size, u32Size, fSize, v3Size;
                            g_memoryCache.GetStats(u64Size, u32Size, fSize, v3Size);
                            size_t totalCache = u64Size + u32Size + fSize + v3Size;
                            
                            ImGui::Text("Memory Cache:");
                            ImGui::BulletText("uint64: %zu entries", u64Size);
                            ImGui::BulletText("uint32: %zu entries", u32Size);
                            ImGui::BulletText("float: %zu entries", fSize);
                            ImGui::BulletText("Vector3: %zu entries", v3Size);
                            ImGui::Text("Total: %zu entries", totalCache);
                            
                            ImGui::Spacing();
                            
                            if (ImGui::Button(ICON_FA_TRASH " Clear Cache", ImVec2(150, 30))) {
                                g_memoryCache.ClearAll();
                                ModernMenu::g_notifications.AddNotification(
                                    "Cache cleared!", 
                                    ModernMenu::Colors::Success
                                );
                            }
                            
                            ImGui::Unindent(10);
                        }
                        
                        if (ImGui::CollapsingHeader(ICON_FA_INFO_CIRCLE " System Info")) {
                            ImGui::Indent(10);
                            
                            ImGui::Text("Process ID: %llu", globals.process_id.load());
                            ImGui::Text("Base Address: 0x%llX", globals.Base.load());
                            ImGui::Text("World Address: 0x%llX", globals.World.load());
                            ImGui::Text("Screen: %dx%d", globals.Width.load(), globals.Height.load());
                            
                            ImGui::Unindent(10);
                        }
                        
                        ImGui::EndChild();
                        break;
                    }
                    
                    case 5: // Config
                    {
                        ImGui::BeginChild("ConfigScroll", ImVec2(0, 0), false);
                        
                        if (ImGui::CollapsingHeader(ICON_FA_SAVE " Save/Load", ImGuiTreeNodeFlags_DefaultOpen)) {
                            ImGui::Indent(10);
                            
                            static char configName[128] = "default";
                            ImGui::SetNextItemWidth(300);
                            ImGui::InputText("##ConfigName", configName, sizeof(configName));
                            
                            ImGui::Spacing();
                            
                            if (ModernMenu::IconButton(ICON_FA_SAVE, "Save Config", ImVec2(150, 35))) {
                                if (g_configSystem.SaveConfig(configName)) {
                                    ModernMenu::g_notifications.AddNotification(
                                        "Config saved successfully!", 
                                        ModernMenu::Colors::Success
                                    );
                                } else {
                                    ModernMenu::g_notifications.AddNotification(
                                        "Failed to save config!", 
                                        ModernMenu::Colors::Danger
                                    );
                                }
                            }
                            
                            ImGui::SameLine();
                            
                            if (ModernMenu::IconButton(ICON_FA_FOLDER_OPEN, "Load Config", ImVec2(150, 35))) {
                                if (g_configSystem.LoadConfig(configName)) {
                                    ModernMenu::g_notifications.AddNotification(
                                        "Config loaded successfully!", 
                                        ModernMenu::Colors::Success
                                    );
                                } else {
                                    ModernMenu::g_notifications.AddNotification(
                                        "Failed to load config!", 
                                        ModernMenu::Colors::Danger
                                    );
                                }
                            }
                            
                            ImGui::Unindent(10);
                            ImGui::Spacing();
                        }
                        
                        if (ImGui::CollapsingHeader(ICON_FA_LIST " Available Configs")) {
                            ImGui::Indent(10);
                            
                            const auto& configs = g_configSystem.GetAvailableConfigs();
                            
                            if (configs.empty()) {
                                ImGui::TextColored(ModernMenu::Colors::TextDisabled, "No configs found");
                            } else {
                                for (const auto& cfg : configs) {
                                    if (ImGui::Selectable(cfg.c_str())) {
                                        strncpy(configName, cfg.c_str(), sizeof(configName));
                                    }
                                }
                            }
                            
                            ImGui::Unindent(10);
                        }
                        
                        ImGui::EndChild();
                        break;
                    }
                }
                
                ImGui::PopStyleVar();
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();
    }
    ImGui::End();
    
    ImGui::PopStyleVar();
    
    // Renderizar notificações
    ModernMenu::g_notifications.Render();
}
