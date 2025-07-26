#pragma once
#include "../../ModuleBase/Module.h"

class DeviceIDSpoofer : public Module {
public:
    DeviceIDSpoofer() : Module("DeviceIDSpoofer", "Spoof your fucking id", Category::MISC) {
    }
};
