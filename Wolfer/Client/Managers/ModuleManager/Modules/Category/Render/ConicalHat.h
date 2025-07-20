#pragma once

#include "../../ModuleBase/Module.h"

class ConicalHat : public Module {
private:
    WolferColor baseColor = WolferColor(255, 0, 0, 255);
    float sizex = 0.75f;
    float sizey = 0.55f;
    float opacityfill = 0.45f;
    float opacityline = 0.45f;

    void drawLineStrip3d(const std::vector<Vector3<float>>& points, const WolferColor& color) {
        if (points.empty()) return;

        std::vector<Vector3<float>> localPoints;
        localPoints.reserve(points.size());
        for (const auto& p : points) {
            localPoints.emplace_back(
                p.x - MCR::origin.x,
                p.y - MCR::origin.y,
                p.z - MCR::origin.z
            );
        }

        for (size_t i = 1; i < localPoints.size(); ++i) {
            MCR::line3dRenderList.push_back(MCR::Render3dData(localPoints[i - 1], color));
            MCR::line3dRenderList.push_back(MCR::Render3dData(localPoints[i], color));
        }
    }

public:
    ConicalHat() : Module("ConicalHat", "Draws a cone hat on your head", Category::RENDER) {
        registerSetting(new SliderSetting<float>("Width", "Width", &sizex, 0.75f, 0.1f, 1.f));
        registerSetting(new SliderSetting<float>("Height", "Height", &sizey, 0.55f, 0.1f, 1.f));
        registerSetting(new SliderSetting<float>("Opacity", "Opacity", &opacityfill, 0.45f, 0.1f, 1.f));
        registerSetting(new SliderSetting<float>("Line Opacity", "Opacity", &opacityline, 0.45f, 0.1f, 1.f));
        registerSetting(new ColorSetting("Color", "NULL", &baseColor, baseColor, false));
    }

    void render() {
        LocalPlayer* player = g_Data.getLocalPlayer();
        if (!player || !player->aabbShape || !MCR::tessellator || !MCR::screenCtx || !MCR::blendMaterial) return;

        AABB aabb = player->aabbShape->aabb;
        Vector3<float> eyePos = player->getEyePos();
        Vector3<float> origin = g_Data.clientInstance->getLevelRenderer()->levelRendererPlayer->realOrigin;

        float width = aabb.upper.x - aabb.lower.x;
        float radius = width * sizex;
        float height = radius * sizey;
        float yOffset = 0.0f;
        if (player->getMoveInputHandler()->sneaking) {
            yOffset = -0.21f;
        }

        float baseY = (eyePos.y + 0.3f + yOffset) - 0.1f;
        float tipY = baseY + height;
        Vector3<float> tip = { eyePos.x - origin.x, tipY - origin.y, eyePos.z - origin.z };

        const int segments = 72;
        std::vector<Vector3<float>> ringPoints;
        ringPoints.reserve(segments + 1);

        for (int i = 0; i <= segments; ++i) {
            float angle = (i * 2.0f * PI) / segments;
            float x = eyePos.x + radius * cosf(angle);
            float z = eyePos.z + radius * sinf(angle);
            ringPoints.emplace_back(x - origin.x, baseY - origin.y, z - origin.z);
        }

        for (int i = 0; i < segments; ++i) {
            Vector3<float>& p1 = ringPoints[i];
            Vector3<float>& p2 = ringPoints[i + 1];

            WolferColor base(baseColor.r, baseColor.g, baseColor.b, static_cast<uint8_t>(255.f * opacityfill));
            MCR::setColor(base);
            MCR::tessellator->begin(VertextFormat::TRIANGLE_STRIP, 3);
            MCR::tessellator->vertex(p1.x, p1.y, p1.z);
            MCR::tessellator->vertex(tip.x, tip.y, tip.z);
            MCR::tessellator->vertex(p2.x, p2.y, p2.z);
            MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);

            MCR::tessellator->begin(VertextFormat::TRIANGLE_STRIP, 3);
            MCR::tessellator->vertex(p2.x, p2.y, p2.z);
            MCR::tessellator->vertex(tip.x, tip.y, tip.z);
            MCR::tessellator->vertex(p1.x, p1.y, p1.z);
            MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);
        }

        WolferColor base2(baseColor.r, baseColor.g, baseColor.b, static_cast<uint8_t>(255.f * opacityline));
        MCR::setColor(base2);
        MCR::tessellator->begin(VertextFormat::LINE_STRIP, 2);
        for (const auto& p : ringPoints) {
            MCR::tessellator->vertex(p.x, p.y, p.z);
        }
        MeshHelpers::renderMeshImmediately(MCR::screenCtx, MCR::tessellator, MCR::blendMaterial);
    }
};
