#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include "game/vector.h"
#include "overlay/globals.h"

namespace MemorySystem {

template<typename T>
class MemoryCache {
private:
    struct CacheEntry {
        T value;
        std::chrono::steady_clock::time_point timestamp;
        
        bool IsValid(uint64_t timeout_ms) const {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - timestamp);
            return elapsed.count() < timeout_ms;
        }
    };
    
    std::unordered_map<uintptr_t, CacheEntry> cache;
    std::mutex cacheMutex;
    uint64_t timeout_ms;
    size_t maxSize;
    
public:
    MemoryCache(uint64_t timeout = 100, size_t max_size = 1000) 
        : timeout_ms(timeout), maxSize(max_size) {}
    
    bool Get(uintptr_t address, T& out) {
        std::lock_guard<std::mutex> lock(cacheMutex);
        
        auto it = cache.find(address);
        if (it != cache.end() && it->second.IsValid(timeout_ms)) {
            out = it->second.value;
            return true;
        }
        return false;
    }
    
    void Set(uintptr_t address, const T& value) {
        std::lock_guard<std::mutex> lock(cacheMutex);
        
        // Limpar cache se estiver muito grande
        if (cache.size() >= maxSize) {
            cache.clear();
        }
        
        cache[address] = {value, std::chrono::steady_clock::now()};
    }
    
    void Clear() {
        std::lock_guard<std::mutex> lock(cacheMutex);
        cache.clear();
    }
    
    size_t Size() const {
        std::lock_guard<std::mutex> lock(cacheMutex);
        return cache.size();
    }
};

// ============================================================================
// BATCH READING - Ler múltiplos endereços de uma vez
// ============================================================================

template<typename T>
class BatchReader {
private:
    std::vector<uintptr_t> addresses;
    std::vector<T> results;
    
public:
    void AddAddress(uintptr_t address) {
        addresses.push_back(address);
    }
    
    bool ReadAll() {
        if (addresses.empty()) return false;
        
        results.resize(addresses.size());
        
        // Ler todos os endereços de uma vez (mais eficiente)
        for (size_t i = 0; i < addresses.size(); ++i) {
            if (!IsValidPointer(addresses[i])) {
                results[i] = T{};
                continue;
            }
            
            SIZE_T bytesRead = 0;
            if (!ReadProcessMemory(globals.handle, 
                                  reinterpret_cast<LPCVOID>(addresses[i]),
                                  &results[i], 
                                  sizeof(T), 
                                  &bytesRead)) {
                results[i] = T{};
            }
        }
        
        return true;
    }
    
    const std::vector<T>& GetResults() const {
        return results;
    }
    
    T GetResult(size_t index) const {
        return (index < results.size()) ? results[index] : T{};
    }
    
    void Clear() {
        addresses.clear();
        results.clear();
    }
};

// ============================================================================
// LEITURA OTIMIZADA COM CACHE
// ============================================================================

template<typename T>
inline T ReadCached(uintptr_t address, MemoryCache<T>& cache) {
    T value;
    if (cache.Get(address, value)) {
        return value;
    }
    
    // Ler da memória
    if (!IsValidPointer(address)) {
        return T{};
    }
    
    SIZE_T bytesRead = 0;
    if (ReadProcessMemory(globals.handle, 
                         reinterpret_cast<LPCVOID>(address),
                         &value, 
                         sizeof(T), 
                         &bytesRead)) {
        cache.Set(address, value);
        return value;
    }
    
    return T{};
}

// ============================================================================
// ESCRITA OTIMIZADA
// ============================================================================

template<typename T>
inline bool WriteSafe(uintptr_t address, const T& value) {
    if (!IsValidPointer(address)) {
        return false;
    }
    
    SIZE_T bytesWritten = 0;
    return WriteProcessMemory(globals.handle,
                             reinterpret_cast<LPVOID>(address),
                             &value,
                             sizeof(T),
                             &bytesWritten) && bytesWritten == sizeof(T);
}

// ============================================================================
// LEITURA DE STRING OTIMIZADA
// ============================================================================

inline std::string ReadStringOptimized(uintptr_t address, size_t maxLength = 256) {
    if (!IsValidPointer(address)) {
        return "";
    }
    
    std::vector<char> buffer(maxLength);
    SIZE_T bytesRead = 0;
    
    if (!ReadProcessMemory(globals.handle,
                          reinterpret_cast<LPCVOID>(address),
                          buffer.data(),
                          maxLength,
                          &bytesRead)) {
        return "";
    }
    
    // Garantir null termination
    buffer[maxLength - 1] = '\0';
    
    return std::string(buffer.data());
}

// ============================================================================
// PATTERN SCANNING OTIMIZADO
// ============================================================================

class PatternScanner {
private:
    static std::vector<int> PatternToBytes(const char* pattern) {
        std::vector<int> bytes;
        const char* start = pattern;
        const char* end = pattern + strlen(pattern);
        
        for (const char* current = start; current < end; ++current) {
            if (*current == '?') {
                ++current;
                if (*current == '?') ++current;
                bytes.push_back(-1);
            } else {
                bytes.push_back(strtoul(current, const_cast<char**>(&current), 16));
            }
        }
        
        return bytes;
    }
    
public:
    static uintptr_t Scan(const char* pattern, uintptr_t start = 0, uintptr_t end = 0) {
        uintptr_t moduleBase = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
        
        auto dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleBase);
        auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(
            reinterpret_cast<uint8_t*>(moduleBase) + dosHeader->e_lfanew
        );
        
        uintptr_t sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
        auto patternBytes = PatternToBytes(pattern);
        auto scanBytes = reinterpret_cast<uint8_t*>(moduleBase);
        
        size_t patternSize = patternBytes.size();
        const int* patternData = patternBytes.data();
        
        uintptr_t scanStart = (start > moduleBase) ? (start - moduleBase) : 0;
        uintptr_t scanEnd = (end > 0) ? (end - moduleBase) : sizeOfImage;
        
        for (uintptr_t i = scanStart; i < scanEnd - patternSize; ++i) {
            bool found = true;
            
            for (size_t j = 0; j < patternSize; ++j) {
                if (scanBytes[i + j] != patternData[j] && patternData[j] != -1) {
                    found = false;
                    break;
                }
            }
            
            if (found) {
                return reinterpret_cast<uintptr_t>(&scanBytes[i]);
            }
        }
        
        return 0;
    }
};

// ============================================================================
// GERENCIADOR DE CACHE GLOBAL
// ============================================================================

class CacheManager {
private:
    MemoryCache<uint64_t> uint64Cache;
    MemoryCache<uint32_t> uint32Cache;
    MemoryCache<float> floatCache;
    MemoryCache<Vector3> vector3Cache;
    
public:
    CacheManager() 
        : uint64Cache(100, 1000)
        , uint32Cache(100, 1000)
        , floatCache(100, 500)
        , vector3Cache(100, 500) {}
    
    template<typename T>
    T Read(uintptr_t address);
    
    void ClearAll() {
        uint64Cache.Clear();
        uint32Cache.Clear();
        floatCache.Clear();
        vector3Cache.Clear();
    }
    
    void GetStats(size_t& u64Size, size_t& u32Size, size_t& fSize, size_t& v3Size) {
        u64Size = uint64Cache.Size();
        u32Size = uint32Cache.Size();
        fSize = floatCache.Size();
        v3Size = vector3Cache.Size();
    }
};

// Especialização de templates
template<>
inline uint64_t CacheManager::Read<uint64_t>(uintptr_t address) {
    return ReadCached(address, uint64Cache);
}

template<>
inline uint32_t CacheManager::Read<uint32_t>(uintptr_t address) {
    return ReadCached(address, uint32Cache);
}

template<>
inline float CacheManager::Read<float>(uintptr_t address) {
    return ReadCached(address, floatCache);
}

template<>
inline Vector3 CacheManager::Read<Vector3>(uintptr_t address) {
    return ReadCached(address, vector3Cache);
}

// Instância global do gerenciador de cache
extern CacheManager g_memoryCache;

} // namespace MemorySystem

// ============================================================================
// FUNÇÕES GLOBAIS COMPATÍVEIS COM CÓDIGO EXISTENTE
// ============================================================================

// Manter compatibilidade com código existente
template<typename T>
inline T read(uintptr_t address) {
    if (!IsValidPointer(address)) {
        return T{};
    }
    
    T value;
    SIZE_T bytesRead = 0;
    
    if (ReadProcessMemory(globals.handle,
                         reinterpret_cast<LPCVOID>(address),
                         &value,
                         sizeof(T),
                         &bytesRead)) {
        return value;
    }
    
    return T{};
}

template<typename T>
inline bool write(uintptr_t address, const T& value) {
    return MemorySystem::WriteSafe(address, value);
}

// Leitura de string Arma (compatibilidade)
inline std::string ReadArmaString(uintptr_t address) {
    if (!IsValidPointer(address)) {
        return "";
    }
    
    // Ler tamanho da string
    int size = read<int>(address + 0x8);
    if (size <= 0 || size > 1024) {
        return "";
    }
    
    // Ler ponteiro para os dados
    uintptr_t dataPtr = read<uintptr_t>(address + 0x10);
    if (!IsValidPointer(dataPtr)) {
        return "";
    }
    
    return MemorySystem::ReadStringOptimized(dataPtr, size + 1);
}
