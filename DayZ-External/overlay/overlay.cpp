#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <thread>
#include <chrono>
#include <Windows.h>
#include <dwmapi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "overlay.h"
#include "font.h"
#include <algorithm>

extern bool showESPMenu; // ESP Menu toggle variable

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "d3dx9.lib")

IDirect3D9Ex* p_Object = NULL;
IDirect3DDevice9Ex* p_Device = NULL;
D3DPRESENT_PARAMETERS p_Params = { NULL };

namespace
{
    void UpdateImguiDisplaySize()
    {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)globals.Width, (float)globals.Height);
    }

    bool ResetDevice()
    {
        if (!p_Device)
            return false;

        ImGui_ImplDX9_InvalidateDeviceObjects();
        HRESULT hr = p_Device->Reset(&p_Params);
        if (FAILED(hr))
            return false;
        ImGui_ImplDX9_CreateDeviceObjects();
        return true;
    }

    float GetDpiScale(HWND hWnd)
    {
        using GetDpiForWindowFn = UINT(WINAPI*)(HWND);
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (!user32)
            return 1.0f;

        auto getDpiForWindow = reinterpret_cast<GetDpiForWindowFn>(GetProcAddress(user32, "GetDpiForWindow"));
        if (!getDpiForWindow)
            return 1.0f;

        const UINT dpi = getDpiForWindow(hWnd);
        return (dpi > 0) ? (dpi / 96.0f) : 1.0f;
    }
}

// Otimizado: C�lculo de FPS mais preciso e eficiente
float CalculateFPS() {
    static int frameCount = 0;
    static std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
    static float fps = 0.0f;

    frameCount++;
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count() / 1000.0f;

    if (elapsed >= 1.0f) {
        fps = frameCount / elapsed;
        frameCount = 0;
        lastTime = currentTime;
    }

    return fps;
}

void ov::create_window(bool console)
{
    ShowWindow(GetConsoleWindow(), console ? SW_SHOW : SW_HIDE);

    WNDCLASSEX wcex = {
        sizeof(WNDCLASSEX),
        0,
        WndProc,
        0,
        0,
        nullptr,
        LoadIcon(0, IDI_APPLICATION),
        LoadCursor(0, IDC_ARROW),
        nullptr,
        nullptr,
        globals.lWindowName,
        LoadIcon(nullptr, IDI_APPLICATION)
    };

    if (!RegisterClassEx(&wcex))
    {
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
            return;
    }

    globals.TargetWnd = FindWindowA(NULL, globals.lTargetWindow);

    if (globals.TargetWnd)
    {
        RECT tSize;
        GetWindowRect(globals.TargetWnd, &tSize);
        globals.Width = tSize.right - tSize.left;
        globals.Height = tSize.bottom - tSize.top;

        globals.OverlayWnd = CreateWindowEx(
            NULL,
            globals.lWindowName,
            globals.lWindowName,
            WS_POPUP | WS_VISIBLE,
            tSize.left,
            tSize.top,
            globals.Width,
            globals.Height,
            NULL,
            NULL,
            0,
            NULL
        );

        const MARGINS Margin = { 0, 0, globals.Width, globals.Height };
        DwmExtendFrameIntoClientArea(globals.OverlayWnd, &Margin);

        SetWindowLong(globals.OverlayWnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);

        ShowWindow(globals.OverlayWnd, SW_SHOW);
        UpdateWindow(globals.OverlayWnd);
    }

    setup_directx(globals.OverlayWnd);
}

// Otimizado: Setup DirectX ultra melhorado com máxima performance
void ov::setup_directx(HWND hWnd)
{
    // Criar objeto Direct3D9Ex
    if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
    {
        MessageBoxA(nullptr, "Failed to create Direct3D9Ex object", "Error", MB_OK | MB_ICONERROR);
        exit(3);
    }

    // Configurar parâmetros de apresentação otimizados
    ZeroMemory(&p_Params, sizeof(p_Params));
    p_Params.Windowed = TRUE;
    p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD; // Mais rápido que FLIP
    p_Params.hDeviceWindow = hWnd;
    p_Params.BackBufferFormat = D3DFMT_A8R8G8B8; // 32-bit com alpha
    p_Params.BackBufferWidth = globals.Width;
    p_Params.BackBufferHeight = globals.Height;
    p_Params.BackBufferCount = 1; // Single buffering para overlay
    p_Params.EnableAutoDepthStencil = FALSE; // Não precisamos de depth buffer
    p_Params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
    p_Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; // Sem VSync
    p_Params.FullScreen_RefreshRateInHz = 0;
    p_Params.MultiSampleType = D3DMULTISAMPLE_NONE; // Sem MSAA para performance
    p_Params.MultiSampleQuality = 0;
    p_Params.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER; // Permitir lock do backbuffer

    // Tentar criar dispositivo com hardware vertex processing (mais rápido)
    HRESULT result = p_Object->CreateDeviceEx(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
        &p_Params,
        nullptr,
        &p_Device
    );

    // Fallback para mixed vertex processing
    if (FAILED(result))
    {
        result = p_Object->CreateDeviceEx(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            hWnd,
            D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
            &p_Params,
            nullptr,
            &p_Device
        );
    }

    // Último fallback para software vertex processing
    if (FAILED(result))
    {
        result = p_Object->CreateDeviceEx(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            hWnd,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING,
            &p_Params,
            nullptr,
            &p_Device
        );
    }

    if (FAILED(result))
    {
        MessageBoxA(nullptr, "Failed to create Direct3D device", "Error", MB_OK | MB_ICONERROR);
        exit(4);
    }

    // Inicializar ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Configurações otimizadas do ImGui para overlay
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;
    io.IniFilename = nullptr; // Não salvar configurações
    io.LogFilename = nullptr; // Não criar log
    io.MouseDrawCursor = false; // Não desenhar cursor do ImGui

    // Configurar display size
    UpdateImguiDisplaySize();

    // Inicializar backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX9_Init(p_Device);

    // Configurar estilo otimizado para overlay
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.WindowBorderSize = 0.0f;
    style.WindowPadding = ImVec2(0.0f, 0.0f);
    style.AntiAliasedLines = false; // Desabilitar anti-aliasing para performance
    style.AntiAliasedFill = false;

    // Carregar fontes otimizadas
    const float dpiScale = GetDpiScale(hWnd);
    const float baseFontSize = 14.0f * dpiScale;

    // Fonte principal
    ImFontConfig fontConfig;
    fontConfig.OversampleH = 1; // Reduzir oversampling para performance
    fontConfig.OversampleV = 1;
    fontConfig.PixelSnapH = true; // Melhor alinhamento de pixels
    
    io.Fonts->AddFontFromFileTTF("c:\\windows\\fonts\\arial.ttf", baseFontSize, &fontConfig);

    // Build atlas de fontes com compressão
    io.Fonts->Build();
    
    // Configurar flags de textura para melhor performance
    io.Fonts->TexDesiredWidth = 2048; // Tamanho otimizado do atlas
    
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
}

// Otimizado: Update overlay com sincroniza��o melhorada
void ov::update_overlay()
{
    if (!globals.OverlayWnd || !globals.TargetWnd)
        return;

    HWND hwnd_active = GetForegroundWindow();
    if (hwnd_active == globals.TargetWnd)
    {
        // Verificar se a janela do jogo mudou de tamanho/posi��o
        RECT tSize;
        if (GetWindowRect(globals.TargetWnd, &tSize))
        {
            int newWidth = tSize.right - tSize.left;
            int newHeight = tSize.bottom - tSize.top;

            // Atualizar tamanho da overlay se necess�rio
            if (newWidth != globals.Width || newHeight != globals.Height)
            {
                globals.Width = newWidth;
                globals.Height = newHeight;
                p_Params.BackBufferWidth = globals.Width;
                p_Params.BackBufferHeight = globals.Height;
                UpdateImguiDisplaySize();
                ResetDevice();
                SetWindowPos(globals.OverlayWnd, HWND_TOPMOST, tSize.left, tSize.top,
                    globals.Width, globals.Height, SWP_SHOWWINDOW);
            }
            else
            {
                // Apenas atualizar posi��o se necess�rio
                SetWindowPos(globals.OverlayWnd, HWND_TOPMOST, tSize.left, tSize.top,
                    0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
            }
        }
    }
}
// Otimizado: Render ultra melhorado com batching e culling
void ov::render()
{
    if (!p_Device)
        return;

    // Cache de estado da janela ativa
    static HWND lastActiveWindow = nullptr;
    static int inactiveFrames = 0;
    
    HWND currentActiveWindow = GetForegroundWindow();
    const bool isGameActive = (currentActiveWindow == globals.TargetWnd);
    
    // Otimização: Reduzir renderização quando jogo não está ativo
    if (!isGameActive && !showESPMenu)
    {
        inactiveFrames++;
        // Renderizar apenas a cada 5 frames quando inativo
        if (inactiveFrames % 5 != 0)
            return;
    }
    else
    {
        inactiveFrames = 0;
    }

    // Verificar estado do dispositivo
    HRESULT deviceStatus = p_Device->TestCooperativeLevel();
    if (deviceStatus == D3DERR_DEVICELOST)
        return;

    if (deviceStatus == D3DERR_DEVICENOTRESET)
    {
        ImGui_ImplDX9_InvalidateDeviceObjects();
        if (FAILED(p_Device->Reset(&p_Params)))
            return;
        ImGui_ImplDX9_CreateDeviceObjects();
    }

    // Iniciar novo frame ImGui
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Atualizar posição da overlay (otimizado - apenas quando necessário)
    static int updateCounter = 0;
    static bool windowSizeChanged = false;
    
    if (++updateCounter >= 15 || windowSizeChanged) // A cada 15 frames ou quando tamanho muda
    {
        update_overlay();
        updateCounter = 0;
        windowSizeChanged = false;
    }

    // Configurar janela principal do overlay
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

    constexpr ImGuiWindowFlags overlayFlags = 
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing;

    ImGui::Begin("##overlay_main", nullptr, overlayFlags);
    
    // Otimização: Usar clip rect para culling
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->PushClipRectFullScreen();

    // Renderizar ESP (sua função principal)
    draw_esp();
    
    // Renderizar menu moderno
    DrawMenuUI();

    drawList->PopClipRect();
    ImGui::End();
    
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(3);

    // Finalizar frame ImGui
    ImGui::EndFrame();
    ImGui::Render();

    // Configurar estados do dispositivo (cache estático)
    static bool renderStatesInitialized = false;
    if (!renderStatesInitialized)
    {
        // Estados de renderização otimizados
        p_Device->SetRenderState(D3DRS_ZENABLE, FALSE);
        p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        p_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        p_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        p_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        p_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
        p_Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
        p_Device->SetRenderState(D3DRS_FOGENABLE, FALSE);
        p_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        p_Device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
        p_Device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
        p_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        p_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
        
        // Otimizações de textura
        p_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        p_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        p_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        p_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        
        renderStatesInitialized = true;
    }

    // Limpar buffer com cor transparente
    p_Device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

    // Renderizar cena
    if (SUCCEEDED(p_Device->BeginScene()))
    {
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        p_Device->EndScene();
    }

    // Apresentar frame (sem VSync para máxima performance)
    HRESULT presentResult = p_Device->Present(nullptr, nullptr, nullptr, nullptr);

    // Tratamento de perda de dispositivo
    if (presentResult == D3DERR_DEVICELOST || presentResult == D3DERR_DEVICENOTRESET)
    {
        deviceStatus = p_Device->TestCooperativeLevel();
        if (deviceStatus == D3DERR_DEVICENOTRESET)
        {
            ImGui_ImplDX9_InvalidateDeviceObjects();
            if (SUCCEEDED(p_Device->Reset(&p_Params)))
            {
                ImGui_ImplDX9_CreateDeviceObjects();
                renderStatesInitialized = false; // Resetar estados
            }
        }
    }
}

// Otimizado: Loop principal ultra melhorado com controle adaptativo de FPS
WPARAM ov::loop()
{
    MSG msg = { 0 };
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    auto lastFpsUpdate = lastFrameTime;
    
    // Configurações de FPS adaptativo
    constexpr auto minFrameTime = std::chrono::microseconds(8333);  // ~120 FPS max
    constexpr auto targetFrameTime = std::chrono::microseconds(16666); // ~60 FPS
    constexpr auto maxFrameTime = std::chrono::milliseconds(100);   // Limite máximo
    
    int frameCount = 0;
    float currentFPS = 0.0f;
    bool isGameActive = true;

    while (true)
    {
        // Processar todas as mensagens pendentes (não bloqueante)
        while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                goto cleanup;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Verificar condições de saída
        if (GetAsyncKeyState(VK_F8) & 0x8000)
            break;

        if (!FindWindowA(NULL, globals.lTargetWindow))
            break;

        // Verificar se o jogo está ativo (otimização)
        HWND foregroundWnd = GetForegroundWindow();
        isGameActive = (foregroundWnd == globals.TargetWnd);

        // Controle de FPS ultra otimizado
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastFrameTime);

        // Limitar frame time máximo para evitar lag em caso de freeze
        if (elapsed > maxFrameTime)
            elapsed = targetFrameTime;

        // FPS adaptativo: mais rápido quando jogo está ativo
        auto currentTargetFrameTime = isGameActive ? minFrameTime : targetFrameTime;

        if (elapsed >= currentTargetFrameTime)
        {
            ov::render();
            lastFrameTime = currentTime;
            frameCount++;

            // Atualizar FPS a cada segundo
            auto fpsDuration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFpsUpdate);
            if (fpsDuration.count() >= 1000)
            {
                currentFPS = frameCount / (fpsDuration.count() / 1000.0f);
                frameCount = 0;
                lastFpsUpdate = currentTime;
            }

            // Sleep preciso apenas se necessário
            auto renderTime = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now() - currentTime
            );
            
            auto sleepTime = currentTargetFrameTime - renderTime;
            if (sleepTime.count() > 1000) // Apenas se > 1ms
            {
                // Sleep de alta precisão
                std::this_thread::sleep_for(sleepTime - std::chrono::microseconds(500));
            }
        }
        else
        {
            // Sleep mínimo quando não está na hora de renderizar
            if (!isGameActive)
            {
                // Quando jogo não está ativo, economizar mais CPU
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
            else
            {
                // Yield para outras threads
                std::this_thread::yield();
            }
        }
    }

cleanup:
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    clean_directx();
    if (globals.OverlayWnd)
        DestroyWindow(globals.OverlayWnd);

    return msg.wParam;
}

void ov::clean_directx()
{
    if (p_Device != NULL)
    {
        p_Device->EndScene();
        p_Device->Release();
        p_Device = NULL;
    }

    if (p_Object != NULL)
    {
        p_Object->Release();
        p_Object = NULL;
    }
}





