#pragma once
#include "../../ModuleBase/Module.h"

class HubLClick : public Module {
public:
    class CommandRequestPacket : public Packet {
    public:
        std::string mCommand;
        uint8_t origin = 0;
        bool isExternal = 0;
        char shit[0x256];
    };

    HubLClick() : Module("HubLClick", "Sends /hub command on right-click", Category::MISC) {}

    void onTick(GameMode* gm) override {
        if (!g_Data.isLeftClickDown()) return;

        auto* player = g_Data.getLocalPlayer();
        if (!player || !player->level) return;

        std::shared_ptr<Packet> packet = MinecraftPackets::createPacket(PacketID::CommandRequest);
        CommandRequestPacket* cmdPkt = reinterpret_cast<CommandRequestPacket*>(packet.get());

        cmdPkt->isExternal = false;
        cmdPkt->mCommand = "/hub";
        cmdPkt->origin = 0;

        player->level->getPacketSender()->send(cmdPkt);

        setEnabled(false);
    }
};
