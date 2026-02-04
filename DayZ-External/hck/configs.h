//#define _CRT_SECURE_NO_WARNINGS
//#pragma once
//#include <chrono>
//#include <filesystem>
//#include <iostream>
//#include <fstream>
//#include <string>
//std::filesystem::path GetConfigFolderPath();
//
//std::string GetNextAvailableSlot() {
//    std::filesystem::path configFolderPath = GetConfigFolderPath();
//    int slotIndex = 1;
//
//    // Procura o próximo slot disponível
//    while (std::filesystem::exists(configFolderPath / ("slot" + std::to_string(slotIndex) + ".cfg"))) {
//        slotIndex++;
//    }
//
//    return "slot" + std::to_string(slotIndex) + ".cfg";
//}
//
//void SaveConfigInNextSlot(int playerESPDistance, int infectedESPDistance, int carESPDistance, int animalESPDistance) {
//    std::string filename = GetNextAvailableSlot();
//    SaveConfig(filename, playerESPDistance, infectedESPDistance, carESPDistance, animalESPDistance);
//}
//
//
//std::filesystem::path GetConfigFolderPath() {
//    std::filesystem::path documentsPath = std::filesystem::path(std::getenv("USERPROFILE")) / "Documents";
//    return documentsPath / "ChabProject";
//}
//
//void CreateConfigFolder() {
//    try {
//        // Get the path to the Documents folder
//        std::filesystem::path documentsPath = std::filesystem::path(std::getenv("USERPROFILE")) / "Documents";
//        std::filesystem::path configFolderPath = documentsPath / "ChabProject";
//
//        // Check if the directory already exists
//        if (!std::filesystem::exists(configFolderPath)) {
//            // Create the directory
//            std::filesystem::create_directory(configFolderPath);
//            std::cout << "Configuration folder created at: " << configFolderPath << std::endl;
//        }
//    }
//    catch (const std::filesystem::filesystem_error& e) {
//        std::cerr << "Filesystem error: " << e.what() << std::endl;
//    }
//    catch (const std::exception& e) {
//        std::cerr << "Unexpected error: " << e.what() << std::endl;
//    }
//}
//
//// Save configuration to a file
//void SaveConfig(const std::string& filename, int playerESPDistance, int infectedESPDistance, int carESPDistance, int animalESPDistance) {
//    std::filesystem::path filePath = GetConfigFolderPath() / filename;
//
//    std::ofstream file(filePath);
//    if (file.is_open()) {
//        file << playerESPDistance << "\n"
//            << infectedESPDistance << "\n"
//            << carESPDistance << "\n"
//            << animalESPDistance << "\n";
//        file.close();
//        std::cout << "Config saved to: " << filePath << std::endl;
//    }
//    else {
//        std::cerr << "Failed to open file for saving: " << filePath << std::endl;
//    }
//}
//
//// Load configuration from a file
//void LoadConfig(const std::string& filename, int& playerESPDistance, int& infectedESPDistance, int& carESPDistance, int& animalESPDistance) {
//    std::filesystem::path filePath = GetConfigFolderPath() / filename;
//
//    std::ifstream file(filePath);
//    if (file.is_open()) {
//        file >> playerESPDistance;
//        file >> infectedESPDistance;
//        file >> carESPDistance;
//        file >> animalESPDistance;
//        file.close();
//        std::cout << "Config loaded from: " << filePath << std::endl;
//    }
//    else {
//        std::cerr << "Failed to open file for loading: " << filePath << std::endl;
//    }
//}
//
//// Delete a configuration file
//void DeleteConfig(const std::string& filename) {
//    std::filesystem::path filePath = GetConfigFolderPath() / filename;
//
//    if (std::filesystem::remove(filePath)) {
//        std::cout << "Config deleted: " << filePath << std::endl;
//    }
//    else {
//        std::cerr << "Failed to delete config: " << filePath << std::endl;
//    }
//}
//
//std::vector<std::string> GetConfigFiles() {
//    std::filesystem::path configFolderPath = GetConfigFolderPath();
//    std::vector<std::string> files;
//
//    if (std::filesystem::exists(configFolderPath)) {
//        for (const auto& entry : std::filesystem::directory_iterator(configFolderPath)) {
//            if (entry.is_regular_file()) {
//                files.push_back(entry.path().filename().string());
//            }
//        }
//    }
//    else {
//        std::cerr << "Config folder does not exist: " << configFolderPath << std::endl;
//    }
//
//    return files;
//}
//
//std::vector<std::string> GetConfigSlots() {
//    std::filesystem::path configFolderPath = GetConfigFolderPath();
//    std::vector<std::string> slots;
//
//    if (std::filesystem::exists(configFolderPath)) {
//        for (const auto& entry : std::filesystem::directory_iterator(configFolderPath)) {
//            if (entry.is_regular_file()) {
//                std::string filename = entry.path().filename().string();
//                if (filename.find("slot") == 0 && filename.find(".cfg") != std::string::npos) {
//                    slots.push_back(filename);
//                }
//            }
//        }
//    }
//    else {
//        std::cerr << "Config folder does not exist: " << configFolderPath << std::endl;
//    }
//
//    return slots;
//}
//
//void InitializeConfigFolder() {
//    CreateConfigFolder();
//}