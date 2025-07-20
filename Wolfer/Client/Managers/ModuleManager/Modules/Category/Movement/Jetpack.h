#pragma once
#include "../../ModuleBase/Module.h"

class Jetpack : public Module {
private:
    float speed = 1.f;

public:
    Jetpack() : Module("Jetpack", "It JetPack :D", Category::MOVEMENT) {
        registerSetting(new SliderSetting<float>("Speed", "NULL", &speed, 1.f, 0.2f, 3.f));
    }

    void onTick(GameMode* gm) override {
        LocalPlayer* localPlayer = g_Data.getLocalPlayer();

        float calcYaw = (localPlayer->rotation->presentRot.y + 90.f) * (PI / 180.f);
        float calcPitch = (localPlayer->rotation->prevRot.x) * -(PI / 180.f);

        Vec3<float> moveVec;
        moveVec.x = std::cos(calcYaw) * std::cos(calcPitch) * speed;
        moveVec.y = std::sin(calcPitch) * speed;
        moveVec.z = std::sin(calcYaw) * std::cos(calcPitch) * speed;

        localPlayer->lerpMotion(moveVec);
    }
};
