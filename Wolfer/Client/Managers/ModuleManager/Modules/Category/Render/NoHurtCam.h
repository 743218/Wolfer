#pragma once
#include "../../ModuleBase/Module.h"

class NoHurtCam : public Module {
public:
    NoHurtCam() : Module("NoHurtCam", "Remove hurt camera effect", Category::RENDER) {}
};