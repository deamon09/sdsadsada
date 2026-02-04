#pragma once
#include "vector.h"
#include "memory.h"
#include <string>
#include "offsets.h"
#include <d3dx9math.h>
#include "../driver/driver.h"
#include "xorstr.h"
//#include "../hack/vars.h" 
#include "../Imgui/imgui.h"
#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>
// Defini��o da macro ISVALID
#define ISVALID(x) ((x) != 0 && (x) != 0xFFFFFFFFFFFFFFFFULL)





void DrawBox(float x, float y, float width, float height, ImColor color, float thickness) {
	ImGui::GetWindowDrawList()->AddRect(ImVec2(x, y), ImVec2(x + width, y + height), color, 0.0f, 0, thickness);
}

using namespace std;

// Otimizado: Variável thread_local para cache
thread_local float CameraSpeed = 1.0f;

// Otimizado: Função inline para validação rápida de ponteiros
inline bool IsValidPtr2(void* pointer) noexcept
{
	if (!pointer)
		return false;

	const uintptr_t ptr = reinterpret_cast<uintptr_t>(pointer);
	return ptr >= 0x1000 && ptr <= 0x7FFFFFFFFFFF;
}

namespace Game {
	float GetLife ( uintptr_t entity );
}


namespace Game
{

	uintptr_t signature(const std::string& signature) {
		uintptr_t moduleAdressmm = 0;
		moduleAdressmm = (uintptr_t)GetModuleHandleA(NULL);

		static auto patternToByteZmm = [](const char* pattern) {
			auto       bytesmm = std::vector<int>{};
			const auto startmm = const_cast<char*>(pattern);
			const auto endmm = const_cast<char*>(pattern) + strlen(pattern);

			for (auto currentmm = startmm; currentmm < endmm; ++currentmm) {
				if (*currentmm == '?') {
					++currentmm;
					if (*currentmm == '?')
						++currentmm;
					bytesmm.push_back(-1);
				}
				else { bytesmm.push_back(strtoul(currentmm, &currentmm, 16)); }
			}
			return bytesmm;
			};

		const auto dosHeadermm = (PIMAGE_DOS_HEADER)moduleAdressmm;
		const auto ntHeadersmm = (PIMAGE_NT_HEADERS)((std::uint8_t*)moduleAdressmm + dosHeadermm->e_lfanew);

		const auto sizeOfImage = ntHeadersmm->OptionalHeader.SizeOfImage;
		auto       patternBytesmm = patternToByteZmm(signature.c_str());
		const auto scanBytesmm = reinterpret_cast<std::uint8_t*>(moduleAdressmm);

		const auto smm = patternBytesmm.size();
		const auto dmm = patternBytesmm.data();

		for (auto imm = 0ul; imm < sizeOfImage - smm; ++imm) {
			bool foundmm = true;
			for (auto jmm = 0ul; jmm < smm; ++jmm) {
				if (scanBytesmm[imm + jmm] != dmm[jmm] && dmm[jmm] != -1) {
					foundmm = false;
					break;
				}
			}
			if (foundmm) { return reinterpret_cast<uintptr_t>(&scanBytesmm[imm]); }
		}
		return NULL;
	}

	std::uint8_t* PatternScan(const char* signature, uint64_t start)
	{
		uintptr_t module = (uintptr_t)GetModuleHandleA(NULL);

		static auto pattern_to_byte = [](const char* pattern) {
			auto bytes = std::vector<int>{};
			auto start = const_cast<char*>(pattern);
			auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current) {
				if (*current == '?') {
					++current;
					if (*current == '?')
						++current;
					bytes.push_back(-1);
				}
				else {
					bytes.push_back(strtoul(current, &current, 16));
				}
			}
			return bytes;
			};

		auto dosHeader = (PIMAGE_DOS_HEADER)module;
		auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

		auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto patternBytes = pattern_to_byte(signature);

		unsigned long startDiff = 0;

		if (start > (uint64_t)module)
			start -= (uint64_t)module;

		auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

		auto s = patternBytes.size();
		auto d = patternBytes.data();

		for (auto i = (unsigned long)start; i < sizeOfImage - s; ++i) {
			bool found = true;
			for (auto j = 0ul; j < s; ++j) {
				if (scanBytes[i + j] != d[j] && d[j] != -1) {
					found = false;
					break;
				}
			}
			if (found) {
				return &scanBytes[i];
			}
		}
		return nullptr;
	}

	//uint32_t get_bone_index_by_name(const char* boneName) 
	//{
	//	typedef uint32_t(__fastcall* fnGetBoneIndexByName)(void*, const char* boneName); 
	//	static const fnGetBoneIndexByName getBoneIndexByName = (fnGetBoneIndexByName)EnfClassHolder(_("Human")).GetFunctionByName(_("GetBoneIndexByName")); 

	//	if (!getBoneIndexByName)
	//	{
	//		g_console.log(_("\n[get_bone_index]: returns null!"));
	//		return 0;
	//	}

	//	return getBoneIndexByName(this, boneName);
	//}

	class USQAnimInstanceSoldier1P {
	public:
	};
	
	// Otimizado: Funções inline para acesso rápido
	inline uint64_t NearEntityTable() noexcept
	{
		return read<uint64_t>(globals.World + WorldOffsets::OFF_NEAR_ENTITY_TABLE);
	}

	inline uint32_t NearEntityTableSize() noexcept
	{
		return read<uint32_t>(globals.World + WorldOffsets::OFF_NearTableSize);
	}

	inline uint64_t GetEntity(uint64_t PlayerList, uint64_t SelectedPlayer) noexcept
	{
		// Otimizado: Cálculo direto sem multiplicação desnecessária
		return read<uint64_t>(PlayerList + (SelectedPlayer << 3));
	}

	// Otimizado: Função inline com validações robustas para evitar crashes
	inline std::string GetEntityTypeName(uint64_t Entity) noexcept
	{
		// L� o ponteiro para o tipo da entidade
		if (!IsValidPtr2(reinterpret_cast<void*>(Entity)))
			return "";

		// Lê o ponteiro para o tipo da entidade
		const uint64_t entityTypePtr = read<uint64_t>(Entity + EntityOffsets::OFF_EntityTypePtr);
		if (!IsValidPtr2(reinterpret_cast<void*>(entityTypePtr)))
			return "";

		// L� o ponteiro para o nome do tipo (dentro do tipo da entidade)
		// Lê o ponteiro para o nome do tipo (dentro do tipo da entidade)
		const uint64_t entityNamePtr = read<uint64_t>(entityTypePtr + EntityOffsets::OFF_EntityTypeName);
		if (!IsValidPtr2(reinterpret_cast<void*>(entityNamePtr)))
			return "";

		// L� e retorna a string do nome
		// Lê e retorna a string do nome
		return ReadArmaString(entityNamePtr);
	}



	// Otimizado: Funções inline
	inline uint64_t FarEntityTable() noexcept
	{
		return read<uint64_t>(globals.World + WorldOffsets::OFF_FAR_ENTITY_TABLE);
	}

	inline uint32_t FarEntityTableSize() noexcept
	{
		return read<uint32_t>(globals.World + WorldOffsets::OFF_FarTableSize);
	}

	inline uint64_t GetCameraOn() noexcept
	{
		return read<uint64_t>(globals.World + LocalPlayerOffsets::OFF_LocalPlayer1);
	}

	// Otimizado: Cache de cameraOn
	inline uint64_t GetLocalPlayer() noexcept
	{
		const uint64_t cameraOn = read<uint64_t>(globals.World + CameraOffsets::OFF_CameraOn);
		return read<uint64_t>(cameraOn + 0x8) - EntityOffsets::OFF_EntityTypeName;
	}

	// Otimizado: Função inline
	inline uint64_t GetInventory(uint64_t Entity) noexcept {
		return read<uint64_t>(Entity + InventoryOffsets::OFF_Inventory);
	}

	// Otimizado: Cache de valores intermediários
	inline string GetItemInHands(uint64_t Entity) {
		const uint64_t inventory = GetInventory(Entity);
		const uint64_t inhands = read<uint64_t>(inventory + PlayerOffsets::OFF_Inhands);
		const uint64_t entityType = read<uint64_t>(inhands + EntityOffsets::OFF_EntityTypePtr);
		const uint64_t realName = read<uint64_t>(entityType + EntityOffsets::OFF_RealName);
		return ReadArmaString(realName);
	}
#define OFF_ENTITY_TYPE 0x180
#define OFF_ENTITY_TYPENAME 0xA8

	// Otimizado: Cache de ponteiros intermediários
	inline std::string GetItemName(uint64_t Entity) {
		const uint64_t entityType = read<uint64_t>(Entity + EntityOffsets::OFF_EntityTypePtr);
		const uint64_t typeName = read<uint64_t>(entityType + EntityOffsets::OFF_EntityTypeName);
		return ReadArmaString(typeName);
	}

	// Otimizado: Cache de ponteiros
	inline std::string GetEntityName(uint64_t Entity)
	{
		const uint64_t ptr1 = read<uint64_t>(Entity + 0x148);
		const uint64_t namePtr = read<uint64_t>(ptr1 + 0x68);
		return ReadArmaString(namePtr);
	}



	// Otimizado: Switch case mais eficiente e função inline
	inline string GetQuality(uint64_t Entity) noexcept {
		const uint64_t quality = read<uint64_t>(Entity + ItemOffsets::off_itemquality);

		switch (quality) {
			case 1: return "Quality (Worn)";
			case 2: return "Quality(Damaged)";
			case 3: return "Quality (Badly damaged)";
			case 4: return "Quality (Ruined)";
			default: return "Quality (Pristine)";
		}
	}



	// Otimizado: Funções inline
	inline uint64_t GetNetworkId(uint64_t Entity) noexcept {
		return read<uint64_t>(Entity + NetworkOffsets::off_entity_networkid);
	}

	inline uint64_t GetLocalPlayerVisualState() noexcept
	{
		return read<uint64_t>(GetLocalPlayer() + EntityOffsets::OFF_VisualState);
	}

	// Otimizado: Cache de visual state
	inline Vector3 GetCoordinate(uint64_t Entity) noexcept {
		const uintptr_t visualState = read<uintptr_t>(Entity + EntityOffsets::OFF_VisualState);
		return Vector3(read<Vector3>(visualState + EntityOffsets::OFF_GetCoordinate));
	}
	// Otimizado: Removido loop infinito desnecessário, função inline
	inline uint64_t GetCamera() noexcept
	{
		return read<uint64_t>(globals.World + CameraOffsets::OFF_Camera);
	}

	// Otimizado: Funções inline usando offsets
	inline void SetTerrainGrid(float value) noexcept {
		write<float>(globals.World + MiscOffsets::OFF_grass, value);
	}

	inline void SetEyeAccom(float value) noexcept {
		write<float>(globals.World + MiscOffsets::OFF_EyeAccom, value);
	}

	inline void SetClariddade(float value) noexcept {
		write<float>(globals.World + 0x2978, value);
	}


	// Otimizado: Funções inline e cache de camera para evitar múltiplas leituras
	inline Vector3 GetInvertedViewTranslation() noexcept
	{
		const uint64_t camera = GetCamera();
		return Vector3(read<Vector3>(camera + EntityOffsets::OFF_GetCoordinate));
	}
	
	inline Vector3 GetInvertedViewRight() noexcept
	{
		const uint64_t camera = GetCamera();
		return Vector3(read<Vector3>(camera + 0x8));
	}
	
	inline Vector3 GetInvertedViewUp() noexcept
	{
		const uint64_t camera = GetCamera();
		return Vector3(read<Vector3>(camera + 0x14));
	}
	
	inline Vector3 GetInvertedViewForward() noexcept
	{
		const uint64_t camera = GetCamera();
		return Vector3(read<Vector3>(camera + 0x20));
	}

	inline Vector3 GetViewportSize() noexcept
	{
		const uint64_t camera = GetCamera();
		return Vector3(read<Vector3>(camera + 0x58));
	}

	inline Vector3 GetProjectionD1() noexcept
	{
		const uint64_t camera = GetCamera();
		return Vector3(read<Vector3>(camera + 0xD0));
	}

	inline Vector3 GetProjectionD2() noexcept
	{
		const uint64_t camera = GetCamera();
		return Vector3(read<Vector3>(camera + 0xDC));
	}

	// Otimizado: Funções inline
	inline uint64_t SlowTable() noexcept
	{
		return read<uint64_t>(globals.World + WorldOffsets::OFF_SLOW_ENTITY_TABLE);
	}

	inline uint32_t SlowTableSize() noexcept
	{
		return read<uint32_t>(globals.World + WorldOffsets::OFF_SlowTableSize);
	}

	// Otimizado: Cache de local player
	inline float GetDistanceToMe(const Vector3& Entity) noexcept
	{
		static thread_local uint64_t cachedLocalPlayer = 0;
		if (!cachedLocalPlayer) {
			cachedLocalPlayer = GetLocalPlayer();
		}
		return Entity.Distance(GetCoordinate(cachedLocalPlayer));
	}

	// Otimizado: Função inline
	inline bool is_dead(uint64_t entityPtr) noexcept {
		return read<uint8_t>(entityPtr + PlayerOffsets::OFF_playerIsDead) == 1;
	}



	// Otimizado: Funções inline
	inline uint64_t ItemTable() noexcept
	{
		return read<uint64_t>(globals.World + WorldOffsets::OFF_ItemTable);
	}

	inline uint32_t ItemTableSize() noexcept
	{
		return read<uint32_t>(globals.World + WorldOffsets::OFF_ItemTableSize);
	}

	// Otimizado: Cache de visual state
	inline Vector3 GetItemCoordinate(uint64_t Item) noexcept
	{
		const uint64_t visualState = read<uint64_t>(Item + EntityOffsets::OFF_VisualState);
		return Vector3(read<Vector3>(visualState + EntityOffsets::OFF_GetCoordinate));
	}

	struct BonePair {
		int boneId1;
		int boneId2;

		BonePair(int id1, int id2) : boneId1(id1), boneId2(id2) {}
	};


	struct BonePosition {
		Vector3 position;
		int index;

		BonePosition(const Vector3& pos, int idx) : position(pos), index(idx) {}
	};









	// Otimizado: Cache de ponteiros e funções inline
	inline std::string GetItemTypeName(uint64_t Item)
	{
		const uint64_t name_ptr = read<uint64_t>(Item + EntityOffsets::OFF_EntityTypePtr);
		const uint64_t real_name_ptr = read<uint64_t>(name_ptr + EntityOffsets::OFF_RealName);
		return ReadArmaString(real_name_ptr);
	}

	// Otimizado: Removida duplicação - usa GetItemTypeName
	inline std::string GetItemRealName(uint64_t Item)
	{
		return GetItemTypeName(Item);
	}

	// Otimizado: Função inline com validação otimizada
	inline uint64_t GeVisualState(uint64_t Entity) noexcept
	{
		if (!IsValidPtr2(reinterpret_cast<void*>(Entity)))
			return 0;

		return read<uint64_t>(Entity + EntityOffsets::OFF_VisualState);
	}

	// Otimizado: Cache de ponteiros
	inline std::string GetEntityRealName(uint64_t Entity)
	{
		const uint64_t name_ptr = read<uint64_t>(Entity + EntityOffsets::OFF_EntityTypePtr);
		const uint64_t real_name_ptr = read<uint64_t>(name_ptr + EntityOffsets::OFF_RealName);
		return ReadArmaString(real_name_ptr);
	}

	// Otimizado: Função inline
	inline std::string GetPlayerName(uint64_t Player)
	{
		const uint64_t name_ptr = read<uint64_t>(Player + PlayerOffsets::OFF_PlayerName);
		return ReadArmaString(name_ptr);
	}


	// Otimizado: Função inline
	inline Vector3 ConvertToVector3(const D3DXVECTOR3& dxVec) noexcept {
		return Vector3(dxVec.x, dxVec.y, dxVec.z);
	}

	struct matrix4x4
	{
		float m[12];
	};

	// Otimizado: Alias para GetDistanceToMe (remover duplicação)
	inline float GetDistanceToMe1(const Vector3& Entity) noexcept
	{
		return GetDistanceToMe(Entity);
	}
	BOOL GetBonePosition(DWORD64 skeleton, DWORD64 visual, DWORD index, Vector3* out)
	{
		// Obt�m a classe de anima��o a partir do esqueleto
		uint64_t animClass = read<uint64_t>(skeleton + MiscOffsets::OFF_AnimClass);
		if (!IsValidPtr2(reinterpret_cast<void*>(animClass)))
			return FALSE;

		// Obt�m a classe de matriz da anima��o
		uint64_t matrixClass = read<uint64_t>(animClass + MiscOffsets::OFF_MatrixClass);
		if (!IsValidPtr2(reinterpret_cast<void*>(matrixClass)))
			return FALSE;

		// L� a matriz de transforma��o
		matrix4x4 matrix_a = read<matrix4x4>(visual + 0x8);

		// L� a posi��o do osso (index refere-se ao osso desejado)
		Vector3 matrix_b = read<Vector3>(matrixClass + MiscOffsets::OFF_Matrixb + index * sizeof(matrix4x4));

		// Calcula a posi��o final do osso baseado na matriz de transforma��o
		out->x = (matrix_a.m[0] * matrix_b.x) + (matrix_a.m[3] * matrix_b.y) + (matrix_a.m[6] * matrix_b.z) + matrix_a.m[9];
		out->y = (matrix_a.m[1] * matrix_b.x) + (matrix_a.m[4] * matrix_b.y) + (matrix_a.m[7] * matrix_b.z) + matrix_a.m[10];
		out->z = (matrix_a.m[2] * matrix_b.x) + (matrix_a.m[5] * matrix_b.y) + (matrix_a.m[8] * matrix_b.z) + matrix_a.m[11];

		return TRUE;
	}




	// Otimizado: Switch case e função inline
	inline uint64_t GetSkeleton(uint64_t Entity, int s2) noexcept {
		switch (s2) {
			case 1: return read<uint64_t>(Entity + PlayerOffsets::OFF_PlayerSkeleton);
			case 2: return read<uint64_t>(Entity + PlayerOffsets::OFF_ZmSkeleton);
			default: return 0;
		}
	}

	// Otimizado: Cache de camera e cálculos otimizados
	inline bool WorldToScreen(const Vector3& Position, Vector3& output) noexcept
	{
		const uint64_t camera = GetCamera();
		if (!camera) return false;

		// Cache de todos os valores da camera de uma vez
		const Vector3 viewTrans = Vector3(read<Vector3>(camera + EntityOffsets::OFF_GetCoordinate));
		const Vector3 viewRight = Vector3(read<Vector3>(camera + 0x8));
		const Vector3 viewUp = Vector3(read<Vector3>(camera + 0x14));
		const Vector3 viewForward = Vector3(read<Vector3>(camera + 0x20));
		const Vector3 viewportSize = Vector3(read<Vector3>(camera + 0x58));
		const Vector3 projD1 = Vector3(read<Vector3>(camera + 0xD0));
		const Vector3 projD2 = Vector3(read<Vector3>(camera + 0xDC));

		const Vector3 temp = Position - viewTrans;

		const float x = temp.Dot(viewRight);
		const float y = temp.Dot(viewUp);
		const float z = temp.Dot(viewForward);

		if (z < 0.1f)
			return false;

		// Otimizado: Cálculo direto sem criar Vector3 intermediário
		output.x = viewportSize.x * (1.0f + (x / projD1.x / z));
		output.y = viewportSize.y * (1.0f - (y / projD2.y / z));
		output.z = z;
		return true;
	}

	uint64_t ModuleBase = NULL;


	// Otimizado: Funções inline e cache de valores
	inline uint64_t GetNetworkManager() noexcept {
		return read<uint64_t>(ModuleBase + NetworkOffsets::off_networkmanager);
	}
	
	inline uint64_t GetNetworkClient() noexcept {
		const uint64_t networkManager = GetNetworkManager();
		return read<uint64_t>(networkManager + NetworkOffsets::off_networkmanager_networkclient);
	}

	inline uint64_t GetNetworkClientScoreBoard() noexcept {
		const uint64_t networkClient = GetNetworkClient();
		return read<uint64_t>(networkClient + NetworkOffsets::off_networkclient_playeridentity_size);
	}

	inline uint64_t GetCountOfPlayers() noexcept {
		const uint64_t scoreBoard = GetNetworkClientScoreBoard();
		return read<uint64_t>(scoreBoard + NetworkOffsets::off_entity_networkid);
	}

	inline string GetNetworkClientServerName() {
		const uint64_t networkClient = GetNetworkClient();
		const uint64_t serverNamePtr = read<uint64_t>(networkClient + 0x360);
		return ReadArmaString(serverNamePtr);
	}



	// Otimizado: Função inline unificada com IsValidPtr2
	inline bool IsValidPtr(void* pointer) noexcept
	{
		return IsValidPtr2(pointer);
	}

	// Otimizado: Usar offsets e função inline
	inline float get_health() noexcept {
		const uintptr_t the_class = read<uintptr_t>(PlayerOffsets::OFF_PlayerHealthStruct);
		if (!IsValidPtr(reinterpret_cast<void*>(the_class)))
			return 100.0f;

		return read<float>(the_class + PlayerOffsets::OFF_PlayerHealthValue);
	}
	// Otimizado: Função inline com cálculos otimizados
	inline Vector3 get_bone_pos(uint64_t matrixClass, const matrix4x4& matrix_a, int index) noexcept
	{
		const Vector3 matrix_b = read<Vector3>(matrixClass + MiscOffsets::OFF_Matrixb + index * sizeof(matrix4x4));

		// Otimizado: Cálculo direto sem variáveis temporárias
		return Vector3(
			matrix_a.m[0] * matrix_b.x + matrix_a.m[3] * matrix_b.y + matrix_a.m[6] * matrix_b.z + matrix_a.m[9],
			matrix_a.m[1] * matrix_b.x + matrix_a.m[4] * matrix_b.y + matrix_a.m[7] * matrix_b.z + matrix_a.m[10],
			matrix_a.m[2] * matrix_b.x + matrix_a.m[5] * matrix_b.y + matrix_a.m[8] * matrix_b.z + matrix_a.m[11]
		);
	}


	// Otimizado: Função inline com validação otimizada
	inline Vector3 GetObjectVisualState(uintptr_t entity) noexcept
	{
		if (!entity)
			return Vector3(-1, -1, -1);

		const uintptr_t renderVisualState = read<uintptr_t>(entity + EntityOffsets::OFF_VisualState);
		if (!IsValidPtr(reinterpret_cast<void*>(renderVisualState)))
			return Vector3(-1, -1, -1);

		return read<Vector3>(renderVisualState + EntityOffsets::OFF_GetCoordinate);
	}

	Vector3 GetHeadPosition()
	{
		return read<Vector3>((const DWORD64)+0x820);

	}

	Vector3 getChestPos(uintptr_t ent)
	{

		uint64_t animClass = read<uint64_t>(GetSkeleton(ent, 1) + MiscOffsets::OFF_AnimClass);
		if (!IsValidPtr2(reinterpret_cast<void*>(animClass))) {
			return Vector3{ 0.0f, 0.0f, 0.0f };
		}


		uint64_t matrixClass = read<uint64_t>(animClass + MiscOffsets::OFF_MatrixClass);
		if (!IsValidPtr2(reinterpret_cast<void*>(matrixClass))) {
			return Vector3{ 0.0f, 0.0f, 0.0f };
		}


		matrix4x4 matrix_a = read<matrix4x4>(Game::GeVisualState(ent) + 0x8);


		Vector3 pos = get_bone_pos(matrixClass, matrix_a, 18);  //peito

		return pos;
	}

	Vector3 getHeadPos(uintptr_t ent)
	{

		uint64_t animClass = read<uint64_t>(GetSkeleton(ent, 1) + MiscOffsets::OFF_AnimClass);
		if (!IsValidPtr2(reinterpret_cast<void*>(animClass))) {
			return Vector3{ 0.0f, 0.0f, 0.0f };
		}


		uint64_t matrixClass = read<uint64_t>(animClass + MiscOffsets::OFF_MatrixClass);
		if (!IsValidPtr2(reinterpret_cast<void*>(matrixClass))) {
			return Vector3{ 0.0f, 0.0f, 0.0f };
		}


		matrix4x4 matrix_a = read<matrix4x4>(Game::GeVisualState(ent) + 0x8);


		Vector3 pos = get_bone_pos(matrixClass, matrix_a, 24);  // cabe�a

		return pos;
	}

	// Otimizado: Função inline
	inline BOOL SetPosition(UINT64 Entity, const Vector3& TargetPos) noexcept
	{
		if (!Entity) return FALSE;
		
		const uint64_t visualState = read<UINT64>(Entity + EntityOffsets::OFF_VisualState);
		write<Vector3>(visualState + EntityOffsets::OFF_GetCoordinate, TargetPos);
		return TRUE;
	}

	// Otimizado: Usar offset correto
	inline uint64_t BulletTable() noexcept {
		return read<uint64_t>(globals.World + MiscOffsets::OFF_Bullets);
	}

	// Otimizado: Usar offset correto
	inline uint32_t BulletTableSize() noexcept {
		return read<uint32_t>(globals.World + MiscOffsets::OFF_Bulletsize);
	}



	DWORD64 getNetworkClient() {
		DWORD64 networkManager = read<DWORD64>(NetworkOffsets::off_networkmanager);  // Leitura usando a fun��o read
		if (networkManager) {
			DWORD64 networkClient = read<DWORD64>(networkManager + NetworkOffsets::off_networkmanager_networkclient);  // Leitura do network client
			return networkClient;
		}
		return 0;
	}



}

