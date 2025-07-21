#pragma once
#include "../../ModuleBase/Module.h"

class ItemESP : public Module {
public:
    WolferColor color;
    float glowIntensity;
    float pulseSpeed;
    bool enablePulse;
    int shaderMode;

    ItemESP()
        : Module("ItemESP", "Highlights items on the floor", Category::RENDER),
        color(255, 255, 100),
        glowIntensity(0.8f),
        pulseSpeed(2.0f),
        enablePulse(true),
        shaderMode(0)
    {
        registerSetting(new ColorSetting("Color", "Item ESP color", &color, color));
        registerSetting(new SliderSetting("Glow Intensity", "Intensity of the glow effect", &glowIntensity, 0.1f, 2.0f, glowIntensity));
        registerSetting(new SliderSetting("Pulse Speed", "Speed of the pulsing effect", &pulseSpeed, 0.5f, 5.0f, pulseSpeed));
        registerSetting(new BoolSetting("Enable Pulse", "Enable pulsing glow effect", &enablePulse, enablePulse));
        registerSetting(new EnumSetting("Shader Mode", "Type of shader effect", { "Glow", "Outline", "Both" }, &shaderMode, shaderMode));
    }

    void onLevelRender() override;
    void renderItemGlow(const Vector3<float>& itemPos, const AABB& itemAABB, const WolferColor& glowColor, float intensity);
    void renderItemOutline(const Vector3<float>& itemPos, const AABB& itemAABB, const WolferColor& outlineColor);
};