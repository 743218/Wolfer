#pragma once
#include "../../ModuleBase/Module.h"

class BlockReach : public Module {
public:
    float blockReach = 7.f;

    BlockReach() : Module("BlockReach", "Exteneded reach for placing/breaking blocks", Category::PLAYER) {
        registerSetting(new SliderSetting<float>("Reach", "How far your reach", &blockReach, blockReach, 7.f, 20.f));
    }
};
