#pragma once
#include "../../ModuleBase/Module.h"

class Disabler : public Module {
private:
    int mode = 0;
public:
    bool autism = false;

    Disabler() : Module("Disabler", "Bypass server anticheats", Category::MISC) {
        registerSetting(new EnumSetting("Mode", "Disabler mode", { "Aternos", "Geyser", "Lifeboat" }, &mode, 0));
        registerSetting(new BoolSetting("Autism", "Have a autism attack", &autism, true));
    }

    void onTick(GameMode* gm) override {
        LocalPlayer* localPlayer = g_Data.getLocalPlayer();
        if (!localPlayer) return;

        if (mode == 1)
            localPlayer->getlevel()->getHitResult()->type = HitResultType::AIR;

        if (mode == 0)
            localPlayer->setStatusFlag(ActorFlags::Crawling, false);

        if (autism)
            Global::rotation.y += ((rand() % 1001) / 100.0f) - 5.0f;
    }

    void onSendPacket(Packet* packet) override {
        if (!packet) return;

        if (mode == 0) {
            if (packet->getId() == PacketID::MovePlayer) {
                auto mpp = static_cast<MovePlayerPacket*>(packet);
                mpp->mPosition.y += 0.001f;
            }
            if (packet->getId() == PacketID::PlayerAuthInput) {
                auto paip = static_cast<PlayerAuthInputPacket*>(packet);
                paip->mInputData |= InputData::StartGliding;
            }
        }

        if (mode == 2) {
            if (packet->getId() == PacketID::PlayerAuthInput) {
                auto paip = static_cast<PlayerAuthInputPacket*>(packet);
                paip->TicksAlive = 0;
            }
        }
    }

    std::string getModeText() override {
        switch (mode) {
        case 0: return "Aternos";
        case 1: return "Guyser";
        case 2: return "Lifeboat";
        default: return "Unknown";
        }
    }
};