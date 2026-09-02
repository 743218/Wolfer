#pragma once
#include "../../ModuleBase/Module.h"

class NoSlow : public Module {
public:
    NoSlow() : Module("NoSlow", "Remove item use slowdown", Category::MOVEMENT) {}

    void onLevelTick(Level* level) override {
    }
};
