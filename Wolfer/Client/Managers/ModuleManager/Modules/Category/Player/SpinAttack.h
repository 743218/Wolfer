#pragma once
#include "../../ModuleBase/Module.h"

class SpinAttack : public Module {
public:
    SpinAttack() : Module("SpinAttack", "Makes you spin as if using a trident", Category::PLAYER) {}

    void onSendPacket(Packet* packet) override {
        if (!packet || packet->getId() != PacketID::PlayerAuthInput) return;
        PlayerAuthInputPacket* paip = static_cast<PlayerAuthInputPacket*>(packet);
        paip->mInputData |= InputData::StopSprinting;
    }

    void onClientTick() override {
        LocalPlayer* localPlayer = g_Data.getLocalPlayer();
        if (!localPlayer) return;
        localPlayer->setStatusFlag(ActorFlags::Damagenearbymobs, true);
    }
};
