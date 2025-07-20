#pragma once
#include "../../ModuleBase/Module.h"

class Aura : public Module {
public:
	Aura() : Module("Aura", "Attacks nearby entities", Category::PLAYER) {
		registerSetting(new SliderSetting<float>("Reach", "Attack range", &range, 5.f, 3.f, 15.f));
		registerSetting(new SliderSetting<int>("Delay", "Attack delay in ticks", &delay, 5, 0, 20));
		registerSetting(new BoolSetting("Strafe", "Strafe around the target, because thats tuff", &strafe, false));
	}

	void onEnable() override {
		tickCounter = delay;
	}

	void onTick(GameMode* gm) override {
		LocalPlayer* localPlayer = g_Data.getLocalPlayer();

		if (!localPlayer) return;
		Level* level = localPlayer->level;
		if (!level) return;

		if (++tickCounter < delay) return;
		tickCounter = 0;

		for (auto& entity : level->getRuntimeActorList()) {
			if (!entity || !TargetUtil::isTargetValid(entity, true, true)) continue;

			if (WorldUtil::distanceToEntity(localPlayer->getPos(), entity) <= range) {
				targets.push_back(entity);
			}
		}

		if (targets.empty()) return;

		Actor* target = targets[0];
		if (!target) return;

		Vector3<float> eyePos = localPlayer->getEyePos();
		Vector3<float> targetPos = target->getEyePos();
		rot = eyePos.CalcAngle(targetPos);

		for (Actor* t : targets) {
			localPlayer->gameMode->attack(t);
			localPlayer->swing();
		}

		targets.clear();
		shouldRotate = true;
	}

	void onSendPacket(Packet* packet) override {
		if (!shouldRotate || !packet || packet->getId() != PacketID::PlayerAuthInput) return;
		if (!targets.empty()) return;

		PlayerAuthInputPacket* paip = static_cast<PlayerAuthInputPacket*>(packet);
		paip->rotation.y = rot.y;
		paip->headYaw = rot.y;
		paip->rotation.x = rot.x;
		shouldRotate = false;
	}

	void onUpdateRotation(LocalPlayer* localPlayer) override {
		if (localPlayer && strafe) {
			if (!targets.empty()) return;
			localPlayer->rotation->presentRot = rot;
			localPlayer->getActorHeadRotationComponent()->headYaw = rot.y; //fun fact! ALWAYS set getActorHeadRotation. if you dont you get a broken looking rotation. this makes it #fancy
		}
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
