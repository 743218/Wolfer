#pragma once
#include "../../ModuleBase/Module.h"

class AutoLog : public Module {
public:
    class CommandRequestPacket : public Packet {
    public:
        std::string mCommand;
        uint8_t origin = 0;
        bool isExternal = 0;
        char filler[0x256];
    };

    AutoLog() : Module("AutoLog", "Logs out when health is low", Category::MISC) {
        registerSetting(new SliderSetting<float>("Health", "Health to log out at", &healthThreshold, 6.f, 1.f, 20.f));
        registerSetting(new BoolSetting("Disable", "Disable module after logging", &disableAfterLog, true));
    }

    void onTick(GameMode* gm) override {
        auto* player = g_Data.getLocalPlayer();
        if (!player || !player->level) return;

        float currentHealth = player->getHealth();

        if (currentHealth <= healthThreshold) {
            std::shared_ptr<Packet> packet = MinecraftPackets::createPacket(PacketID::CommandRequest);
            CommandRequestPacket* cmdPkt = reinterpret_cast<CommandRequestPacket*>(packet.get());

            cmdPkt->isExternal = false;
            cmdPkt->mCommand = "/hub";
            cmdPkt->origin = 0;

            player->level->getPacketSender()->send(cmdPkt);

            if (disableAfterLog)
                setEnabled(false);
        }
    }

private:
    float healthThreshold = 6.f;
    bool disableAfterLog = true;
};
