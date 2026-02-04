#include "globals.h"

GlobalState globals;
GlobalCache g_cache;
GlobalConfig g_config;
std::atomic<uintptr_t> g_SilentAimTarget{0};
