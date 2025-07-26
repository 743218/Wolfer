#pragma once
#include "../../ModuleBase/Module.h"

class PacketFixer : public Module {
private:
    bool posfix = false;
    bool rotfix = true;
    bool velfix = true;
    bool grdflags = false;
public:
    PacketFixer() : Module("PacketFixer", "fix stupid packets from being desynced", Category::MISC) {
        registerSetting(new BoolSetting("Position", "Sync position. do not use with anticrystal", &posfix, false));
        registerSetting(new BoolSetting("Rotation", "Sync rotation. do not use with fancy rotations", &rotfix, true));
        registerSetting(new BoolSetting("Velocity", "Sync velocity. do not use with anticrystal or velocity", &velfix, true));
        registerSetting(new BoolSetting("Ground Flags", "Sync ground flags.", &grdflags, false));
    }

    void onSendPacket(Packet* packet) override {
        if (!packet) return;
        if (!g_Data.getLocalPlayer()) return;

        if (packet->getId() == PacketID::PlayerAuthInput) {
            auto paip = static_cast<PlayerAuthInputPacket*>(packet);
            if (paip) {
                if (posfix) {
                    paip->position = g_Data.getLocalPlayer()->getPos();
                }
                if (rotfix) {
                    paip->rotation = g_Data.getLocalPlayer()->rotation->presentRot;
                    paip->headYaw = g_Data.getLocalPlayer()->getActorHeadRotationComponent()->headYaw;
                }
                if (velfix) {
                    paip->mPosDelta = g_Data.getLocalPlayer()->stateVector->velocity;
                }
            }
        }
        else if (packet->getId() == PacketID::MovePlayer) {
            auto mpp = static_cast<MovePlayerPacket*>(packet);
            if (mpp) {
                if (posfix) {
                    mpp->mPosition = g_Data.getLocalPlayer()->getPos();
                }
                if (rotfix) {
                    mpp->headYaw = g_Data.getLocalPlayer()->getActorHeadRotationComponent()->headYaw;
                }
                if (grdflags) {
                    g_Data.getLocalPlayer()->setIsOnGround(mpp->onGround);
                }
            }
        }
    }
};
