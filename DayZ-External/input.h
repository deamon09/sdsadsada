#pragma once
#include <Windows.h>
#include "camera.h"
#include "memory.h"

inline Camera cam;
inline float speed = 0.3f;
inline bool freecamAtiva = false;

inline void AtualizarFreecam ( ) {
    if ( !freecamAtiva ) return;

    if ( GetAsyncKeyState ( 'W' ) & 0x8000 ) cam.position.z += speed;
    if ( GetAsyncKeyState ( 'S' ) & 0x8000 ) cam.position.z -= speed;
    if ( GetAsyncKeyState ( 'A' ) & 0x8000 ) cam.position.x -= speed;
    if ( GetAsyncKeyState ( 'D' ) & 0x8000 ) cam.position.x += speed;
    if ( GetAsyncKeyState ( VK_SPACE ) & 0x8000 ) cam.position.y += speed;
    if ( GetAsyncKeyState ( VK_CONTROL ) & 0x8000 ) cam.position.y -= speed;

    EscreverCameraNaMemoria ( cam );
}

inline void TeclasFreecam ( ) {
    if ( GetAsyncKeyState ( VK_F1 ) & 1 ) {
        freecamAtiva = !freecamAtiva;
    }
}
