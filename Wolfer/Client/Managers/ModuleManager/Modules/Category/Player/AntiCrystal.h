#pragma once
#include "../../ModuleBase/Module.h"

class AntiCrystal : public Module {
private:
    float height = 0.40f;

public:
    AntiCrystal() : Module("AntiCrystal", "Phase into the ground using packets", Category::PLAYER) {
        registerSetting(new SliderSetting<float>("Height", "NULL", &height, 0.40f, 0.f, 1.f));
    }

    void onSendPacket(Packet* packet) override {
        if (!packet) return;

        if (packet->getId() == PacketID::PlayerAuthInput) {
            auto paip = static_cast<PlayerAuthInputPacket*>(packet);
            paip->position.y -= height;
        }
        else if (packet->getId() == PacketID::MovePlayer) {
            auto mpp = static_cast<MovePlayerPacket*>(packet);
            mpp->mPosition.y -= height;
        }
    }
};
