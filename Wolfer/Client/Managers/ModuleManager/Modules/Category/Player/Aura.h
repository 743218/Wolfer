#pragma once
#include "../../ModuleBase/Module.h"

class Aura : public Module {
public:
	Aura() : Module("Aura", "Attacks nearby entities", Category::PLAYER) {
		registerSetting(new SliderSetting<float>("Reach", "Attack range", &range, 5.f, 3.f, 15.f));
		registerSetting(new SliderSetting<int>("Delay", "Attack delay in ticks", &delay, 5, 0, 20));
		registerSetting(new BoolSetting("Strafe", "Strafe around the target", &strafe, false));
		registerSetting(new BoolSetting("Mobs", "Attack mobs", &mobs, false));
		registerSetting(new BoolSetting("Lifeboat", "Does more damage on lifeboat, apparently.", &lifeboat, false));
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
		player->gameMode->attack(target);
		player->swing();
		shouldRotate = true;
	}

	void onSendPacket(Packet* packet) override {
		if (!packet || packet->getId() != PacketID::PlayerAuthInput) return;

		auto* player = g_Data.getLocalPlayer();
		if (!shouldRotate || !target || !TargetUtil::isTargetValid(target, mobs, true, range) || !player) {
			target = nullptr;
			shouldRotate = false;
			return;
		}

		auto* input = reinterpret_cast<PlayerAuthInputPacket*>(packet);

		{
			input->rotation.x = targetRot.x;
			input->rotation.y = targetRot.y;
			input->headYaw = targetRot.y;
		}

		shouldRotate = false;
	}

	void onUpdateRotation(LocalPlayer* player) override {
		if (!player || !target || !strafe) return;

		if (!TargetUtil::isTargetValid(target, mobs, true, range)) {
			target = nullptr;
			return;
		}

		player->getActorHeadRotationComponent()->headYaw = targetRot.y;
		player->rotation->presentRot = targetRot;
	}

	void onAttack(Actor* actor, bool& cancel) override {
		if (!actor || !g_Data.getLocalPlayer() || !lifeboat) return;
		auto* lp = g_Data.getLocalPlayer();
		lp->setSprinting(true);
		lp->stateVector->oldPos.y -= 0.001f;
	}

private:
	Actor* target = nullptr;
	Vector2<float> targetRot{};
	float range = 5.f;
	int delay = 5;
	int tickCounter = 0;
	bool shouldRotate = false;
	bool strafe = false;
	bool mobs = false;
	bool lifeboat = false;
};