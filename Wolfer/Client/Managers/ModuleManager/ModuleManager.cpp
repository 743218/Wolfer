#include "ModuleManager.h"
#include "../../Client.h"
#include "Modules/Category/Client/Hud.h"
#include "Modules/Category/Client/ClickGUI.h"

void ModuleManager::init() {

	// MOVEMENT
	moduleList.push_back(new Velocity());
	moduleList.push_back(new Jetpack());
	moduleList.push_back(new NoClip());
	moduleList.push_back(new Speed());
	moduleList.push_back(new Fly());

	// RENDER
	moduleList.push_back(new ChunkBorders());
	moduleList.push_back(new ConicalHat());
	moduleList.push_back(new Fullbright());
	moduleList.push_back(new StorageESP());
	moduleList.push_back(new NoHurtCam());
	moduleList.push_back(new NoRender());
	moduleList.push_back(new NameTags());
	moduleList.push_back(new Swing());
	moduleList.push_back(new Zoom());

	// PLAYER
	moduleList.push_back(new ChestStealer());
	moduleList.push_back(new AntiCrystal());
	moduleList.push_back(new SpinAttack());
	moduleList.push_back(new PopCounter());
	moduleList.push_back(new BlockReach());
	moduleList.push_back(new Scaffold());
	moduleList.push_back(new Offhand());
	moduleList.push_back(new Aura());

	// MISC
	moduleList.push_back(new NoPacket());
	moduleList.push_back(new Timer());
	moduleList.push_back(new Disabler());

	// CLIENT
	moduleList.push_back(new CustomFont());
	moduleList.push_back(new Hud());
	moduleList.push_back(new ClickGUI());

	// EXPLOIT

	moduleList.push_back(new Phase());

	std::sort(moduleList.begin(), moduleList.end(), [](Module* lhs, Module* rhs) {
		return lhs->getModuleName() < rhs->getModuleName();
	});

	getModule<ClickGUI>()->InitClickGUI();
}

void ModuleManager::shutdown() {
	for (auto& mod : moduleList) {
		mod->setEnabled(false);
	}
	for (auto& mod : moduleList) {
		delete mod;
		mod = nullptr;
	}
	moduleList.clear();
}

void ModuleManager::onKeyUpdate(int key, bool isDown) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		mod->onKeyUpdate(key, isDown);
	}
}

void ModuleManager::onClientTick() {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onClientTick();
		}
	}
}

void ModuleManager::onClientTick(Packet* packet) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onSendPacket(packet);
		}
	}
}

void ModuleManager::onContainerTick(ContainerScreenController* controller) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onContainerTick(controller);
		}
	}
}

void ModuleManager::onTick(GameMode* gm) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onTick(gm);
		}
	}
}

void ModuleManager::onLevelTick(Level* level) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onLevelTick(level);
		}
	}
}

void ModuleManager::onUpdateRotation(LocalPlayer* localPlayer) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onUpdateRotation(localPlayer);
		}
	}
}

void ModuleManager::onSendPacket(Packet* packet) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onSendPacket(packet);
		}
	}
}

void ModuleManager::onD2DRender() {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onD2DRender();
		}
	}
}

void ModuleManager::onMCRender(MinecraftUIRenderContext* renderCtx) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onMCRender(renderCtx);
		}
	}
}

void ModuleManager::onLevelRender() {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		if (mod->isEnabled() || mod->runOnBackground()) {
			mod->onLevelRender();
		}
	}
}

void ModuleManager::onLoadConfig(void* conf) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		mod->onLoadConfig(conf);
	}
}

void ModuleManager::onSaveConfig(void* conf) {
	if (!Client::isInitialized())
		return;

	for (auto& mod : moduleList) {
		mod->onSaveConfig(conf);
	}
}