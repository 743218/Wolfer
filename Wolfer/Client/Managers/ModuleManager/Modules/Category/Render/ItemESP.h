#pragma once
#include "../../ModuleBase/Module.h"
#include <cmath>

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

    void onLevelRender() override {
        LocalPlayer* localPlayer = g_Data.getLocalPlayer();
        if (!localPlayer) return;
        Level* level = localPlayer->level;
        if (!level) return;
        Vector3<float> playerPos = localPlayer->getPos();
        static float timeAccumulator = 0.0f;
        timeAccumulator += MCR::deltaTime;
        float pulseMultiplier = 1.0f;
        if (enablePulse) {
            pulseMultiplier = 0.5f + 0.5f * sinf(timeAccumulator * pulseSpeed);
        }
        for (auto entity : level->getRuntimeActorList()) {
            if (!entity) continue;
            uint32_t entityId = entity->getActorTypeComponent()->id;
            if (entityId != 64) continue;
            Vector3<float> entityPos = entity->getPos();
            float dx = entityPos.x - playerPos.x;
            float dy = entityPos.y - playerPos.y;
            float dz = entityPos.z - playerPos.z;
            float distSq = dx * dx + dy * dy + dz * dz;
            if (distSq > 10000.0f) continue;
            AABB entityAABB = entity->aabbShape->aabb;
            float currentIntensity = glowIntensity * pulseMultiplier;
            WolferColor dynamicColor(
                color.r,
                color.g,
                color.b,
                static_cast<int>(255 * currentIntensity)
            );
            switch (shaderMode) {
            case 0:
                renderItemGlow(entityPos, entityAABB, dynamicColor, currentIntensity);
                break;
            case 1:
                renderItemOutline(entityPos, entityAABB, dynamicColor);
                break;
            case 2:
                renderItemGlow(entityPos, entityAABB, dynamicColor, currentIntensity);
                renderItemOutline(entityPos, entityAABB, dynamicColor);
                break;
            }
        }
    }

    void renderItemGlow(const Vector3<float>& itemPos, const AABB& itemAABB, const WolferColor& glowColor, float intensity) {
        if (!MCR::tessellator || !MCR::screenCtx || !MCR::blendMaterial) return;
        Vector3<float> origin = MCR::origin;
        Vector3<float> localPos = {
            itemPos.x - origin.x,
            itemPos.y - origin.y,
            itemPos.z - origin.z
        };
        Vector3<float> size = {
            itemAABB.upper.x - itemAABB.lower.x,
            itemAABB.upper.y - itemAABB.lower.y,
            itemAABB.upper.z - itemAABB.lower.z
        };
        const int glowLayers = 3;
        for (int layer = 0; layer < glowLayers; layer++) {
            float layerScale = 1.0f + (layer * 0.2f * intensity);
            float layerAlpha = (glowColor.a / 255.0f) * (1.0f - (layer * 0.3f));
            WolferColor layerColor(glowColor.r, glowColor.g, glowColor.b, static_cast<int>(255 * layerAlpha));
            MCR::setColor(layerColor);
            MCR::tessellator->begin(VertextFormat::TRIANGLE_STRIP);
            Vector3<float> expandedSize = {
                size.x * layerScale, size.y * layerScale, size.z * layerScale
            };
            Vector3<float> halfSize = {
                expandedSize.x * 0.5f, expandedSize.y * 0.5f, expandedSize.z * 0.5f
            };
            MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y - halfSize.y, localPos.z + halfSize.z);
            MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y - halfSize.y, localPos.z + halfSize.z);
            MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y + halfSize.y, localPos.z + halfSize.z);
            MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y + halfSize.y, localPos.z + halfSize.z);
            MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);
            MCR::tessellator->begin(VertextFormat::TRIANGLE_STRIP);
            MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y - halfSize.y, localPos.z - halfSize.z);
            MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y - halfSize.y, localPos.z - halfSize.z);
            MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y + halfSize.y, localPos.z - halfSize.z);
            MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y + halfSize.y, localPos.z - halfSize.z);
            MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);
            MCR::tessellator->begin(VertextFormat::TRIANGLE_STRIP);
            MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y - halfSize.y, localPos.z - halfSize.z);
            MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y - halfSize.y, localPos.z + halfSize.z);
            MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y + halfSize.y, localPos.z - halfSize.z);
            MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y + halfSize.y, localPos.z + halfSize.z);
            MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);
            MCR::tessellator->begin(VertextFormat::TRIANGLE_STRIP);
            MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y - halfSize.y, localPos.z + halfSize.z);
            MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y - halfSize.y, localPos.z - halfSize.z);
            MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y + halfSize.y, localPos.z + halfSize.z);
            MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y + halfSize.y, localPos.z - halfSize.z);
            MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);
            MCR::tessellator->begin(VertextFormat::TRIANGLE_STRIP);
            MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y + halfSize.y, localPos.z + halfSize.z);
            MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y + halfSize.y, localPos.z + halfSize.z);
            MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y + halfSize.y, localPos.z - halfSize.z);
            MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y + halfSize.y, localPos.z - halfSize.z);
            MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);
            MCR::tessellator->begin(VertextFormat::TRIANGLE_STRIP);
            MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y - halfSize.y, localPos.z - halfSize.z);
            MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y - halfSize.y, localPos.z - halfSize.z);
            MCR::tessellator->vertex(localPos.x - halfSize.x, localPos.y - halfSize.y, localPos.z + halfSize.z);
            MCR::tessellator->vertex(localPos.x + halfSize.x, localPos.y - halfSize.y, localPos.z + halfSize.z);
            MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);
        }
    }

    void renderItemOutline(const Vector3<float>& itemPos, const AABB& itemAABB, const WolferColor& outlineColor) {
        if (!MCR::tessellator || !MCR::screenCtx || !MCR::blendMaterial) return;
        Vector3<float> origin = MCR::origin;
        Vector3<float> localPos = {
            itemPos.x - origin.x,
            itemPos.y - origin.y,
            itemPos.z - origin.z
        };
        Vector3<float> size = {
            itemAABB.upper.x - itemAABB.lower.x,
            itemAABB.upper.y - itemAABB.lower.y,
            itemAABB.upper.z - itemAABB.lower.z
        };
        Vector3<float> halfSize = {
            size.x * 0.5f, size.y * 0.5f, size.z * 0.5f
        };
        MCR::setColor(outlineColor);
        MCR::tessellator->begin(VertextFormat::LINE_LIST);
        Vector3<float> v[8] = {
            { localPos.x - halfSize.x, localPos.y - halfSize.y, localPos.z - halfSize.z },
            { localPos.x + halfSize.x, localPos.y - halfSize.y, localPos.z - halfSize.z },
            { localPos.x + halfSize.x, localPos.y + halfSize.y, localPos.z - halfSize.z },
            { localPos.x - halfSize.x, localPos.y + halfSize.y, localPos.z - halfSize.z },
            { localPos.x - halfSize.x, localPos.y - halfSize.y, localPos.z + halfSize.z },
            { localPos.x + halfSize.x, localPos.y - halfSize.y, localPos.z + halfSize.z },
            { localPos.x + halfSize.x, localPos.y + halfSize.y, localPos.z + halfSize.z },
            { localPos.x - halfSize.x, localPos.y + halfSize.y, localPos.z + halfSize.z }
        };
        int edges[][2] = {
            {0,1},{1,2},{2,3},{3,0},
            {4,5},{5,6},{6,7},{7,4},
            {0,4},{1,5},{2,6},{3,7}
        };
        for (auto& e : edges) {
            MCR::tessellator->vertex(v[e[0]].x, v[e[0]].y, v[e[0]].z);
            MCR::tessellator->vertex(v[e[1]].x, v[e[1]].y, v[e[1]].z);
        }
        MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);
    }
};
