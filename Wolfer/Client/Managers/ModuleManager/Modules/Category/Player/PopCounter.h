#pragma once
#include "../../ModuleBase/Module.h"
#include <unordered_map>
#include <vector>
#include <string>

class PopCounter : public Module {
private:
    std::unordered_map<std::string, bool> hadTotem;
    std::unordered_map<std::string, int> popCount;

public:
    PopCounter() : Module("PopCounter", "Simple totem pop tracker", Category::PLAYER) {}

    void onTick(GameMode* gm) override {
        LocalPlayer* localPlayer = g_Data.getLocalPlayer();

        if (!localPlayer || !localPlayer->level) return;

        std::vector<Actor*> players = localPlayer->level->getRuntimeActorList();
        players.push_back(g_Data.getLocalPlayer());

        for (Actor* actor : players) {
            if (!actor) continue;
            if (actor->getActorTypeComponent()->id != 319) continue;

            std::string name = *actor->getNameTag();
            if (name.empty()) continue;

            bool hasTotem = actor->getOffhandSlot()->item.get() != nullptr;

            if (hadTotem[name] && !hasTotem) {
                popCount[name]++;
                int count = popCount[name];

                g_Data.DisplayClientMessage(
                    "%sWolfer %s%s %spopped %s%d %stotem%s",
                    MCTF::RED,
                    MCTF::AQUA, name.c_str(),
                    MCTF::GRAY, MCTF::RED, count,
                    MCTF::GRAY, count == 1 ? "" : "s"
                );
            }

            hadTotem[name] = hasTotem;
        }
    }
};
