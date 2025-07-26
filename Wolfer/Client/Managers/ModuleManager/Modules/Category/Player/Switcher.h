#pragma once
#include "../../ModuleBase/Module.h"

class Switcher : public Module {
private:
    int startSlot = 0;
    int endSlot = 2;
    int delay = 0;
    int ticks = 0;
    bool fast = false;
    int currentSlot = 0;

public:
    Switcher() : Module("Switcher", "Cycles through hotbar slots", Category::PLAYER) {
        registerSetting(new SliderSetting<int>("Start Slot", "First slot to cycle", &startSlot, 0, 0, 8));
        registerSetting(new SliderSetting<int>("End Slot", "Last slot to cycle", &endSlot, 2, 0, 8));
        registerSetting(new SliderSetting<int>("Delay", "Delay between switches", &delay, 0, 0, 20));
        registerSetting(new BoolSetting("Fast", "Use client tick instead of game tick", &fast, false));
    }

    void onEnable() override {
        currentSlot = startSlot;
        ticks = 0;
    }

    void tickInternal() {
        LocalPlayer* player = g_Data.getLocalPlayer();
        if (!player || !player->playerInventory) return;

        if (delay > 0 && ++ticks < delay) return;
        ticks = 0;

        currentSlot++;
        if (currentSlot > endSlot || currentSlot < startSlot)
            currentSlot = startSlot;

        player->playerInventory->selectedSlot = currentSlot;
    }

    void onTick(GameMode*) override {
        if (!fast) tickInternal();
    }

    void onClientTick() override {
        if (fast) tickInternal();
    }
};
