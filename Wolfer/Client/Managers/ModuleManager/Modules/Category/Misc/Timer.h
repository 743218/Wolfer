#pragma once
#include "../../ModuleBase/Module.h"

class Timer : public Module {
private:
    float timerValue = 40;
    int mode = 0;
    float currentTPS = 20.f;

public:
    Timer() : Module("Timer", "Modify tps of game", Category::MISC) {
        registerSetting(new SliderSetting<float>("TPS", "ticks per second", &timerValue, 24, 1, 50));
        registerSetting(new EnumSetting("Mode", "Mode for timer", { "Regular", "TickShift" }, &mode, 0));
    }

    std::string getModeText() override {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(1) << currentTPS;
        return ss.str() + " tps";
    }

    void onDisable() override {
        Minecraft* mc = g_Data.clientInstance->minecraft;
        if (mc != nullptr) {
            *mc->minecraftTimer = 20.0f;
            *mc->minecraftRenderTimer = 20.0f;
            currentTPS = 20.0f;
        }
    }

    void onClientTick() override {
        Minecraft* mc = g_Data.clientInstance->minecraft;
        if (mc != nullptr) {
            if (mode == 0) {
                currentTPS = timerValue;
            }
            else if (mode == 1) {
                static std::mt19937 rng((unsigned int)std::chrono::steady_clock::now().time_since_epoch().count());
                float min = 20.0f;
                float max = std::max(timerValue, 24.1f);
                std::uniform_real_distribution<float> dist(0.f, 1.f);
                float skewed = dist(rng);
                skewed *= skewed;
                currentTPS = min + (max - min) * skewed;
            }

            *mc->minecraftTimer = currentTPS;
            *mc->minecraftRenderTimer = currentTPS;
        }
    }
};
