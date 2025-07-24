#pragma once
#include "Modules/ModuleBase/Module.h"

// MOVEMENT
#include "Modules/Category/Movement/Velocity.h"
#include "Modules/Category/Movement/Jetpack.h"
#include "Modules/Category/Movement/NoClip.h"
#include "Modules/Category/Movement/Speed.h"
#include "Modules/Category/Movement/Fly.h"
#include "Modules/Category/Movement/ZeroTP.h"
#include "Modules/Category/Movement/AirJump.h"

// RENDER
#include "Modules/Category/Render/ChunkBorders.h"
#include "Modules/Category/Render/StorageESP.h"
#include "Modules/Category/Render/ItemESP.h"
#include "Modules/Category/Render/ConicalHat.h"
#include "Modules/Category/Render/Fullbright.h"
#include "Modules/Category/Render/NoHurtCam.h"
#include "Modules/Category/Render/NoRender.h"
#include "Modules/Category/Render/Nametags.h"
#include "Modules/Category/Render/Swing.h"
#include "Modules/Category/Render/Zoom.h"

// PLAYER
#include "Modules/Category/Player/ChestStealer.h"
#include "Modules/Category/Player/AntiCrystal.h"
#include "Modules/Category/Player/PopCounter.h"
#include "Modules/Category/Player/SpinAttack.h"
#include "Modules/Category/Player/BlockReach.h"
#include "Modules/Category/Player/Scaffold.h"
#include "Modules/Category/Player/Offhand.h"
#include "Modules/Category/Player/Aura.h"

// MISC
#include "Modules/Category/Misc/Disabler.h"
#include "Modules/Category/Misc/NoPacket.h"
#include "Modules/Category/Misc/Timer.h"

// CLIENT
#include "Modules/Category/Client/CustomFont.h"

// EXPLOIT
#include "Modules/Category/Exploit/Phase.h"
#include "Modules/Category/Exploit/InventoryDesync.h"
#include "Modules/Category/Exploit/InstaUse.h"

class ModuleManager {
public:
	static inline std::vector<Module*> moduleList;

	static void init();
	static void shutdown();

	template <typename TRet>
	static TRet* getModule() {
		for (Module* mod : moduleList) {
			TRet* result = dynamic_cast<TRet*>(mod);
			if (result == nullptr)
				continue;
			return result;
		}
		return nullptr;
	}

	static void onKeyUpdate(int key, bool isDown);
	static void onClientTick();
	static void onClientTick(Packet* packet);
	static void onTick(GameMode* gm);
	static void onLevelTick(Level* level);
	static void onUpdateRotation(LocalPlayer* localPlayer);
	static void onSendPacket(Packet* packet);
	static void onD2DRender();
	static void onMCRender(MinecraftUIRenderContext* renderCtx);
	static void onLevelRender();
	static void onLoadConfig(void* conf);
	static void onSaveConfig(void* conf);
	static void onContainerTick(ContainerScreenController* controller);
	static void onAttack(Actor* actor, bool& cancel);
};
