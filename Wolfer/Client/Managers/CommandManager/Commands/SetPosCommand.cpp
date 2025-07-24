#include "SetPosCommand.h"
#include "../../../Managers/ModuleManager/ModuleManager.h"
#include "../../../../Client/Client.h"

SetPosCommand::SetPosCommand() : CommandBase("setpos", "Sets teleport target position", "<x> <y> <z>", {}) {}

bool SetPosCommand::execute(const std::vector<std::string>& args) {
    if (args.size() != 4) {
        Client::DisplayClientMessage("%sUsage: .setpos <x> <y> <z>", MCTF::RED);
        return false;
    }

    try {
        float x = std::stof(args[1]);
        float y = std::stof(args[2]);
        float z = std::stof(args[3]);

        auto teleportMod = ModuleManager::getModule<SMTeleport>();
        if (!teleportMod) {
            Client::DisplayClientMessage("%sSMTeleport module not found!", MCTF::RED);
            return true;
        }

        teleportMod->setTargetPos(Vector3<float>(x, y, z));
        return true;
    }
    catch (...) {
        Client::DisplayClientMessage("%sInvalid coordinates!", MCTF::RED);
    }

    return true;
}