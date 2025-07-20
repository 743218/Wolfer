#pragma once
#include "../../ModuleBase/Module.h"

class NoClip : public Module {
private:
    float hSpeed = 1.f;
    float vSpeed = 0.5f;

public:
    NoClip() : Module("NoClip", "Move through blocks with setPos", Category::MOVEMENT) {
        registerSetting(new SliderSetting<float>("Horizontal", "Horizontal noclip speed", &hSpeed, 1.f, 0.1f, 10.f));
        registerSetting(new SliderSetting<float>("Vertical", "Vertical noclip speed", &vSpeed, 0.5f, 0.1f, 5.f));
    }

    void onTick(GameMode* gm) override {
        LocalPlayer* localPlayer = g_Data.getLocalPlayer();

        if (!g_Data.canUseMoveKeys()) return;

        Vec3<float> currentPos = localPlayer->getPos();

        float yaw = localPlayer->rotation->presentRot.y;

        bool forward = g_Data.isKeyDown('W');
        bool back = g_Data.isKeyDown('S');
        bool left = g_Data.isKeyDown('A');
        bool right = g_Data.isKeyDown('D');

        int x = 0, y = 0;

        if (forward) y += 1;
        if (back) y -= 1;
        if (left) x -= 1;
        if (right) x += 1;

        Vec3<float> offset(0.f, 0.f, 0.f);

        if (x != 0 || y != 0) {
            double angle = std::atan2(static_cast<double>(x), static_cast<double>(y)) * (180.0 / static_cast<double>(PI));
            double finalYaw = (static_cast<double>(yaw) + angle + 90.0) * (static_cast<double>(PI) / 180.0);
            offset.x = static_cast<float>(std::cos(finalYaw) * static_cast<double>(hSpeed));
            offset.z = static_cast<float>(std::sin(finalYaw) * static_cast<double>(hSpeed));
        }

        if (g_Data.isKeyDown(VK_SPACE)) offset.y += vSpeed;
        if (g_Data.isKeyDown(VK_SHIFT)) offset.y -= vSpeed;

        Vec3<float> newPos(
            currentPos.x + offset.x,
            currentPos.y + offset.y,
            currentPos.z + offset.z
        );

        localPlayer->setPos(newPos);
    }
};
