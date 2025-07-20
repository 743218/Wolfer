#pragma once
#include "../../Api/Game.h"

namespace PlayerUtil {
	extern uint32_t selectedSlotServerSide;
	bool canPlaceBlock(const BlockPos& blockPos, bool airPlace = false);
	bool tryPlaceBlock(const BlockPos& blockPos, bool airPlace = false, bool packetPlace = false);
	bool tryPlaceBlockSupport(const BlockPos& pos, int proximity = 5, bool airplace = false, bool packetPlace = false);
	inline void SendTextMessage(const std::string& message) {
		std::shared_ptr<Packet> sharedPkt = MinecraftPackets::createPacket(PacketID::Text);
		TextPacket* textPacket = (TextPacket*)sharedPkt.get();
		textPacket->mType = TextPacketType::Chat;
		textPacket->mAuthor = *g_Data.getLocalPlayer()->getNameTag();
		textPacket->mMessage = message;
		textPacket->mLocalize = false;
		textPacket->mPlatformId = textPacket->mPlatformId;
		textPacket->mXuid = textPacket->mXuid;
		g_Data.getLocalPlayer()->level->getPacketSender()->send(textPacket);
	}
}

namespace MoveUtil {
	bool isMoving();
	Vec2<float> getMotion(const float& speed);
	float getAdjustedYaw();
	void setSpeed(const float& speed);
	inline void stopMotion() {
		g_Data.getLocalPlayer()->stateVector->velocity.x = 0.f;
		g_Data.getLocalPlayer()->stateVector->velocity.z = 0.f;
	}

	inline Vec3<float>& getVelocity() {
		return g_Data.getLocalPlayer()->stateVector->velocity;
	}
}