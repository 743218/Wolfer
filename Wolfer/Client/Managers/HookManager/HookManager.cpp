#include "HookManager.h"

#include "Hooks/Game/ClientInstanceUpdateHook.h"
#include "Hooks/Game/SendChatMessageHook.h"
#include "Hooks/Input/KeyMapHook.h"
#include "Hooks/Input/KeyMouseHook.h"
#include "Hooks/Render/GetCameraComponentHook.h"
#include "Hooks/Render/GetFovHook.h"
#include "Hooks/Render/GetGammaHook.h"
#include "Hooks/Render/GetViewPerspectiveHook.h"
#include "Hooks/Render/HurtCamHook.h"
#include "Hooks/Render/RenderHitSelectHook.h"
#include "Hooks/Render/RenderLevelHook.h"
#include "Hooks/Render/SetUpAndRenderHook.h"
#include "Hooks/Network/SendPacketHook.h"
#include "Hooks/World/Block/GetBlockHook.h"
#include "Hooks/World/Entity/ActorLerpMotionHook.h"
#include "Hooks/World/Entity/ActorNormalTickHook.h"
#include "Hooks/World/Entity/ActorIsOnFireHook.h"
#include "Hooks/World/Entity/ActorSwingHook.h"
#include "Hooks/World/Entity/ActorSetRotHook.h"
#include "Hooks/World/Entity/MobSwingDurationHook.h"
#include "Hooks/World/Entity/PlayerSlowDownHook.h"
#include "Hooks/World/GameMode/StartDestroyBlockHook.h"
#include "Hooks/World/GameMode/StopDestroyBlockHook.h"
#include "Hooks/World/GameMode/GetPickRangeHook.h"
#include "Hooks/World/Level/LevelTickHook.h"
#include "Hooks/DirectX/PresentHook.h"
#include "Hooks/DirectX/ResizeBuffersHook.h"
#include "Hooks/Game/ContainerScreenHook.h"
#include <kiero.h>

class AttackHook : public FuncHook {
private:
	using func_t = bool(__thiscall*)(GameMode*, Actor*);
	static inline func_t oFunc = nullptr;

	static bool callback(GameMode* _this, Actor* _actor) {
		if (!_this || !_actor) return false; 

		bool cancel = false;
		ModuleManager::onAttack(_actor, cancel);
		if (cancel) return false;

		if (oFunc)
			return oFunc(_this, _actor);
		return false;
	}
public:
	AttackHook() {
		OriginFunc = reinterpret_cast<void*>(&oFunc);
		func = reinterpret_cast<void*>(&callback);
	}

	static void setOriginal(func_t orig) {
		oFunc = orig;
	}
};

class ConnectionRequestCreateHook : public FuncHook {
private:
	using func_t = void(__fastcall*)(
		__int64*, __int64, __int64, __int64, __int64, __int64, __int64,
		__int64, __int64, __int64, std::string*, int, int, int, __int64,
		char, char, __int64, int, std::string*, std::string*, bool,
		__int64, __int64, __int64, char);

	static inline func_t oFunc;

	static std::string GenerateRandomDeviceID() {
		static const char charset[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";

		const size_t maxSize = 102 * 102;
		std::string result;
		result.reserve(maxSize);

		for (size_t i = 0; i < maxSize; ++i) {
			result += charset[rand() % (sizeof(charset) - 1)];
		}

		return result;
	}

	static void callback(
		__int64* a1, __int64 a2, __int64 a3, __int64 a4, __int64 a5,
		__int64 a6, __int64 a7, __int64 a8, __int64 a9, __int64 a10,
		std::string* deviceId, int a12, int a13, int a14, __int64 a15,
		char a16, char a17, __int64 a18, int a19,
		std::string* platformUserId, std::string* thirdPartyName,
		bool thirdPartyNameOnly, __int64 a23, __int64 a24,
		__int64 a25, char a26)
	{
		if (deviceId && ModuleManager::getModule<DeviceIDSpoofer>()->isEnabled()) {
			*deviceId = GenerateRandomDeviceID();
		}

		oFunc(
			a1, a2, a3, a4, a5, a6, a7, a8, a9, a10,
			deviceId, a12, a13, a14, a15, a16, a17, a18, a19,
			platformUserId, thirdPartyName, thirdPartyNameOnly,
			a23, a24, a25, a26);
	}

public:
	ConnectionRequestCreateHook() {
		OriginFunc = (void**)&oFunc;
		func = (void*)&callback;
	}
};

void HookManager::init() {
	MH_Initialize();

	RequestHook<ClientInstanceUpdateHook>(Addresses::ClientInstance_update);
	RequestHook<SendChatMessageHook>(Addresses::ClientInstanceScreenModel_sendChatMessage);
	RequestHook<KeyMapHook>(Addresses::KeyPressFunc);
	RequestHook<KeyMouseHook>(Addresses::KeyMouseFunc);
	RequestHook<GetCameraComponentHook>(Addresses::GetCameraComponent);
	RequestHook<GetFovHook>(Addresses::LevelRendererPlayer_getFov);
	RequestHook<GetGammaHook>(Addresses::Options_getGamma);
	RequestHook<GetViewPerspectiveHook>(Addresses::Options_getViewPerspective);
	RequestHook<HurtCamHook>(Addresses::CauseHurtCamFunc);
	RequestHook<RenderHitSelectHook>(Addresses::LevelRendererPlayer_renderHitSelect);
	RequestHook<RenderLevelHook>(Addresses::LevelRenderer_renderLevel);
	RequestHook<SetUpAndRenderHook>(Addresses::ScreenView_setUpAndRender);
	uintptr_t connReqCreateAddr = Memory::findSig("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 B4 24 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 8B D9 48 89 55");
	RequestHook<ConnectionRequestCreateHook>(connReqCreateAddr);

	uintptr_t containerScreenTickAddr = Memory::findSig("48 8B C4 48 89 58 ?? 48 89 68 ?? 48 89 70 ?? 57 41 56 41 57 48 81 EC ?? ?? ?? ?? 0F 29 70 D8 4C");
	RequestHook<ContainerScreenTickHook>(containerScreenTickAddr);

	{
		// Network Hook
		uintptr_t** PacketSenderVTable = (uintptr_t**)Addresses::PacketSenderVTable;
		RequestHook<SendPacketHook>(PacketSenderVTable, 1);
	}

	{
		// Block Hook
		uintptr_t** BlockSourceVTable = (uintptr_t**)Addresses::BlockSourceVTable;
		RequestHook<GetBlockHook>(BlockSourceVTable, 2);
	}

	{
		// Entity Hook
		uintptr_t** PlayerVTable = (uintptr_t**)Addresses::PlayerVTable;
		RequestHook<ActorLerpMotionHook>(PlayerVTable, 24);
		RequestHook<ActorNormalTickHook>(PlayerVTable, 26);
		RequestHook<ActorIsOnFireHook>(PlayerVTable, 51);
		RequestHook<ActorSwingHook>(PlayerVTable, 117);
		RequestHook<ActorSetRotHook>("4C 8B DC 49 89 73 10 49 89 7B 20 55 49 8D 6B A1 48 81 EC ? ? ? ? 48 8B");
		RequestHook<MobSwingDurationHook>("48 89 5C 24 ? 57 48 83 EC 20 48 8B 15 ? ? ? ? 48 8B F9 33 DB");
		RequestHook<PlayerSlowDownHook>("40 53 48 83 EC 30 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 48 8B DA 4D 85 C0 74 5D");
	}

	{
		// GameMode Hook
		uintptr_t** GameModeVTable = (uintptr_t**)Addresses::GameModeVTable;
		RequestHook<StartDestroyBlockHook>(GameModeVTable, 1);
		RequestHook<StopDestroyBlockHook>(GameModeVTable, 4);
		RequestHook<GetPickRangeHook>(GameModeVTable, 10);
		RequestHook<AttackHook>(GameModeVTable, 14);
	}

	{
		// Level Hook
		// Search for "Client%d camera ticking systems" -> xref first function -> What access to a1
		uintptr_t** LevelVTable = (uintptr_t**)Addresses::LevelVTable;
		RequestHook<LevelTickHook>(LevelVTable, 99);
	}

	if (kiero::init(kiero::RenderType::D3D12) == kiero::Status::Success) {
		uintptr_t** methodsTable = (uintptr_t**)kiero::getMethodsTable();
		RequestHook<PresentHook>(methodsTable, 140);
		RequestHook<ResizeBuffersHook>(methodsTable, 145);
	}
	else if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success) {
		uintptr_t** methodsTable = (uintptr_t**)kiero::getMethodsTable();
		RequestHook<PresentHook>(methodsTable, 8);
		RequestHook<ResizeBuffersHook>(methodsTable, 13);
	}
	else {
		return;
	}

	if (!getHook<ClientInstanceUpdateHook>()->enableHook()) {
		return;
	}

	while (g_Data.clientInstance == nullptr) {
		Sleep(250);
	}

	for (auto& funcHook : hooksCache) {
		funcHook->enableHook();
	}

	// We don't this anymore so we can delete it
	for (auto& funcHook : hooksCache) {
		delete funcHook;
	}
	hooksCache.clear();
}

void HookManager::shutdown() {
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
	kiero::shutdown();

	for (auto& funcHook : hooksCache) {
		delete funcHook;
	}
	hooksCache.clear();
}