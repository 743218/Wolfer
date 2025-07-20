#pragma once
#include "../../ModuleBase/Module.h"

class Fullbright : public Module {
public:
    Fullbright() : Module("Fullbright", "Makes the game constantly bright", Category::RENDER) {}
};