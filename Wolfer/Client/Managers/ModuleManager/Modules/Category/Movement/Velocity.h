#pragma once
#include "../../ModuleBase/Module.h"

class Velocity : public Module {
public:
    float horizontal = 0.f;
    float vertical = 0.f;

    Velocity() : Module("Velocity", "Prevent you from knockback", Category::MOVEMENT) {
        registerSetting(new SliderSetting<float>("Horizontal", "NULL", &horizontal, horizontal, 0.f, 100.f));
        registerSetting(new SliderSetting<float>("Vertical", "NULL", &vertical, vertical, 0.f, 100.f));
    }

    std::string getModeText() override {
        static char textStr[15];
        std::sprintf(textStr, "H%i%%V%i%%", static_cast<int>(std::round(horizontal)), static_cast<int>(std::round(vertical)));
        return std::string(textStr);
    }
};