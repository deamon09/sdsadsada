#pragma once
#include "../Imgui/imgui.h"
#include <string>
#include <unordered_map>

// ============================================================================
// FUNÇÕES DE DESENHO OTIMIZADAS PARA OVERLAY
// ============================================================================

namespace DrawOptimized {

    // Cache de conversões de cor para melhor performance
    static std::unordered_map<uint32_t, ImU32> colorCache;

    // Otimizado: Conversão de cor com cache
    inline ImU32 GetColorU32Cached(float r, float g, float b, float a = 1.0f) {
        const uint32_t key = 
            (static_cast<uint32_t>(r * 255) << 24) |
            (static_cast<uint32_t>(g * 255) << 16) |
            (static_cast<uint32_t>(b * 255) << 8) |
            static_cast<uint32_t>(a * 255);

        auto it = colorCache.find(key);
        if (it != colorCache.end()) {
            return it->second;
        }

        ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, a));
        colorCache[key] = color;
        return color;
    }

    // Otimizado: Texto com sombra (versão rápida)
    inline void DrawTextWithShadow(ImDrawList* drawList, const ImVec2& pos, 
                                   ImU32 color, const char* text, 
                                   float shadowOffset = 1.0f) {
        if (!drawList || !text || !*text) return;

        // Sombra
        constexpr ImU32 shadowColor = IM_COL32(0, 0, 0, 200);
        drawList->AddText(ImVec2(pos.x + shadowOffset, pos.y + shadowOffset), 
                         shadowColor, text);
        
        // Texto principal
        drawList->AddText(pos, color, text);
    }

    // Otimizado: Texto com outline completo (4 direções)
    inline void DrawTextWithOutline(ImDrawList* drawList, const ImVec2& pos,
                                   ImU32 color, const char* text,
                                   float outlineThickness = 1.0f) {
        if (!drawList || !text || !*text) return;

        constexpr ImU32 outlineColor = IM_COL32(0, 0, 0, 255);
        
        // Outline em 4 direções (mais rápido que 8)
        drawList->AddText(ImVec2(pos.x - outlineThickness, pos.y), outlineColor, text);
        drawList->AddText(ImVec2(pos.x + outlineThickness, pos.y), outlineColor, text);
        drawList->AddText(ImVec2(pos.x, pos.y - outlineThickness), outlineColor, text);
        drawList->AddText(ImVec2(pos.x, pos.y + outlineThickness), outlineColor, text);
        
        // Texto principal
        drawList->AddText(pos, color, text);
    }

    // Otimizado: Linha com espessura
    inline void DrawLine(ImDrawList* drawList, const ImVec2& p1, const ImVec2& p2,
                        ImU32 color, float thickness = 1.0f) {
        if (!drawList) return;
        drawList->AddLine(p1, p2, color, thickness);
    }

    // Otimizado: Retângulo preenchido
    inline void DrawFilledRect(ImDrawList* drawList, const ImVec2& min, const ImVec2& max,
                              ImU32 color, float rounding = 0.0f) {
        if (!drawList) return;
        drawList->AddRectFilled(min, max, color, rounding);
    }

    // Otimizado: Retângulo com borda
    inline void DrawRect(ImDrawList* drawList, const ImVec2& min, const ImVec2& max,
                        ImU32 color, float thickness = 1.0f, float rounding = 0.0f) {
        if (!drawList) return;
        drawList->AddRect(min, max, color, rounding, 0, thickness);
    }

    // Otimizado: Círculo
    inline void DrawCircle(ImDrawList* drawList, const ImVec2& center, float radius,
                          ImU32 color, int segments = 0, float thickness = 1.0f) {
        if (!drawList) return;
        
        // Auto-calcular segmentos baseado no raio para melhor performance
        if (segments == 0) {
            segments = static_cast<int>(radius * 0.5f);
            segments = (segments < 12) ? 12 : (segments > 64) ? 64 : segments;
        }
        
        drawList->AddCircle(center, radius, color, segments, thickness);
    }

    // Otimizado: Círculo preenchido
    inline void DrawFilledCircle(ImDrawList* drawList, const ImVec2& center, float radius,
                                 ImU32 color, int segments = 0) {
        if (!drawList) return;
        
        if (segments == 0) {
            segments = static_cast<int>(radius * 0.5f);
            segments = (segments < 12) ? 12 : (segments > 64) ? 64 : segments;
        }
        
        drawList->AddCircleFilled(center, radius, color, segments);
    }

    // Otimizado: Box ESP com cantos (corner box)
    inline void DrawCornerBox(ImDrawList* drawList, const ImVec2& min, const ImVec2& max,
                             ImU32 color, float cornerLength = 10.0f, float thickness = 2.0f) {
        if (!drawList) return;

        const float width = max.x - min.x;
        const float height = max.y - min.y;
        const float cornerW = (cornerLength > width * 0.5f) ? width * 0.3f : cornerLength;
        const float cornerH = (cornerLength > height * 0.5f) ? height * 0.3f : cornerLength;

        // Canto superior esquerdo
        drawList->AddLine(ImVec2(min.x, min.y), ImVec2(min.x + cornerW, min.y), color, thickness);
        drawList->AddLine(ImVec2(min.x, min.y), ImVec2(min.x, min.y + cornerH), color, thickness);

        // Canto superior direito
        drawList->AddLine(ImVec2(max.x, min.y), ImVec2(max.x - cornerW, min.y), color, thickness);
        drawList->AddLine(ImVec2(max.x, min.y), ImVec2(max.x, min.y + cornerH), color, thickness);

        // Canto inferior esquerdo
        drawList->AddLine(ImVec2(min.x, max.y), ImVec2(min.x + cornerW, max.y), color, thickness);
        drawList->AddLine(ImVec2(min.x, max.y), ImVec2(min.x, max.y - cornerH), color, thickness);

        // Canto inferior direito
        drawList->AddLine(ImVec2(max.x, max.y), ImVec2(max.x - cornerW, max.y), color, thickness);
        drawList->AddLine(ImVec2(max.x, max.y), ImVec2(max.x, max.y - cornerH), color, thickness);
    }

    // Otimizado: Barra de vida/saúde
    inline void DrawHealthBar(ImDrawList* drawList, const ImVec2& pos, float width, float height,
                             float healthPercent, bool vertical = false) {
        if (!drawList) return;

        healthPercent = (healthPercent < 0.0f) ? 0.0f : (healthPercent > 1.0f) ? 1.0f : healthPercent;

        // Cores baseadas na porcentagem de vida
        ImU32 healthColor;
        if (healthPercent > 0.6f) {
            healthColor = IM_COL32(0, 255, 0, 255); // Verde
        } else if (healthPercent > 0.3f) {
            healthColor = IM_COL32(255, 255, 0, 255); // Amarelo
        } else {
            healthColor = IM_COL32(255, 0, 0, 255); // Vermelho
        }

        constexpr ImU32 bgColor = IM_COL32(0, 0, 0, 180);
        constexpr ImU32 borderColor = IM_COL32(0, 0, 0, 255);

        if (vertical) {
            const float filledHeight = height * healthPercent;
            const ImVec2 bgMin = pos;
            const ImVec2 bgMax = ImVec2(pos.x + width, pos.y + height);
            const ImVec2 healthMin = ImVec2(pos.x, pos.y + height - filledHeight);
            const ImVec2 healthMax = ImVec2(pos.x + width, pos.y + height);

            // Background
            drawList->AddRectFilled(bgMin, bgMax, bgColor);
            // Barra de vida
            drawList->AddRectFilled(healthMin, healthMax, healthColor);
            // Borda
            drawList->AddRect(bgMin, bgMax, borderColor, 0.0f, 0, 1.0f);
        } else {
            const float filledWidth = width * healthPercent;
            const ImVec2 bgMin = pos;
            const ImVec2 bgMax = ImVec2(pos.x + width, pos.y + height);
            const ImVec2 healthMin = pos;
            const ImVec2 healthMax = ImVec2(pos.x + filledWidth, pos.y + height);

            // Background
            drawList->AddRectFilled(bgMin, bgMax, bgColor);
            // Barra de vida
            drawList->AddRectFilled(healthMin, healthMax, healthColor);
            // Borda
            drawList->AddRect(bgMin, bgMax, borderColor, 0.0f, 0, 1.0f);
        }
    }

    // Otimizado: Linha tracejada (dashed line)
    inline void DrawDashedLine(ImDrawList* drawList, const ImVec2& p1, const ImVec2& p2,
                              ImU32 color, float dashLength = 5.0f, float gapLength = 3.0f,
                              float thickness = 1.0f) {
        if (!drawList) return;

        const float dx = p2.x - p1.x;
        const float dy = p2.y - p1.y;
        const float length = sqrtf(dx * dx + dy * dy);
        
        if (length < 0.01f) return;

        const float dirX = dx / length;
        const float dirY = dy / length;
        const float segmentLength = dashLength + gapLength;
        const int segments = static_cast<int>(length / segmentLength);

        for (int i = 0; i <= segments; ++i) {
            const float startDist = i * segmentLength;
            const float endDist = startDist + dashLength;

            if (startDist >= length) break;

            const ImVec2 start = ImVec2(
                p1.x + dirX * startDist,
                p1.y + dirY * startDist
            );

            const ImVec2 end = ImVec2(
                p1.x + dirX * ((endDist > length) ? length : endDist),
                p1.y + dirY * ((endDist > length) ? length : endDist)
            );

            drawList->AddLine(start, end, color, thickness);
        }
    }

    // Otimizado: Texto centralizado
    inline void DrawCenteredText(ImDrawList* drawList, const ImVec2& center,
                                 ImU32 color, const char* text, bool withOutline = true) {
        if (!drawList || !text || !*text) return;

        const ImVec2 textSize = ImGui::CalcTextSize(text);
        const ImVec2 pos = ImVec2(center.x - textSize.x * 0.5f, center.y - textSize.y * 0.5f);

        if (withOutline) {
            DrawTextWithOutline(drawList, pos, color, text);
        } else {
            drawList->AddText(pos, color, text);
        }
    }

    // Otimizado: Limpar cache de cores (chamar periodicamente)
    inline void ClearColorCache() {
        if (colorCache.size() > 1000) {
            colorCache.clear();
        }
    }

} // namespace DrawOptimized
