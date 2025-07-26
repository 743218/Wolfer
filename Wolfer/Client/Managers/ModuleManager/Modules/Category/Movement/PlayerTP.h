#pragma once
#include "../../ModuleBase/Module.h"

class PlayerTP : public Module {
private:
    float speed = 7.5f;
    float arrivalThreshold = 1.0f;
    bool timerSet = false;

    Vector3<float> targetPos = Vector3<float>(0.f, 0.f, 0.f);

public:
    PlayerTP() : Module("PlayerTP", "Smoothly teleports to nearest player", Category::MOVEMENT) {
        registerSetting(new SliderSetting<float>("Speed", "Movement speed", &speed, 1.5f, 0.1f, 3.f));
        registerSetting(new SliderSetting<float>("Threshold", "Arrival distance", &arrivalThreshold, 1.0f, 0.1f, 5.0f));
    }

    void onEnable() override {
        auto player = g_Data.getLocalPlayer();
        if (!player) {
            setEnabled(false);
            return;
        }

        Actor* closest = nullptr;
        float minDist = FLT_MAX;

        for (auto& entity : ActorUtils::getActorList(false, true)) {
            if (!entity || entity == player) continue;
            
            float dist = player->getPos().dist(entity->getPos());
            if (dist > 130.f) continue;

            if (dist < minDist) {
                minDist = dist;
                closest = entity;
            }
        }

        if (!closest) {
            player->displayClientMessage("No player found in render distance.");
            setEnabled(false);
            return;
        }

        targetPos = closest->getPos();

        Minecraft* mc = g_Data.clientInstance->minecraft;
        if (mc) {
            *mc->minecraftTimer = 725000.f;
            *mc->minecraftRenderTimer = 725000.f;
            timerSet = true;
        }
    }

    void onDisable() override {
        Minecraft* mc = g_Data.clientInstance->minecraft;
        if (mc && timerSet) {
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
};
