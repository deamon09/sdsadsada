#include "overlay/overlay.h"
#include "overlay/globals.h"
#include "memory_optimized.h"
#include "hck/ConfigSystem.h"
#include "hck/ModernMenu.h"

// Declaração da função do menu
void DrawMenuUI();

// Variável global para controle do menu
bool g_showMenu = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	switch (uMsg)
	{
	case WM_KEYDOWN:
		// INSERT para abrir/fechar menu
		if (wParam == VK_INSERT) {
			g_showMenu = !g_showMenu;
		}
		break;
		
	case WM_DESTROY:
	{
		ov::clean_directx();
		PostQuitMessage(0);
		return 0;
	}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int main() {
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleTitleA("DayZ ESP - Modern Edition");
	
	// Configurar estilo moderno do ImGui
	ModernMenu::SetupStyle();
	
	// Tentar carregar configuração padrão
	if (!g_configSystem.LoadConfig("default")) {
		// Se não existir, criar uma configuração padrão
		g_configSystem.SaveConfig("default");
	}
	
	// Inicializar sistema
	init();
	
	// Criar janela overlay
	ov::create_window();
	
	// Loop principal
	ov::loop();

	return 0;
}
