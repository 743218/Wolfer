#pragma once
#include "../../ModuleBase/Module.h"
#include <cmath>

class Fly : public Module {
private:
    float hSpeed = 1.f;
    float vSpeed = 0.5f;
    float glide = -0.02f;

public:
    Fly() : Module("Fly", "Fly with horizontal and vertical speed", Category::MOVEMENT) {
        registerSetting(new SliderSetting<float>("Horizontal", "Horizontal fly speed", &hSpeed, 1.f, 0.1f, 10.f));
        registerSetting(new SliderSetting<float>("Vertical", "Vertical fly speed", &vSpeed, 0.5f, 0.1f, 5.f));
        registerSetting(new SliderSetting<float>("Glide", "Constant downward glide force", &glide, -0.02f, -1.0f, 0.0f));
    }

    void onTick(GameMode* gm) override {
        LocalPlayer* localPlayer = g_Data.getLocalPlayer();

        if (!g_Data.canUseMoveKeys()) return;

        Vec3<float> motion(0.f, glide, 0.f);

        if (g_Data.isKeyDown(VK_SPACE)) motion.y += vSpeed;
        if (g_Data.isKeyDown(VK_SHIFT)) motion.y -= vSpeed;

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

        if (x != 0 || y != 0) {
            double angle = std::atan2(static_cast<double>(x), static_cast<double>(y)) * (180.0 / static_cast<double>(PI));
            double finalYaw = (static_cast<double>(yaw) + angle + 90.0) * (static_cast<double>(PI) / 180.0);
            motion.x = static_cast<float>(std::cos(finalYaw) * static_cast<double>(hSpeed));
            motion.z = static_cast<float>(std::sin(finalYaw) * static_cast<double>(hSpeed));
        }

        localPlayer->lerpMotion(motion);
    }
};
