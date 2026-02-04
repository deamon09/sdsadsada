#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include "../api/json.hpp"
#include "../Imgui/imgui.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

// ============================================================================
// SISTEMA DE CONFIGURAÇÃO AVANÇADO
// ============================================================================

class ConfigSystem {
public:
    struct Config {
        // ESP Settings
        struct ESPSettings {
            bool enablePlayerESP = true;
            bool enableZombieESP = false;
            bool enableItemESP = true;
            bool enableCarESP = true;
            
            int playerDistance = 1500;
            int zombieDistance = 500;
            int itemDistance = 1200;
            int carDistance = 1500;
            
            bool showPlayerBox = true;
            bool showPlayerName = true;
            bool showPlayerDistance = true;
            bool showPlayerSkeleton = false;
            bool showPlayerHealth = true;
            
            ImVec4 playerColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            ImVec4 zombieColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            ImVec4 itemColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
            ImVec4 carColor = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
        } esp;
        
        // Aimbot Settings
        struct AimbotSettings {
            bool enabled = false;
            float fov = 100.0f;
            float smoothing = 5.0f;
            int maxDistance = 1500;
            int boneIndex = 24; // Head
            bool drawFOV = true;
            bool visibleOnly = false;
            ImVec4 fovColor = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
        } aimbot;
        
        // Loot Settings
        struct LootSettings {
            bool showWeapons = true;
            bool showFood = true;
            bool showMedical = true;
            bool showAmmo = true;
            bool showClothing = true;
            bool showBackpacks = true;
            bool showContainers = true;
            bool showQuality = true;
            
            ImVec4 weaponColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            ImVec4 foodColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
            ImVec4 medicalColor = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);
            ImVec4 ammoColor = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
        } loot;
        
        // Misc Settings
        struct MiscSettings {
            bool removeGrass = false;
            bool perfectWeather = false;
            bool changeFOV = false;
            float horizontalFOV = 1.333f;
            float verticalFOV = 0.75f;
            float eyeAccom = 5.0f;
            float brightness = 1.0f;
            
            bool showCrosshair = false;
            float crosshairSize = 8.0f;
            float crosshairGap = 2.5f;
            float crosshairThickness = 2.0f;
            ImVec4 crosshairColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
        } misc;
        
        // Performance Settings
        struct PerformanceSettings {
            int maxFPS = 120;
            bool enableCache = true;
            bool enableCulling = true;
            bool enableLOD = true;
            int updateInterval = 100;
        } performance;
        
        std::string name = "default";
        std::string description = "";
        std::string author = "";
        std::string version = "1.0";
    };
    
private:
    Config currentConfig;
    std::string configDirectory = "configs";
    std::vector<std::string> availableConfigs;
    
    // Converter ImVec4 para JSON
    static json Vec4ToJson(const ImVec4& vec) {
        return json::array({vec.x, vec.y, vec.z, vec.w});
    }
    
    // Converter JSON para ImVec4
    static ImVec4 JsonToVec4(const json& j) {
        if (j.is_array() && j.size() == 4) {
            return ImVec4(j[0], j[1], j[2], j[3]);
        }
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    
public:
    ConfigSystem() {
        EnsureDirectoryExists();
        ScanConfigs();
    }
    
    // Garantir que o diretório existe
    void EnsureDirectoryExists() {
        if (!fs::exists(configDirectory)) {
            fs::create_directories(configDirectory);
        }
    }
    
    // Escanear configurações disponíveis
    void ScanConfigs() {
        availableConfigs.clear();
        
        if (!fs::exists(configDirectory)) {
            return;
        }
        
        for (const auto& entry : fs::directory_iterator(configDirectory)) {
            if (entry.path().extension() == ".json") {
                availableConfigs.push_back(entry.path().stem().string());
            }
        }
    }
    
    // Salvar configuração
    bool SaveConfig(const std::string& name) {
        try {
            json j;
            
            // Metadata
            j["name"] = name;
            j["description"] = currentConfig.description;
            j["author"] = currentConfig.author;
            j["version"] = currentConfig.version;
            j["timestamp"] = std::time(nullptr);
            
            // ESP Settings
            j["esp"]["enablePlayerESP"] = currentConfig.esp.enablePlayerESP;
            j["esp"]["enableZombieESP"] = currentConfig.esp.enableZombieESP;
            j["esp"]["enableItemESP"] = currentConfig.esp.enableItemESP;
            j["esp"]["enableCarESP"] = currentConfig.esp.enableCarESP;
            
            j["esp"]["playerDistance"] = currentConfig.esp.playerDistance;
            j["esp"]["zombieDistance"] = currentConfig.esp.zombieDistance;
            j["esp"]["itemDistance"] = currentConfig.esp.itemDistance;
            j["esp"]["carDistance"] = currentConfig.esp.carDistance;
            
            j["esp"]["showPlayerBox"] = currentConfig.esp.showPlayerBox;
            j["esp"]["showPlayerName"] = currentConfig.esp.showPlayerName;
            j["esp"]["showPlayerDistance"] = currentConfig.esp.showPlayerDistance;
            j["esp"]["showPlayerSkeleton"] = currentConfig.esp.showPlayerSkeleton;
            j["esp"]["showPlayerHealth"] = currentConfig.esp.showPlayerHealth;
            
            j["esp"]["playerColor"] = Vec4ToJson(currentConfig.esp.playerColor);
            j["esp"]["zombieColor"] = Vec4ToJson(currentConfig.esp.zombieColor);
            j["esp"]["itemColor"] = Vec4ToJson(currentConfig.esp.itemColor);
            j["esp"]["carColor"] = Vec4ToJson(currentConfig.esp.carColor);
            
            // Aimbot Settings
            j["aimbot"]["enabled"] = currentConfig.aimbot.enabled;
            j["aimbot"]["fov"] = currentConfig.aimbot.fov;
            j["aimbot"]["smoothing"] = currentConfig.aimbot.smoothing;
            j["aimbot"]["maxDistance"] = currentConfig.aimbot.maxDistance;
            j["aimbot"]["boneIndex"] = currentConfig.aimbot.boneIndex;
            j["aimbot"]["drawFOV"] = currentConfig.aimbot.drawFOV;
            j["aimbot"]["visibleOnly"] = currentConfig.aimbot.visibleOnly;
            j["aimbot"]["fovColor"] = Vec4ToJson(currentConfig.aimbot.fovColor);
            
            // Loot Settings
            j["loot"]["showWeapons"] = currentConfig.loot.showWeapons;
            j["loot"]["showFood"] = currentConfig.loot.showFood;
            j["loot"]["showMedical"] = currentConfig.loot.showMedical;
            j["loot"]["showAmmo"] = currentConfig.loot.showAmmo;
            j["loot"]["showClothing"] = currentConfig.loot.showClothing;
            j["loot"]["showBackpacks"] = currentConfig.loot.showBackpacks;
            j["loot"]["showContainers"] = currentConfig.loot.showContainers;
            j["loot"]["showQuality"] = currentConfig.loot.showQuality;
            
            j["loot"]["weaponColor"] = Vec4ToJson(currentConfig.loot.weaponColor);
            j["loot"]["foodColor"] = Vec4ToJson(currentConfig.loot.foodColor);
            j["loot"]["medicalColor"] = Vec4ToJson(currentConfig.loot.medicalColor);
            j["loot"]["ammoColor"] = Vec4ToJson(currentConfig.loot.ammoColor);
            
            // Misc Settings
            j["misc"]["removeGrass"] = currentConfig.misc.removeGrass;
            j["misc"]["perfectWeather"] = currentConfig.misc.perfectWeather;
            j["misc"]["changeFOV"] = currentConfig.misc.changeFOV;
            j["misc"]["horizontalFOV"] = currentConfig.misc.horizontalFOV;
            j["misc"]["verticalFOV"] = currentConfig.misc.verticalFOV;
            j["misc"]["eyeAccom"] = currentConfig.misc.eyeAccom;
            j["misc"]["brightness"] = currentConfig.misc.brightness;
            
            j["misc"]["showCrosshair"] = currentConfig.misc.showCrosshair;
            j["misc"]["crosshairSize"] = currentConfig.misc.crosshairSize;
            j["misc"]["crosshairGap"] = currentConfig.misc.crosshairGap;
            j["misc"]["crosshairThickness"] = currentConfig.misc.crosshairThickness;
            j["misc"]["crosshairColor"] = Vec4ToJson(currentConfig.misc.crosshairColor);
            
            // Performance Settings
            j["performance"]["maxFPS"] = currentConfig.performance.maxFPS;
            j["performance"]["enableCache"] = currentConfig.performance.enableCache;
            j["performance"]["enableCulling"] = currentConfig.performance.enableCulling;
            j["performance"]["enableLOD"] = currentConfig.performance.enableLOD;
            j["performance"]["updateInterval"] = currentConfig.performance.updateInterval;
            
            // Salvar arquivo
            std::string filepath = configDirectory + "/" + name + ".json";
            std::ofstream file(filepath);
            file << j.dump(4); // Pretty print com indentação de 4 espaços
            file.close();
            
            currentConfig.name = name;
            ScanConfigs();
            
            return true;
        }
        catch (const std::exception& e) {
            // Log error
            return false;
        }
    }
    
    // Carregar configuração
    bool LoadConfig(const std::string& name) {
        try {
            std::string filepath = configDirectory + "/" + name + ".json";
            
            if (!fs::exists(filepath)) {
                return false;
            }
            
            std::ifstream file(filepath);
            json j = json::parse(file);
            file.close();
            
            // Metadata
            currentConfig.name = j.value("name", name);
            currentConfig.description = j.value("description", "");
            currentConfig.author = j.value("author", "");
            currentConfig.version = j.value("version", "1.0");
            
            // ESP Settings
            if (j.contains("esp")) {
                auto& esp = j["esp"];
                currentConfig.esp.enablePlayerESP = esp.value("enablePlayerESP", true);
                currentConfig.esp.enableZombieESP = esp.value("enableZombieESP", false);
                currentConfig.esp.enableItemESP = esp.value("enableItemESP", true);
                currentConfig.esp.enableCarESP = esp.value("enableCarESP", true);
                
                currentConfig.esp.playerDistance = esp.value("playerDistance", 1500);
                currentConfig.esp.zombieDistance = esp.value("zombieDistance", 500);
                currentConfig.esp.itemDistance = esp.value("itemDistance", 1200);
                currentConfig.esp.carDistance = esp.value("carDistance", 1500);
                
                currentConfig.esp.showPlayerBox = esp.value("showPlayerBox", true);
                currentConfig.esp.showPlayerName = esp.value("showPlayerName", true);
                currentConfig.esp.showPlayerDistance = esp.value("showPlayerDistance", true);
                currentConfig.esp.showPlayerSkeleton = esp.value("showPlayerSkeleton", false);
                currentConfig.esp.showPlayerHealth = esp.value("showPlayerHealth", true);
                
                if (esp.contains("playerColor")) currentConfig.esp.playerColor = JsonToVec4(esp["playerColor"]);
                if (esp.contains("zombieColor")) currentConfig.esp.zombieColor = JsonToVec4(esp["zombieColor"]);
                if (esp.contains("itemColor")) currentConfig.esp.itemColor = JsonToVec4(esp["itemColor"]);
                if (esp.contains("carColor")) currentConfig.esp.carColor = JsonToVec4(esp["carColor"]);
            }
            
            // Aimbot Settings
            if (j.contains("aimbot")) {
                auto& aimbot = j["aimbot"];
                currentConfig.aimbot.enabled = aimbot.value("enabled", false);
                currentConfig.aimbot.fov = aimbot.value("fov", 100.0f);
                currentConfig.aimbot.smoothing = aimbot.value("smoothing", 5.0f);
                currentConfig.aimbot.maxDistance = aimbot.value("maxDistance", 1500);
                currentConfig.aimbot.boneIndex = aimbot.value("boneIndex", 24);
                currentConfig.aimbot.drawFOV = aimbot.value("drawFOV", true);
                currentConfig.aimbot.visibleOnly = aimbot.value("visibleOnly", false);
                
                if (aimbot.contains("fovColor")) currentConfig.aimbot.fovColor = JsonToVec4(aimbot["fovColor"]);
            }
            
            // Loot Settings
            if (j.contains("loot")) {
                auto& loot = j["loot"];
                currentConfig.loot.showWeapons = loot.value("showWeapons", true);
                currentConfig.loot.showFood = loot.value("showFood", true);
                currentConfig.loot.showMedical = loot.value("showMedical", true);
                currentConfig.loot.showAmmo = loot.value("showAmmo", true);
                currentConfig.loot.showClothing = loot.value("showClothing", true);
                currentConfig.loot.showBackpacks = loot.value("showBackpacks", true);
                currentConfig.loot.showContainers = loot.value("showContainers", true);
                currentConfig.loot.showQuality = loot.value("showQuality", true);
                
                if (loot.contains("weaponColor")) currentConfig.loot.weaponColor = JsonToVec4(loot["weaponColor"]);
                if (loot.contains("foodColor")) currentConfig.loot.foodColor = JsonToVec4(loot["foodColor"]);
                if (loot.contains("medicalColor")) currentConfig.loot.medicalColor = JsonToVec4(loot["medicalColor"]);
                if (loot.contains("ammoColor")) currentConfig.loot.ammoColor = JsonToVec4(loot["ammoColor"]);
            }
            
            // Misc Settings
            if (j.contains("misc")) {
                auto& misc = j["misc"];
                currentConfig.misc.removeGrass = misc.value("removeGrass", false);
                currentConfig.misc.perfectWeather = misc.value("perfectWeather", false);
                currentConfig.misc.changeFOV = misc.value("changeFOV", false);
                currentConfig.misc.horizontalFOV = misc.value("horizontalFOV", 1.333f);
                currentConfig.misc.verticalFOV = misc.value("verticalFOV", 0.75f);
                currentConfig.misc.eyeAccom = misc.value("eyeAccom", 5.0f);
                currentConfig.misc.brightness = misc.value("brightness", 1.0f);
                
                currentConfig.misc.showCrosshair = misc.value("showCrosshair", false);
                currentConfig.misc.crosshairSize = misc.value("crosshairSize", 8.0f);
                currentConfig.misc.crosshairGap = misc.value("crosshairGap", 2.5f);
                currentConfig.misc.crosshairThickness = misc.value("crosshairThickness", 2.0f);
                
                if (misc.contains("crosshairColor")) currentConfig.misc.crosshairColor = JsonToVec4(misc["crosshairColor"]);
            }
            
            // Performance Settings
            if (j.contains("performance")) {
                auto& perf = j["performance"];
                currentConfig.performance.maxFPS = perf.value("maxFPS", 120);
                currentConfig.performance.enableCache = perf.value("enableCache", true);
                currentConfig.performance.enableCulling = perf.value("enableCulling", true);
                currentConfig.performance.enableLOD = perf.value("enableLOD", true);
                currentConfig.performance.updateInterval = perf.value("updateInterval", 100);
            }
            
            return true;
        }
        catch (const std::exception& e) {
            // Log error
            return false;
        }
    }
    
    // Deletar configuração
    bool DeleteConfig(const std::string& name) {
        try {
            std::string filepath = configDirectory + "/" + name + ".json";
            
            if (fs::exists(filepath)) {
                fs::remove(filepath);
                ScanConfigs();
                return true;
            }
            
            return false;
        }
        catch (const std::exception& e) {
            return false;
        }
    }
    
    // Getters
    Config& GetConfig() { return currentConfig; }
    const Config& GetConfig() const { return currentConfig; }
    const std::vector<std::string>& GetAvailableConfigs() const { return availableConfigs; }
    std::string GetCurrentConfigName() const { return currentConfig.name; }
};

// Instância global
extern ConfigSystem g_configSystem;
