#pragma once
#include "../../ModuleBase/Module.h"

class SMTeleport : public Module {
private:
    float speed = 7.5f;
    float arrivalThreshold = 1.0f;
    bool timerSet = false;

    Vector3<float> targetPos = Vector3<float>(0.f, 0.f, 0.f);

public:
    SMTeleport() : Module("SMTeleport", "Smoothly teleports to a target using setPos()", Category::MOVEMENT) {
        registerSetting(new SliderSetting<float>("Speed", "Movement speed", &speed, 7.5f, 0.1f, 10.f));
        registerSetting(new SliderSetting<float>("Threshold", "Arrival distance", &arrivalThreshold, 1.0f, 0.1f, 5.0f));
    }

    void onEnable() override {
        auto player = g_Data.getLocalPlayer();
        if (player)
            player->displayClientMessage("Use the command .setPos <x> <y> <z> to set a target!");

        Minecraft* mc = g_Data.clientInstance->minecraft;
        if (mc != nullptr) {
            *mc->minecraftTimer = 500.f;
            *mc->minecraftRenderTimer = 500.f;
            timerSet = true;
        }
    }

    void onDisable() override {
        Minecraft* mc = g_Data.clientInstance->minecraft;
        if (mc != nullptr && timerSet) {
            *mc->minecraftTimer = 20.f;
            *mc->minecraftRenderTimer = 20.f;
        }
        timerSet = false;

        auto player = g_Data.getLocalPlayer();
        if (player)
            player->stateVector->velocity = Vector3<float>(0.f, 0.f, 0.f);
    }

    void onTick(GameMode* gm) override {
        auto player = g_Data.getLocalPlayer();
        if (!player) return;

        player->stateVector->velocity = Vector3<float>(0.f, 0.f, 0.f);
        Vector3<float> pos = player->getPos();
        Vector3<float> delta = targetPos.sub(pos);

        float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
        if (distance < arrivalThreshold) {
            player->displayClientMessage("Arrived near target!");
            setEnabled(false);
            return;
        }

        float invDist = 1.0f / distance;
        Vector3<float> dir(delta.x * invDist, delta.y * invDist, delta.z * invDist);

        Vector3<float> newPos(pos.x + dir.x * speed, pos.y + dir.y * speed, pos.z + dir.z * speed);
        player->setPos(newPos);
    }

    void setTargetPos(Vector3<float> pos) {
        targetPos = pos;
        auto player = g_Data.getLocalPlayer();
        if (player) {
            std::ostringstream ss;
            ss << "Target position set to: X" << pos.x << " Y" << pos.y << " Z" << pos.z;
            player->displayClientMessage(ss.str());
        }
    }
};