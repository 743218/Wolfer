#pragma once
#include "../../ModuleBase/Module.h"

class AirJump : public Module {
public:
    AirJump() : Module("AirJump", "Allow you to jump in the air", Category::MOVEMENT) {}

    void onLevelTick(Level* level) override {
        LocalPlayer* localPlayer = g_Data.getLocalPlayer();
        if (localPlayer) {
            localPlayer->setIsOnGround(true);
        }
    }
};