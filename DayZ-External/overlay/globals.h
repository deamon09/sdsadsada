#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <cstdint>
#include <atomic>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <chrono>

extern LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct player_t {
    std::uint64_t EntityPtr;
    std::uint64_t TableEntry;
    std::uint32_t NetworkID;
    
    mutable std::string cachedName;
    mutable std::chrono::steady_clock::time_point lastNameUpdate;
    
    player_t() : EntityPtr(0), TableEntry(0), NetworkID(0) {}
    
    bool IsValid() const noexcept {
        return EntityPtr != 0 && EntityPtr != 0xFFFFFFFFFFFFFFFF;
    }
};

// Estrutura de item otimizada
struct item_t {
    std::uint64_t ItemPtr;
    std::uint64_t ItemTable;
    
    item_t() : ItemPtr(0), ItemTable(0) {}
    
    bool IsValid() const noexcept {
        return ItemPtr != 0 && ItemPtr != 0xFFFFFFFFFFFFFFFF;
    }
};

// ============================================================================
// ESTRUTURA GLOBAL PRINCIPAL
// ============================================================================

struct GlobalState {
    // Process Information
    std::atomic<uintptr_t> process_id{0};
    HANDLE handle{nullptr};
    
    // Window Information
    HWND OverlayWnd{nullptr};
    HWND TargetWnd{nullptr};
    std::atomic<int> Width{0};
    std::atomic<int> Height{0};
    
    // Configuration
    bool console{false};
    char lTargetWindow[256]{"DayZ"};
    char lWindowName[256]{"Google"};
    
    // Screen Information
    std::atomic<unsigned int> game_screen_X{0};
    std::atomic<unsigned int> game_screen_Y{0};
    
    // Game Memory
    std::atomic<uintptr_t> modulebase{0};
    std::atomic<uint64_t> Base{0};
    std::atomic<uint64_t> World{0};
    
    // Performance Metrics
    struct {
        std::atomic<float> currentFPS{0.0f};
        std::atomic<int> drawCalls{0};
        std::atomic<int> entityCount{0};
        std::atomic<int> itemCount{0};
    } metrics;
    
    // Thread Safety
    mutable std::mutex worldMutex;
    mutable std::mutex entityMutex;
    
    // Helper Methods
    bool IsInitialized() const noexcept {
        return Base.load() != 0 && World.load() != 0;
    }
    
    bool IsProcessValid() const noexcept {
        return handle != nullptr && handle != INVALID_HANDLE_VALUE;
    }
    
    bool IsWindowValid() const noexcept {
        return OverlayWnd != nullptr && TargetWnd != nullptr;
    }
};

// ============================================================================
// SISTEMA DE CACHE GLOBAL
// ============================================================================

struct GlobalCache {
    // Cache de entidades
    struct EntityCache {
        std::atomic<uint64_t> localPlayer{0};
        std::atomic<uint64_t> camera{0};
        std::atomic<uint64_t> networkManager{0};
        std::chrono::steady_clock::time_point lastUpdate;
        std::mutex cacheMutex;
        
        void Invalidate() noexcept {
            localPlayer.store(0);
            camera.store(0);
            networkManager.store(0);
        }
        
        bool IsValid() const noexcept {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate);
            return elapsed.count() < 1000; // Cache válido por 1 segundo
        }
    } entityCache;
    
    // Cache de nomes
    struct NameCache {
        std::unordered_map<uint32_t, std::pair<std::string, uint64_t>> playerNames;
        std::mutex nameMutex;
        static constexpr uint64_t CACHE_TIMEOUT = 30000; // 30 segundos
        
        void Clear() {
            std::lock_guard<std::mutex> lock(nameMutex);
            playerNames.clear();
        }
        
        void CleanupOld() {
            std::lock_guard<std::mutex> lock(nameMutex);
            const uint64_t currentTime = GetTickCount64();
            
            for (auto it = playerNames.begin(); it != playerNames.end();) {
                if ((currentTime - it->second.second) > CACHE_TIMEOUT) {
                    it = playerNames.erase(it);
                } else {
                    ++it;
                }
            }
        }
    } nameCache;
};

// ============================================================================
// SISTEMA DE CONFIGURAÇÃO GLOBAL
// ============================================================================

struct GlobalConfig {
    // Paths
    std::string configDirectory{"configs"};
    std::string currentConfigName{"default"};
    
    // Performance Settings
    struct {
        int maxFPSActive{120};
        int maxFPSInactive{60};
        bool enableVSync{false};
        bool enableMultithreading{true};
        int updateInterval{100}; // ms
    } performance;
    
    // ESP Settings
    struct {
        int maxPlayerDistance{1500};
        int maxItemDistance{1200};
        int maxZombieDistance{500};
        bool enableCulling{true};
        bool enableLOD{true};
    } esp;
    
    // Debug Settings
    struct {
        bool showFPS{true};
        bool showMetrics{false};
        bool enableLogging{false};
        bool showConsole{false};
    } debug;
};

// ============================================================================
// VARIÁVEIS GLOBAIS EXPORTADAS
// ============================================================================

extern GlobalState globals;
extern GlobalCache g_cache;
extern GlobalConfig g_config;

// Silent Aim Target (thread-safe)
extern std::atomic<uintptr_t> g_SilentAimTarget;

// ============================================================================
// FUNÇÕES AUXILIARES INLINE
// ============================================================================

// Validação rápida de ponteiro
inline bool IsValidPointer(uintptr_t ptr) noexcept {
    return ptr >= 0x1000 && ptr <= 0x7FFFFFFFFFFF && ptr != 0xFFFFFFFFFFFFFFFF;
}

// Obter FPS atual
inline float GetCurrentFPS() noexcept {
    return globals.metrics.currentFPS.load();
}

// Atualizar FPS
inline void UpdateFPS(float fps) noexcept {
    globals.metrics.currentFPS.store(fps);
}

// Incrementar draw calls
inline void IncrementDrawCalls() noexcept {
    globals.metrics.drawCalls.fetch_add(1);
}

// Resetar draw calls
inline void ResetDrawCalls() noexcept {
    globals.metrics.drawCalls.store(0);
}

// Obter contagem de entidades
inline int GetEntityCount() noexcept {
    return globals.metrics.entityCount.load();
}

// Atualizar contagem de entidades
inline void UpdateEntityCount(int count) noexcept {
    globals.metrics.entityCount.store(count);
}

// ============================================================================
// MACROS ÚTEIS
// ============================================================================

#define ISVALID(x) IsValidPointer(static_cast<uintptr_t>(x))
#define SAFE_READ(type, address) (ISVALID(address) ? read<type>(address) : type{})

// ============================================================================
// SISTEMA DE LOGGING (OPCIONAL)
// ============================================================================

#ifdef ENABLE_LOGGING
    #define LOG_INFO(msg) if(g_config.debug.enableLogging) { std::cout << "[INFO] " << msg << std::endl; }
    #define LOG_ERROR(msg) if(g_config.debug.enableLogging) { std::cerr << "[ERROR] " << msg << std::endl; }
    #define LOG_WARNING(msg) if(g_config.debug.enableLogging) { std::cout << "[WARNING] " << msg << std::endl; }
#else
    #define LOG_INFO(msg)
    #define LOG_ERROR(msg)
    #define LOG_WARNING(msg)
#endif