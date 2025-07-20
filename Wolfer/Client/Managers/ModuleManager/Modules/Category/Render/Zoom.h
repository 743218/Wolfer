#pragma once
#include "../../ModuleBase/Module.h"

class Zoom : public Module {
public:
    float zoomFov = 30.f;
    float currentFov = 90.f;
    float zoomSpeed = 5.f;

    Zoom() : Module("Zoom", "Zoom camera in by modifying FOV", Category::RENDER) {
        registerSetting(new SliderSetting<float>("Zoom FOV", "FOV when zoomed in", &zoomFov, 30.f, 1.f, 90.f));
    }

    bool isHoldMode() override {
        return true;
    }
};