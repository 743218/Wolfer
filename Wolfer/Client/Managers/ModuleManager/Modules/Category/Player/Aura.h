#pragma once
#include "../../ModuleBase/Module.h"

class Aura : public Module {
public:
	Aura() : Module("Aura", "Attacks nearby entities", Category::PLAYER) {
		registerSetting(new SliderSetting<float>("Reach", "Attack range", &range, 5.f, 3.f, 35.f));
		registerSetting(new SliderSetting<int>("Delay", "Attack delay in ticks", &delay, 5, 0, 20));
		registerSetting(new SliderSetting<int>("Hit Attempts", "How many times to attack the target.", &hitAttempts, 1, 0, 10));
		registerSetting(new EnumSetting("Rotation", "The rotation to the target target", { "None", "Silent", "Strafe" }, &strafe, strafe));
		registerSetting(new BoolSetting("Mobs", "Attack mobs", &mobs, false));
	}

	void onEnable() override {
		tickCounter = delay;
		target = nullptr;
		shouldRotate = false;
	}

	void onTick(GameMode* gm) override {
		auto* player = g_Data.getLocalPlayer();
		if (!player || !player->level) return;

		if (++tickCounter < delay) return;
		tickCounter = 0;

		Actor* closest = nullptr;
		float minDist = range;

		for (auto& entity : ActorUtils::getActorList()) {
			if (!entity || !TargetUtil::isTargetValid(entity, mobs, true, range)) continue;
			float dist = player->getPos().dist(entity->getPos());
			if (dist < minDist) {
				minDist = dist;
				closest = entity;
			}
		}

		if (!closest) {
			target = nullptr;
			return;
		}

		target = closest;
		targetRot = player->getEyePos().CalcAngle(target->getEyePos());

		for (int i = 0; i < hitAttempts; i++) {
	    	player->gameMode->attack(target);
		}
		player->swing(); //Servers on bedrock don't care if you swing or not. There's no point in the wasted packets. We only need to do this one time.
		
		shouldRotate = true;
	}

	void onSendPacket(Packet* packet) override {
		if (!packet || packet->getId() != PacketID::PlayerAuthInput) return;
		if (strafe == 0) return;
		
		auto* player = g_Data.getLocalPlayer();
		if (!shouldRotate || !target || !TargetUtil::isTargetValid(target, mobs, true, range) || !player) {
			target = nullptr;
			shouldRotate = false;
			return;
		}

		auto* input = reinterpret_cast<PlayerAuthInputPacket*>(packet);

		input->rotation.x = targetRot.x;
		input->rotation.y = targetRot.y;
		input->headYaw = targetRot.y;

		shouldRotate = false;
	}

	void onUpdateRotation(LocalPlayer* player) override {
		if (!player || !target || strafe != 2) return;

		if (!TargetUtil::isTargetValid(target, mobs, true, range)) {
			target = nullptr;
			return;
		}

		player->getActorHeadRotationComponent()->headYaw = targetRot.y;
		player->rotation->presentRot = targetRot;
	}

	void onLevelTick(Level* level) override {

	}
private:
	Actor* target = nullptr;
	Vector2<float> targetRot{};
	float range = 5.f;
	int hitAttempts = 1;
	int delay = 5;
	int tickCounter = 0;
	bool shouldRotate = false;
	int strafe = 0;
	bool mobs = false;
	bool i2b2tPE = false;
};
