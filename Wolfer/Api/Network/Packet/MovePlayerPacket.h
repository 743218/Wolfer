#pragma once
#include "Packet.h"
#include "../../../Api/Utils/Maths.h"
enum class PositionMode : int {
    Normal = 0,
    Respawn = 1,
    Teleport = 2,
    OnlyHeadRot = 3,
};

enum class TeleportationCause : int {
    Unknown = 0x0,
    Projectile = 0x1,
    ChorusFruit = 0x2,
    Command = 0x3,
    Behavior = 0x4,
    Count = 0x5,
};

class MovePlayerPacket : public Packet {
public:
    int64_t          mRuntimeId;
    Vector3<float>      mPosition;
    float            headYaw;
    PositionMode          mode;
    bool             onGround;
    uint64_t         ridingRuntimeId;
    TeleportationCause              mTeleportCause;
    __int32          mTeleportItem;
    __int32          mTeleportTick;
};