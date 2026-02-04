#pragma once
#include "../Imgui/imgui.h"
#include "../Imgui/imgui_internal.h"
#include "ConfigSystem.h"
#include "iconfont.h"
#include "fa.h"
#include <string>
#include <vector>
#include <random>
#include <cmath>

namespace ModernMenu {

namespace Colors {
    constexpr ImVec4 Background = ImVec4(0.06f, 0.06f, 0.08f, 0.98f);
    constexpr ImVec4 BackgroundDark = ImVec4(0.04f, 0.04f, 0.06f, 1.0f);
    constexpr ImVec4 Header = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);
    constexpr ImVec4 Accent = ImVec4(0.40f, 0.70f, 1.0f, 1.0f);
    constexpr ImVec4 AccentHovered = ImVec4(0.50f, 0.80f, 1.0f, 1.0f);
    constexpr ImVec4 AccentActive = ImVec4(0.30f, 0.60f, 0.90f, 1.0f);
    constexpr ImVec4 AccentGlow = ImVec4(0.40f, 0.70f, 1.0f, 0.3f);
    constexpr ImVec4 Text = ImVec4(0.95f, 0.95f, 0.98f, 1.0f);
    constexpr ImVec4 TextDisabled = ImVec4(0.5f, 0.5f, 0.55f, 1.0f);
    constexpr ImVec4 Border = ImVec4(0.25f, 0.25f, 0.30f, 0.5f);
    constexpr ImVec4 Success = ImVec4(0.2f, 0.9f, 0.4f, 1.0f);
    constexpr ImVec4 Warning = ImVec4(1.0f, 0.8f, 0.2f, 1.0f);
    constexpr ImVec4 Danger = ImVec4(0.9f, 0.2f, 0.3f, 1.0f);
}

struct Particle {
    ImVec2 position;
    ImVec2 velocity;
    float size;
    float alpha;
    float life;
    ImVec4 color;
    
    Particle() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> distX(0.0f, 1920.0f);
        std::uniform_real_distribution<float> distY(0.0f, 1080.0f);
        std::uniform_real_distribution<float> distVel(-0.5f, 0.5f);
        std::uniform_real_distribution<float> distSize(1.0f, 3.0f);
        
        position = ImVec2(distX(gen), distY(gen));
        velocity = ImVec2(distVel(gen), distVel(gen));
        size = distSize(gen);
        alpha = 0.3f;
        life = 1.0f;
        color = Colors::Accent;
    }
    
    void Update(float deltaTime, const ImVec2& windowSize) {
        position.x += velocity.x * deltaTime * 60.0f;
        position.y += velocity.y * deltaTime * 60.0f;
        
        if (position.x < 0) position.x = windowSize.x;
        if (position.x > windowSize.x) position.x = 0;
        if (position.y < 0) position.y = windowSize.y;
        if (position.y > windowSize.y) position.y = 0;
    }
};

class ParticleSystem {
private:
    std::vector<Particle> particles;
    float time;
    
public:
    ParticleSystem(int count = 100) : time(0.0f) {
        particles.reserve(count);
        for (int i = 0; i < count; i++) {
            particles.emplace_back();
        }
    }
    
    void Update(float deltaTime, const ImVec2& windowSize) {
        time += deltaTime;
        for (auto& p : particles) {
            p.Update(deltaTime, windowSize);
        }
    }
    
    void Render(ImDrawList* drawList, const ImVec2& windowPos, const ImVec2& windowSize) {
        for (size_t i = 0; i < particles.size(); i++) {
            auto& p = particles[i];
            ImVec2 screenPos = ImVec2(windowPos.x + p.position.x, windowPos.y + p.position.y);
            
            ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(p.color.x, p.color.y, p.color.z, p.alpha));
            drawList->AddCircleFilled(screenPos, p.size, color);
            
            for (size_t j = i + 1; j < particles.size(); j++) {
                auto& p2 = particles[j];
                float dx = p.position.x - p2.position.x;
                float dy = p.position.y - p2.position.y;
                float dist = sqrtf(dx * dx + dy * dy);
                
                if (dist < 150.0f) {
                    float alpha = (1.0f - dist / 150.0f) * 0.15f;
                    ImVec2 screenPos2 = ImVec2(windowPos.x + p2.position.x, windowPos.y + p2.position.y);
                    ImU32 lineColor = ImGui::ColorConvertFloat4ToU32(ImVec4(p.color.x, p.color.y, p.color.z, alpha));
                    drawList->AddLine(screenPos, screenPos2, lineColor, 1.0f);
                }
            }
        }
    }
};

inline void SetupStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    style.WindowRounding = 12.0f;
    style.ChildRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 6.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 6.0f;
    
    style.WindowPadding = ImVec2(15.0f, 15.0f);
    style.FramePadding = ImVec2(10.0f, 6.0f);
    style.ItemSpacing = ImVec2(10.0f, 10.0f);
    style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
    style.IndentSpacing = 25.0f;
    
    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 0.0f;
    style.PopupBorderSize = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;
    
    style.ScrollbarSize = 16.0f;
    style.GrabMinSize = 12.0f;
    
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = Colors::Text;
    colors[ImGuiCol_TextDisabled] = Colors::TextDisabled;
    colors[ImGuiCol_WindowBg] = Colors::Background;
    colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.08f, 0.10f, 0.6f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.06f, 0.06f, 0.08f, 0.95f);
    colors[ImGuiCol_Border] = Colors::Border;
    colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.14f, 0.8f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.16f, 0.18f, 0.8f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.20f, 0.22f, 1.0f);
    colors[ImGuiCol_TitleBg] = Colors::Header;
    colors[ImGuiCol_TitleBgActive] = Colors::Header;
    colors[ImGuiCol_TitleBgCollapsed] = Colors::Header;
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.07f, 0.6f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.35f, 0.8f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.45f, 0.8f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.50f, 0.50f, 0.55f, 0.8f);
    colors[ImGuiCol_CheckMark] = Colors::Accent;
    colors[ImGuiCol_SliderGrab] = Colors::Accent;
    colors[ImGuiCol_SliderGrabActive] = Colors::AccentActive;
    colors[ImGuiCol_Button] = ImVec4(0.12f, 0.12f, 0.14f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.16f, 0.16f, 0.18f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.20f, 0.22f, 1.0f);
    colors[ImGuiCol_Header] = ImVec4(0.12f, 0.12f, 0.14f, 0.8f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.16f, 0.16f, 0.18f, 0.8f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.20f, 0.22f, 1.0f);
    colors[ImGuiCol_Separator] = Colors::Border;
    colors[ImGuiCol_SeparatorHovered] = Colors::Accent;
    colors[ImGuiCol_SeparatorActive] = Colors::AccentActive;
    colors[ImGuiCol_ResizeGrip] = Colors::Accent;
    colors[ImGuiCol_ResizeGripHovered] = Colors::AccentHovered;
    colors[ImGuiCol_ResizeGripActive] = Colors::AccentActive;
    colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.12f, 0.14f, 0.9f);
    colors[ImGuiCol_TabHovered] = Colors::AccentHovered;
    colors[ImGuiCol_TabActive] = Colors::Accent;
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.12f, 0.12f, 0.14f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.16f, 0.16f, 0.18f, 1.0f);
    colors[ImGuiCol_PlotLines] = Colors::Accent;
    colors[ImGuiCol_PlotLinesHovered] = Colors::AccentHovered;
    colors[ImGuiCol_PlotHistogram] = Colors::Accent;
    colors[ImGuiCol_PlotHistogramHovered] = Colors::AccentHovered;
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.40f, 0.70f, 1.0f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = Colors::Accent;
    colors[ImGuiCol_NavHighlight] = Colors::Accent;
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.06f, 0.06f, 0.08f, 0.75f);
}

inline bool IconButton(const char* icon, const char* label, const ImVec2& size = ImVec2(0, 0)) {
    char buf[256];
    snprintf(buf, sizeof(buf), "%s  %s", icon, label);
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    
    bool result = ImGui::Button(buf, size);
    
    if (ImGui::IsItemHovered()) {
        ImVec2 buttonSize = size.x > 0 ? size : ImGui::CalcTextSize(buf);
        buttonSize.x += ImGui::GetStyle().FramePadding.x * 2;
        buttonSize.y += ImGui::GetStyle().FramePadding.y * 2;
        
        drawList->AddRect(pos, ImVec2(pos.x + buttonSize.x, pos.y + buttonSize.y), 
                         ImGui::ColorConvertFloat4ToU32(Colors::Accent), 6.0f, 0, 2.0f);
    }
    
    return result;
}

inline bool StyledCheckbox(const char* label, bool* v) {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    bool result = ImGui::Checkbox(label, v);
    ImGui::PopStyleVar(2);
    return result;
}

inline bool StyledSliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d") {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.12f, 0.12f, 0.14f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.16f, 0.16f, 0.18f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.22f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 6.0f);
    bool result = ImGui::SliderInt(label, v, v_min, v_max, format);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
    return result;
}

inline bool StyledSliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f") {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.12f, 0.12f, 0.14f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.16f, 0.16f, 0.18f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.22f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 6.0f);
    bool result = ImGui::SliderFloat(label, v, v_min, v_max, format);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
    return result;
}

inline bool StyledColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags = 0) {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    bool result = ImGui::ColorEdit4(label, col, flags | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
    ImGui::PopStyleVar(2);
    return result;
}

inline void SeparatorText(const char* text) {
    ImGui::Spacing();
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float width = ImGui::GetContentRegionAvail().x;
    ImVec2 textSize = ImGui::CalcTextSize(text);
    
    float lineY = pos.y + textSize.y * 0.5f;
    float textX = pos.x + (width - textSize.x) * 0.5f;
    
    drawList->AddLine(ImVec2(pos.x, lineY), ImVec2(textX - 10, lineY), 
                     ImGui::ColorConvertFloat4ToU32(Colors::Border), 1.0f);
    drawList->AddLine(ImVec2(textX + textSize.x + 10, lineY), ImVec2(pos.x + width, lineY), 
                     ImGui::ColorConvertFloat4ToU32(Colors::Border), 1.0f);
    
    ImGui::SetCursorPosX(textX);
    ImGui::TextColored(Colors::Accent, "%s", text);
    ImGui::Spacing();
}

inline void BeginGroup(const char* title) {
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.10f, 0.7f));
    ImGui::BeginChild(title, ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysAutoResize);
    
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
    ImGui::TextColored(Colors::Accent, "%s", title);
    ImGui::PopFont();
    
    ImGui::Spacing();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float width = ImGui::GetContentRegionAvail().x;
    drawList->AddLine(pos, ImVec2(pos.x + width, pos.y), 
                     ImGui::ColorConvertFloat4ToU32(Colors::Border), 1.0f);
    ImGui::Spacing();
}

inline void EndGroup() {
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

inline bool TabButton(const char* icon, const char* label, bool selected, const ImVec2& size) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    
    if (selected) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.40f, 0.70f, 1.0f, 0.3f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.50f, 0.80f, 1.0f, 0.4f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.30f, 0.60f, 0.90f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Text, Colors::Accent);
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.12f, 0.12f, 0.14f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.16f, 0.16f, 0.18f, 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20f, 0.20f, 0.22f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, Colors::Text);
    }
    
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    
    char buf[256];
    snprintf(buf, sizeof(buf), "%s\n%s", icon, label);
    
    bool result = ImGui::Button(buf, size);
    
    if (selected) {
        drawList->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), 
                         ImGui::ColorConvertFloat4ToU32(Colors::Accent), 8.0f, 0, 2.0f);
    }
    
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(4);
    
    return result;
}

struct Notification {
    std::string message;
    ImVec4 color;
    float duration;
    float elapsed;
    float slideIn;
    
    Notification(const std::string& msg, const ImVec4& col, float dur)
        : message(msg), color(col), duration(dur), elapsed(0.0f), slideIn(0.0f) {}
};

class NotificationSystem {
private:
    std::vector<Notification> notifications;
    
public:
    void AddNotification(const std::string& message, const ImVec4& color = Colors::Accent, float duration = 3.0f) {
        notifications.emplace_back(message, color, duration);
    }
    
    void Update(float deltaTime) {
        for (auto it = notifications.begin(); it != notifications.end();) {
            it->elapsed += deltaTime;
            it->slideIn = std::min(it->slideIn + deltaTime * 4.0f, 1.0f);
            
            if (it->elapsed >= it->duration) {
                it = notifications.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    void Render() {
        ImGuiIO& io = ImGui::GetIO();
        float yOffset = 20.0f;
        
        for (auto& notif : notifications) {
            float alpha = 1.0f;
            if (notif.elapsed > notif.duration - 0.5f) {
                alpha = (notif.duration - notif.elapsed) / 0.5f;
            }
            
            float slideOffset = (1.0f - notif.slideIn) * 300.0f;
            
            ImVec2 textSize = ImGui::CalcTextSize(notif.message.c_str());
            ImVec2 padding = ImVec2(20, 12);
            ImVec2 windowSize = ImVec2(textSize.x + padding.x * 2, textSize.y + padding.y * 2);
            ImVec2 pos = ImVec2(io.DisplaySize.x - windowSize.x - 20.0f + slideOffset, yOffset);
            
            ImGui::SetNextWindowPos(pos);
            ImGui::SetNextWindowSize(windowSize);
            ImGui::SetNextWindowBgAlpha(alpha * 0.95f);
            
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.10f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Border, notif.color);
            
            ImGui::Begin("##notification", nullptr, 
                        ImGuiWindowFlags_NoTitleBar | 
                        ImGuiWindowFlags_NoResize | 
                        ImGuiWindowFlags_NoMove | 
                        ImGuiWindowFlags_NoScrollbar | 
                        ImGuiWindowFlags_NoInputs);
            
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 windowPos = ImGui::GetWindowPos();
            drawList->AddRectFilled(windowPos, ImVec2(windowPos.x + 4, windowPos.y + windowSize.y), 
                                   ImGui::ColorConvertFloat4ToU32(notif.color), 8.0f);
            
            ImGui::TextColored(ImVec4(notif.color.x, notif.color.y, notif.color.z, alpha), "%s", notif.message.c_str());
            
            ImGui::End();
            
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar(2);
            
            yOffset += windowSize.y + 10.0f;
        }
    }
};

extern NotificationSystem g_notifications;
extern ParticleSystem g_particles;

} // namespace ModernMenu
