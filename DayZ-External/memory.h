#pragma once
#include <Windows.h>
#include "camera.h"

// Externa para acessar o handle no main
extern HANDLE hProcess;

inline DWORD_PTR cameraBase = 0x00000000; // Coloque o endereço real aqui

inline void EscreverCameraNaMemoria ( const Camera& cam ) {
    WriteProcessMemory ( hProcess, (LPVOID)(cameraBase + 0x10), &cam.position, sizeof ( Vector3 ), nullptr );
    WriteProcessMemory ( hProcess, (LPVOID)(cameraBase + 0x1C), &cam.rotation, sizeof ( Vector3 ), nullptr );
    WriteProcessMemory ( hProcess, (LPVOID)(cameraBase + 0x28), &cam.fov, sizeof ( float ), nullptr );
}

