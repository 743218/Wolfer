#pragma once
#include "../../ModuleBase/Module.h"

class Aura : public Module {
public:
	Aura() : Module("Aura", "Attacks nearby entities", Category::PLAYER) {
		registerSetting(new SliderSetting<float>("Reach", "Attack range", &range, 5.f, 3.f, 15.f));
		registerSetting(new SliderSetting<int>("Delay", "Attack delay in ticks", &delay, 5, 0, 20));
		registerSetting(new BoolSetting("Strafe", "Strafe around the target", &strafe, false));
	}

	void onEnable() override {
		tickCounter = delay;
	}

	void onTick(GameMode* gm) override {
		auto* player = g_Data.getLocalPlayer();
		if (!player || !player->level) return;

		if (++tickCounter < delay) return;
		tickCounter = 0;

		Actor* closest = nullptr;
		float minDist = range;

		for (auto& entity : player->level->getRuntimeActorList()) {
			if (!entity || !TargetUtil::isTargetValid(entity, false, true, range)) continue;
			float dist = player->getPos().dist(entity->getPos());
			if (dist <= minDist) {
				minDist = dist;
				closest = entity;
			}
		}

		if (!closest) return;

		rot = player->getEyePos().CalcAngle(closest->getEyePos());
		player->gameMode->attack(closest);
		player->swing();

		targets.clear();
		targets.push_back(closest);
		shouldRotate = true;
	}

	void onSendPacket(Packet* packet) override {
		if (!shouldRotate || !packet || packet->getId() != PacketID::PlayerAuthInput) return;
		if (!targets.empty()) return;

		auto* input = static_cast<PlayerAuthInputPacket*>(packet);
		input->rotation.y = rot.y;
		input->headYaw = rot.y;
		input->rotation.x = rot.x;
		shouldRotate = false;
	}

	void onUpdateRotation(LocalPlayer* player) override {
		if (!player || !strafe || !targets.empty()) return;
		player->rotation->presentRot = rot;
		player->getActorHeadRotationComponent()->headYaw = rot.y;
	}

private:
	std::vector<Actor*> targets;
	Vector2<float> rot{};
	float range = 5.f;
	int delay = 5;
	int tickCounter = 0;
	bool shouldRotate = false;
	bool strafe = false;
};
