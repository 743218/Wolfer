#pragma once
#include "../../ModuleBase/Module.h"

class Swing : public Module {
public:
    int swingSpeed = 12;

    Swing() : Module("Swing", "Swing animation", Category::RENDER) {
        registerSetting(new SliderSetting<int>("Speed", "Swing speed", &swingSpeed, swingSpeed, 1, 20));
    }
};