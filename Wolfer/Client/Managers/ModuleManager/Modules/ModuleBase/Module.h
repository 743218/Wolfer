#pragma once
#include <string>
#include <vector>
#include "../../../../../Api/Game.h"
#include "../../../../../Renderer/D2D.h"
#include "../../../../../Renderer/MCR.h"
#include "../../../../../Api/Utils/TimerUtil.h"
#include "../../../../../Api/Utils/Minecraft/InventoryUtil.h"
#include "../../../../../Api/Utils/Minecraft/PlayerUtil.h"
#include "../../../../../Api/Utils/Minecraft/TargetUtil.h"
#include "../../../../../Api/Utils/Minecraft/WorldUtil.h"
#include "../../../../../Libs/json.hpp"

#include "../../../HookManager/Hooks/Game/ContainerTickController.h"
#include "Settings/Setting.h"
#include "Settings/BoolSetting.h"
#include "Settings/ColorSetting.h"
#include "Settings/EnumSetting.h"
#include "Settings/KeybindSetting.h"
#include "Settings/SliderSetting.h"

enum class Category {
	MOVEMENT = 0,
	RENDER = 1,
	PLAYER = 2,
	MISC = 3,
	CLIENT = 4,
	EXPLOIT = 5
};

class Module {
private:
	std::string name;
	std::string description;
	Category category;
	bool enabled = false;
	bool visible = true;
	int keybind = 0x0;
	int toggleMode = 0;

	std::vector<Setting*> settings;
public:
	//Arraylist stuff
	float arraylistAnim = 0.f;

	//ClickGUI stuff
	float selectedAnim = 0.f;
	bool extended = false;
protected:
	inline Setting* registerSetting(Setting* setting) {
		this->settings.push_back(setting);
		return setting;
	}
public:
	Module(std::string moduleName, std::string des, Category c, int k = 0x0);
	~Module();

	inline std::string getModuleName() {
		return this->name;
	}

	inline std::string getDescription() {
		return this->description;
	}

	inline Category getCategory() { 
		return this->category;
	}

	inline std::vector<Setting*>& getSettingList() {
		return this->settings;
	}
public:
	virtual std::string getModeText();
	virtual bool isEnabled();
	virtual bool isVisible();
	virtual bool isHoldMode();
	virtual int getKeybind();
	virtual void setKeybind(int key);
	virtual bool runOnBackground();
	virtual void setEnabled(bool enable);
	virtual void toggle();
	virtual void onDisable();
	virtual void onEnable();
	virtual void onKeyUpdate(int key, bool isDown);
	virtual void onClientTick();
	virtual void onTick(GameMode* gm);
	virtual void onLevelTick(Level* level);
	virtual void onUpdateRotation(LocalPlayer* localPlayer);
	virtual void onSendPacket(Packet* packet);
	virtual void onD2DRender();
	virtual void onMCRender(MinecraftUIRenderContext* renderCtx);
	virtual void onLevelRender();
	virtual void onLoadConfig(void* conf);
	virtual void onSaveConfig(void* conf);
	virtual void onContainerTick(ContainerScreenController* controller);
	virtual void onAttack(Actor* actor, bool& cancel);
};
