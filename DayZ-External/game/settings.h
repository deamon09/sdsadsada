//#pragma once
//
//void SaveConfig(const std::string& filename, int playerESPDistance, int infectedESPDistance, int carESPDistance, int animalESPDistance);
//
//
///*bones*/
//int num1 = 24;
//int num2 = 21;
//int num3 = 21;
//int num4 = 61;
//int num5 = 61;
//int num6 = 63;
//int num7 = 63;
//int num8 = 65;
//int num9 = 21;
//int num10 = 94;
//int num11 = 94;
//int num12 = 97;
//int num13 = 97;
//int num14 = 99;
//int num15 = 21;
//int num16 = 18;
//int num17 = 18;
//int num18 = 1;
//int num19 = 1;
//int num20 = 4;
//int num21 = 4;
//int num22 = 6;
//int num23 = 18;
//int num24 = 9;
//int num25 = 9;
//int num26 = 11;
//int num27 = 11;
//int num28 = 14;
//
//void SetCustomTabStyle() {
//    ImGuiStyle& style = ImGui::GetStyle();
//
//    // Ajustar padding e frame
//    style.FramePadding = ImVec2(5, 3); // Reduzir o padding dentro das abas
//    style.ItemSpacing = ImVec2(4, 4);  // Espaçamento entre itens
//
//    // Definir borda mais sutil e menor
//    style.TabBorderSize = 1.0f;
//    style.FrameBorderSize = 1.0f;
//
//    // Definir arredondamento das bordas das abas
//    style.TabRounding = 0.0f; // Remover o arredondamento das abas
//    style.WindowRounding = 0.0f; // Remover o arredondamento das janelas
//
//    // Definir as cores das abas
//    ImVec4 activeTabColor = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);   // Cor da aba ativa (cor escura)
//    ImVec4 inactiveTabColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // Cor da aba inativa (um pouco mais clara)
//    ImVec4 tabTextColor = ImVec4(0.4f, 0.4f, 1.0f, 1.0f);     // Cor do texto da aba ativa (azul)
//
//    // Cor das abas quando ativadas, inativas e quando estão com o mouse sobre elas
//    style.Colors[ImGuiCol_Tab] = inactiveTabColor;
//    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f); // Quando hover
//    style.Colors[ImGuiCol_TabActive] = activeTabColor;
//    style.Colors[ImGuiCol_TabUnfocused] = inactiveTabColor;
//    style.Colors[ImGuiCol_TabUnfocusedActive] = activeTabColor;
//
//    // Cor do texto
//    style.Colors[ImGuiCol_Text] = tabTextColor;
//}
//
//
//
//void SetCustomStyle() {
//    ImGuiStyle& style = ImGui::GetStyle();
//
//    style.Alpha = 1.0f;
//    //style.DisabledAlpha = 0.6000000238418579f;
//    style.WindowPadding = ImVec2(8.0f, 8.0f);
//    style.WindowRounding = 0.0f;
//    style.WindowBorderSize = 2.0f;
//    style.WindowMinSize = ImVec2(32.0f, 32.0f);
//    style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
//    //style.WindowMenuButtonPosition = ImGuiDir_Left;
//    style.ChildRounding = 0.0f;
//    style.ChildBorderSize = 2.0f;
//    style.PopupRounding = 2.0f;
//    style.PopupBorderSize = 1.0f;
//    style.FramePadding = ImVec2(5, 3);
//    style.FrameRounding = 0.0f;
//    style.FrameBorderSize = 1.0f;
//    style.ItemSpacing = ImVec2(4, 4);
//    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
//    //style.CellPadding = ImVec2(4.0f, 2.0f);
//    style.IndentSpacing = 30.0f;
//    style.ColumnsMinSpacing = 6.0f;
//    style.ScrollbarSize = 14.0f;
//    style.ScrollbarRounding = 0.0f;
//    style.GrabMinSize = 10.0f;
//    style.GrabRounding = 0.0f;
//    style.TabRounding = 0.0f;
//    style.TabBorderSize = 1.0f;
//    //style.TabMinWidthForCloseButton = 0.0f;
//    //style.ColorButtonPosition = ImGuiDir_Right;
//    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
//    //style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
//
//    style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
//    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
//    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);
//    style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
//    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.85f);
//    style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
//    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
//    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.01f, 1.00f);
//    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
//    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
//    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.83f);
//    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
//    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.87f);
//    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);
//    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
//    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.55f, 0.53f, 0.55f, 0.51f);
//    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
//    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.91f);
//    //style.Colors[ImGuiCol_ComboBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.99f);
//    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.83f);
//    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.70f, 0.70f, 0.70f, 0.62f);
//    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.30f, 0.30f, 0.30f, 0.84f);
//    style.Colors[ImGuiCol_Button] = ImVec4(0.48f, 0.72f, 0.89f, 0.49f);
//    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.69f, 0.99f, 0.68f);
//    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
//    style.Colors[ImGuiCol_Header] = ImVec4(0.30f, 0.69f, 1.00f, 0.53f);
//    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
//    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.62f, 0.83f, 1.00f);
//    style.Colors[ImGuiCol_Column] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
//    style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
//    style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
//    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
//    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
//    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
//    //style.Colors[ImGuiCol_CloseButton] = ImVec4(0.50f, 0.50f, 0.90f, 0.50f);
//    //style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.70f, 0.70f, 0.90f, 0.60f);
//    //style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
//    style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
//    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
//    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
//    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
//    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
//    style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
//}
///*floats*/
////float width = GetSystemMetrics(SM_CXSCREEN);
////float height = GetSystemMetrics(SM_CYSCREEN);
//static int playerESPDistance = 0; // Unique variable for Player ESP Distance
//static int infectedESPDistance = 0; // Unique variable for Infected ESP Distance
//static int carESPDistance = 0; // Unique variable for Car ESP Distance
//static int animalESPDistance = 0; // Unique variable for Animal ESP Distance
//static std::string selectedConfig;
///*entitys esp*/
//bool enablePlayerESP = true;
//bool enableInfectedESP = true;
//bool enableCarESP = true;
//bool enableAnimalESP = true;
////bool render_skeleton = true;
//bool zoomActive = false;
//bool Playerbox = false;
//bool Playername = true;
//bool playerDistance = false;
//bool PlayerSkeleton = true;
//
//bool removeGrass = false;
//ImVec4 playerNameColor = ImVec4(0, 1, 0, 1);      // Cor inicial para o nome do jogador
//ImVec4 playerDistanceColor = ImVec4(1, 1, 0, 1);   // Cor inicial para a distância
//ImVec4 playerBoxColor = ImVec4(1, 0, 0, 1);        // Cor inicial para a caixa do jogador
//ImVec4 infectedColor = ImVec4(1, 0, 0, 1);         // Cor inicial para infectados
//ImVec4 carColor = ImVec4(0, 0, 1, 1);               // Cor inicial para carros
//ImVec4 animalColor = ImVec4(0, 0.39f, 0, 1);       // Cor inicial para animais
//
//float aimFOVvalue = 100.0f;
//bool drawfov = false;
//bool enablesilentAim = false;
//float aimFOV = 100.0f;
//
//
//bool Changefov = false;
//float Horizontal = 1.333;
//float Vertical = 0.75;
//
//int selectedKey = 0; // Armazena o código da tecla escolhida
//bool waitingForKey = false;
//
//std::string keyMode = "Press";
//std::string KeyToString(int key) {
//    if (key == 0) return "Auto Keybind"; // Nome inicial
//    // Adicione aqui os códigos de tecla que você quer tratar
//    if (key == VK_LBUTTON) return "Left Mouse Button";
//    if (key == VK_RBUTTON) return "Right Mouse Button";
//    if (key == VK_MBUTTON) return "Middle Mouse Button";
//    if (key >= 'A' && key <= 'Z') return std::string(1, (char)key); // Letras de A a Z
//    if (key >= '0' && key <= '9') return std::string(1, (char)key); // Números de 0 a 9
//    // Outros mapeamentos de teclas podem ser adicionados aqui
//    return "Unknown Key";
//}
//
//float INSVeh[] = { 255, 0, 255, 255.f };
//float INSVeh1[] = { 255, 122, 31, 255.f };
//float INSVeh2[] = { 255, 0, 0, 255.f };
//
//float DistanceItems = 150;
//
//
//bool Weapon_Or_Ammo_Esp = true;
//bool QualityItems = true;
//bool Items = true;
//bool Barrel_Or_Wooden_Chest_Esp = true;
//bool Car_Esp = true;
//bool Building_Esp = true;
//bool Food_Medical_Esp = true;
//
//
//
