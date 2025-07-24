#pragma once
#include "../../ModuleBase/Module.h"

class ServerRotations : public Module {
public:
	ServerRotations() : Module("ServerRotations", "Lerps global rotation to our rotation and sets our rotation to it", Category::MISC) {}

	static float normalizeAngle(float angle) {
		while (angle > 180.f) angle -= 360.f;
		while (angle < -180.f) angle += 360.f;
		return angle;
	}

	void onUpdateRotation(LocalPlayer* player) override {
		if (!player) return;

		Vector2<float> currentGlobalRot;
		{
			std::lock_guard<std::mutex> lock(Global::rotationMutex);
			currentGlobalRot = Global::rotation;
		}

		float localPitch = normalizeAngle(player->rotation->presentRot.x);
		float localYaw = normalizeAngle(player->getMobBodyRotationComponent()->bodyYaw);
		float headYaw = normalizeAngle(player->getActorHeadRotationComponent()->headYaw);

		currentGlobalRot.x = normalizeAngle(currentGlobalRot.x);
		currentGlobalRot.y = normalizeAngle(currentGlobalRot.y);

		const float bodyLerpSpeed = 0.35f;
		const float headLerpSpeed = 0.10f;

		Vector2<float> newRot;
		newRot.x = Math::lerp(currentGlobalRot.x, localPitch, bodyLerpSpeed);
		newRot.y = Math::lerpAngle(currentGlobalRot.y, localYaw, bodyLerpSpeed);

		float newHeadYaw = Math::lerpAngle(headYaw, localYaw, headLerpSpeed);

		newRot.x = normalizeAngle(newRot.x);
		newRot.y = normalizeAngle(newRot.y);
		newHeadYaw = normalizeAngle(newHeadYaw);

		{
			std::lock_guard<std::mutex> lock(Global::rotationMutex);
			Global::rotation = newRot;
		}

		player->rotation->presentRot.x = newRot.x;
		player->rotation->presentRot.y = newHeadYaw;
		player->getMobBodyRotationComponent()->bodyYaw = newRot.y;
		player->getActorHeadRotationComponent()->headYaw = newHeadYaw;
	}

	void onSendPacket(Packet* packet) override {
		if (!packet || packet->getId() != PacketID::PlayerAuthInput) return;

		auto* input = reinterpret_cast<PlayerAuthInputPacket*>(packet);
		Vector2<float> currentGlobalRot;
		{
			std::lock_guard<std::mutex> lock(Global::rotationMutex);
			currentGlobalRot = Global::rotation;
		}

		input->rotation.x = currentGlobalRot.x;
		input->rotation.y = currentGlobalRot.y;
		input->headYaw = currentGlobalRot.y;
	}

private:
};
