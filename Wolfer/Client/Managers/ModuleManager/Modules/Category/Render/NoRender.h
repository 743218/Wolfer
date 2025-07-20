#pragma once
#include "../../ModuleBase/Module.h"

class NoRender : public Module {
public:
    bool noFire = false;
    bool noWeather = false;
    bool noEntities = false;
    bool noBlockEntities = false;
    bool noParticles = false;

    bool* noWeatherBoolPtr = nullptr;
    bool* noEntitiesBoolPtr = nullptr;
    bool* noBlockEntitiesBoolPtr = nullptr;
    bool* noParticlesBoolPtr = nullptr;

    NoRender()
        : Module("NoRender", "Don't render thing", Category::RENDER)
    {
        registerSetting(new BoolSetting("Fire", "NULL", &noFire, noFire));
        registerSetting(new BoolSetting("Weather", "Disable render weather", &noWeather, noWeather));
        registerSetting(new BoolSetting("Entities", "Disable render entities", &noEntities, noEntities));
        registerSetting(new BoolSetting("Block Entities", "Disable render block entities", &noBlockEntities, noBlockEntities));
        registerSetting(new BoolSetting("Particles", "Disable render particles", &noParticles, noParticles));
    }

    void onDisable() override {
        if (noWeatherBoolPtr != nullptr)
            *noWeatherBoolPtr = false;
        if (noEntitiesBoolPtr != nullptr)
            *noEntitiesBoolPtr = false;
        if (noBlockEntitiesBoolPtr != nullptr)
            *noBlockEntitiesBoolPtr = false;
        if (noParticlesBoolPtr != nullptr)
            *noParticlesBoolPtr = false;
    }

    void onClientTick() override {
        if (noWeatherBoolPtr != nullptr)
            *noWeatherBoolPtr = noWeather;
        if (noEntitiesBoolPtr != nullptr)
            *noEntitiesBoolPtr = noEntities;
        if (noBlockEntitiesBoolPtr != nullptr)
            *noBlockEntitiesBoolPtr = noBlockEntities;
        if (noParticlesBoolPtr != nullptr)
            *noParticlesBoolPtr = noParticles;
    }
};
