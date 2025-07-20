#pragma once
#include "../../ModuleBase/Module.h"

class NoPacket : public Module {
public:
    NoPacket() : Module("NoPacket", "Stop sending packets to servers\n(This module commonly used for bug/dupe on server)", Category::MISC) {}
};
