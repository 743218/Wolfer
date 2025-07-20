#pragma once

#include "../../ModuleBase/Module.h"
#include "../../../ModuleManager.h"

class Hud : public Module {
public:
    WolferColor color = WolferColor(115, 135, 255);

    Hud() : Module("Hud", "", Category::CLIENT) {
        registerSetting(new ColorSetting("Color", "", &color, color, false));
    }

    void onD2DRender() override {
        LocalPlayer* player = g_Data.getLocalPlayer();
        if (!player) return;
        if (!g_Data.getClientInstance()) return;
        if (!g_Data.canUseMoveKeys()) return;

        Vector2<float> winSize = g_Data.clientInstance->guiData->windowSizeReal;
        Vector2<float> winReal = g_Data.clientInstance->guiData->windowSizeReal;

        float textSize = 1.5f;
        float padX = 2.f;
        float padY = 2.f;
        float textHeight = D2D::getTextHeight("A", textSize);
        Vector2<float> basePos(winSize.x - 2.f, winSize.y - 2.f);

        static std::map<Module*, Vector2<float>> animMap;

        std::vector<Module*> list = ModuleManager::moduleList;

        std::sort(list.begin(), list.end(), [](Module* lhs, Module* rhs) {
            float textSize = 1.f;
            float widthL = D2D::getTextWidth(lhs->getModuleName(), textSize);
            float widthR = D2D::getTextWidth(rhs->getModuleName(), textSize);
            return widthL > widthR;
            });

        float yOffset = 0.f;

        const float lerpInSpeed = 0.1f;
        const float lerpOutSpeed = 0.05f;

        for (auto& mod : list) {
            std::string name = mod->getModuleName();
            std::string display = name + ">";
            float fullWidth = D2D::getTextWidth(display, textSize);
            float nameWidth = D2D::getTextWidth(name, textSize);

            Vector2<float> targetPos(basePos.x - padX - fullWidth, basePos.y - textHeight - yOffset);
            Vector2<float>& animPos = animMap[mod];

            if (animPos.x == 0 && animPos.y == 0)
                animPos = targetPos;

            bool enabled = mod->isEnabled() && mod->isVisible();

            if (enabled) {
                animPos.x += (targetPos.x - animPos.x) * lerpInSpeed;
                animPos.y += (targetPos.y - animPos.y) * lerpInSpeed;

                Vector2<float> drawPos(animPos.x * (winReal.x / winSize.x), animPos.y * (winReal.y / winSize.y));
                D2D::drawText(drawPos, name, color, textSize);
                D2D::drawText(Vector2<float>(drawPos.x + nameWidth, drawPos.y), ">", WolferColor(150, 150, 150), textSize);

                yOffset += textHeight;
            }
            else {
                Vector2<float> offscreenPos(basePos.x + 20.f, animPos.y);
                animPos.x += (offscreenPos.x - animPos.x) * lerpOutSpeed;

                if (std::abs(animPos.x - offscreenPos.x) > 1.0f) {
                    Vector2<float> drawPos(animPos.x * (winReal.x / winSize.x), animPos.y * (winReal.y / winSize.y));
                    D2D::drawText(drawPos, name, color, textSize);
                    D2D::drawText(Vector2<float>(drawPos.x + nameWidth, drawPos.y), ">", WolferColor(150, 150, 150), textSize);
                }
            }
        }

        Vector3 pos = player->getPos();

        std::string xyzLabel = "XYZ:";
        WolferColor gray(128, 128, 128);
        WolferColor white(255, 255, 255);

        float labelWidth = D2D::getTextWidth(xyzLabel, textSize);
        Vector2<float> posLabelPos(padX, winSize.y - textHeight - padY);
        Vector2<float> posLabelPosReal(posLabelPos.x * (winReal.x / winSize.x), posLabelPos.y * (winReal.y / winSize.y));
        D2D::drawText(posLabelPosReal, xyzLabel, gray, textSize);

        std::string xStr = std::to_string((int)pos.x);
        std::string yStr = std::to_string((int)pos.y);
        std::string zStr = std::to_string((int)pos.z);
        std::string comma = ", ";

        float xWidth = D2D::getTextWidth(xStr, textSize);
        float commaWidth = D2D::getTextWidth(comma, textSize);
        float yWidth = D2D::getTextWidth(yStr, textSize);
        float zWidth = D2D::getTextWidth(zStr, textSize);

        float startX = posLabelPos.x + labelWidth;
        float startXReal = startX * (winReal.x / winSize.x);
        float posYReal = posLabelPos.y * (winReal.y / winSize.y);

        D2D::drawText(Vector2<float>(startXReal, posYReal), " [", gray, textSize);
        startX += D2D::getTextWidth(" [", textSize);
        startXReal = startX * (winReal.x / winSize.x);

        D2D::drawText(Vector2<float>(startXReal, posYReal), xStr, white, textSize);
        startX += xWidth;
        startXReal = startX * (winReal.x / winSize.x);

        D2D::drawText(Vector2<float>(startXReal, posYReal), comma, gray, textSize);
        startX += commaWidth;
        startXReal = startX * (winReal.x / winSize.x);

        D2D::drawText(Vector2<float>(startXReal, posYReal), yStr, white, textSize);
        startX += yWidth;
        startXReal = startX * (winReal.x / winSize.x);

        D2D::drawText(Vector2<float>(startXReal, posYReal), comma, gray, textSize);
        startX += commaWidth;
        startXReal = startX * (winReal.x / winSize.x);

        D2D::drawText(Vector2<float>(startXReal, posYReal), zStr, white, textSize);
        startX += zWidth;
        startXReal = startX * (winReal.x / winSize.x);

        D2D::drawText(Vector2<float>(startXReal, posYReal), "]", gray, textSize);

        std::string watermark = "Wolfer";
        float wmWidth = D2D::getTextWidth(watermark, textSize);
        Vector2<float> wmPos(winSize.x - wmWidth - padX, padY);
        Vector2<float> wmPosReal(wmPos.x * (winReal.x / winSize.x), wmPos.y * (winReal.y / winSize.y));
        float wmHeight = D2D::getTextHeight(watermark, textSize);

        D2D::drawText(wmPosReal, watermark, color, textSize);
    }
};
