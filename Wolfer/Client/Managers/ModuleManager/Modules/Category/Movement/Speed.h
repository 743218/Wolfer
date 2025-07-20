#pragma once
#include "../../ModuleBase/Module.h"

class Speed : public Module {
private:
    float speed = 0.40f;

public:
    Speed() : Module("Speed", "Move faster", Category::MOVEMENT) {
        registerSetting(new SliderSetting<float>("Speed", "Movement speed", &speed, 0.40f, 0.0f, 2.0f));
    }

    void onClientTick() override {
        LocalPlayer* localPlayer = g_Data.getLocalPlayer();
        if (!localPlayer) return;

        MoveUtil::setSpeed(speed);
    }
};
