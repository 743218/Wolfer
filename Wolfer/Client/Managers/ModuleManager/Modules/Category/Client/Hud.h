#pragma once

#include "../../ModuleBase/Module.h"
#include "../../../ModuleManager.h"

class Hud : public Module {
public:
    WolferColor color = WolferColor(115, 135, 255);
    WolferColor waveColor = WolferColor(255, 255, 255);
    int welcomeMode = 0;
    int clientNameMode = 0;

    Hud() : Module("Hud", "Hud for client", Category::CLIENT) {
        registerSetting(new ColorSetting("Color", "NULL", &color, color, false));
        registerSetting(new ColorSetting("Wave Color", "NULL", &waveColor, waveColor, false));
        registerSetting(new EnumSetting("", "Change the welcome message style", {
            "Welcome to", "Wazzup", "Hello", "Hewwo UwU", "Yo", "Haii ^_^", "WolferClient Boot"
            }, &welcomeMode, 0));
        registerSetting(new EnumSetting("", "Client name branding", {
            "Wolfer", "Nuke.jp", "Nuke.us", "NukeV6", "Triangle", "NyawHack", "MewHack",
            "CuteHack", "MelonWare", "Neko+", "3arthc4t", "m4dc3t", "Sirius", "JapHack",
            "Lexical", "MelodyV2", "Melody", "KarrotHax"
            }, &clientNameMode, 0));
    }

    std::string getClientName() {
        static const std::vector<std::string> names = {
            "Wolfer", "Nuke.jp", "Nuke.us", "NukeV6", "Triangle", "NyawHack", "MewHack",
            "CuteHack", "MelonWare", "Neko+", "3arthc4t", "m4dc3t", "Sirius", "JapHack",
            "Lexical", "MelodyV2", "Melody", "KarrotHax"
        };
        if (clientNameMode >= 0 && clientNameMode < names.size())
            return names[clientNameMode];
        return "Wolfer";
    }

    std::string getWelcomeText(const std::string& playerName) {
        std::string name = getClientName();
        switch (welcomeMode) {
        case 0: return "Welcome to " + name;
        case 1: return "Wazzup " + playerName;
        case 2: return "Hello " + name + " user (" + playerName + ")";
        case 3: return "Hewwo " + playerName + " UwU";
        case 4: return "Yo " + playerName;
        case 5: return "Haii ^_^ " + playerName;
        case 6: return name + " Boot Sequence Initiated";
        default: return "Welcome to " + name;
        }
    }

    void onMCRender(MinecraftUIRenderContext* mcr) override {
        LocalPlayer* player = g_Data.getLocalPlayer();
        if (!player) return;
        if (!g_Data.getClientInstance()) return;
        if (!g_Data.canUseMoveKeys()) return;

        Vector2<float> winSize = g_Data.clientInstance->guiData->windowSizeScaled;
        Vector2<float> winReal = g_Data.clientInstance->guiData->windowSizeScaled;

        float textSize = 1.5f;
        float pad = 1.0f;
        float textHeight = MCR::getTextHeight(textSize);
        Vector2<float> basePos(winSize.x, winSize.y);

        static std::map<Module*, Vector2<float>> animMap;
        std::vector<Module*> list = ModuleManager::moduleList;
        WolferColor waveNoIndex = Colors::getWaveColor(color, waveColor, 100);

        std::sort(list.begin(), list.end(), [](Module* lhs, Module* rhs) {
            float textSize = 1.f;
            return MCR::getTextWidth(lhs->getModuleName(), textSize) > MCR::getTextWidth(rhs->getModuleName(), textSize);
            });

        float yOffset = 0.f;
        long index = 0;

        const float lerpInSpeed = 0.1f;
        const float lerpOutSpeed = 0.05f;

        for (auto& mod : list) {
            std::string name = mod->getModuleName();
            std::string display = name + ">";
            float fullWidth = MCR::getTextWidth(display, textSize);
            float nameWidth = MCR::getTextWidth(name, textSize);

            Vector2<float> targetPos(basePos.x - pad - fullWidth, basePos.y - yOffset - textHeight);
            Vector2<float>& animPos = animMap[mod];

            if (animPos.x == 0 && animPos.y == 0)
                animPos = targetPos;

            bool enabled = mod->isEnabled() && mod->isVisible();
            WolferColor wave = Colors::getWaveColor(color, waveColor, index * 20);

            if (enabled) {
                animPos.x += (targetPos.x - animPos.x) * lerpInSpeed;
                animPos.y += (targetPos.y - animPos.y) * lerpInSpeed;

                Vector2<float> drawPos(animPos.x * (winReal.x / winSize.x), animPos.y * (winReal.y / winSize.y));
                Vector4<float> bgRect(drawPos.x - pad, drawPos.y - pad, drawPos.x + fullWidth + pad, drawPos.y + textHeight + pad);
                MCR::fillRectangle(bgRect, WolferColor(0, 0, 0, 100));

                MCR::drawText(drawPos, name, wave, textSize);
                MCR::drawText(Vector2<float>(drawPos.x + nameWidth, drawPos.y), ">", WolferColor(150, 150, 150), textSize);

                yOffset += textHeight + (pad * 2.f);
            }
            else {
                Vector2<float> offscreenPos(basePos.x + 20.f, animPos.y);
                animPos.x += (offscreenPos.x - animPos.x) * lerpOutSpeed;

                if (std::abs(animPos.x - offscreenPos.x) > 1.0f) {
                    Vector2<float> drawPos(animPos.x * (winReal.x / winSize.x), animPos.y * (winReal.y / winSize.y));
                    Vector4<float> bgRect(drawPos.x - pad, drawPos.y - pad, drawPos.x + fullWidth + pad, drawPos.y + textHeight + pad);
                    MCR::fillRectangle(bgRect, WolferColor(0, 0, 0, 100));
                    MCR::drawText(drawPos, name, wave, textSize);
                    MCR::drawText(Vector2<float>(drawPos.x + nameWidth, drawPos.y), ">", WolferColor(150, 150, 150), textSize);
                }
            }

            index++;
        }

        Vector3 pos = player->getPos();
        std::string xyzLabel = "XYZ:";
        WolferColor gray(128, 128, 128);
        WolferColor white(255, 255, 255);

        float labelWidth = MCR::getTextWidth(xyzLabel, textSize);
        Vector2<float> posLabelPos(pad, winSize.y - textHeight - pad);
        Vector2<float> posLabelPosReal(posLabelPos.x * (winReal.x / winSize.x), posLabelPos.y * (winReal.y / winSize.y));

        std::string xStr = std::to_string((int)pos.x);
        std::string yStr = std::to_string((int)pos.y);
        std::string zStr = std::to_string((int)pos.z);
        std::string comma = ", ";

        float totalWidth =
            labelWidth + MCR::getTextWidth(" [", textSize) +
            MCR::getTextWidth(xStr, textSize) + MCR::getTextWidth(comma, textSize) +
            MCR::getTextWidth(yStr, textSize) + MCR::getTextWidth(comma, textSize) +
            MCR::getTextWidth(zStr, textSize) + MCR::getTextWidth("]", textSize);

        Vector4<float> xyzBg(
            posLabelPosReal.x - pad,
            posLabelPosReal.y - pad,
            posLabelPosReal.x + totalWidth + pad,
            posLabelPosReal.y + textHeight + pad);
        MCR::fillRectangle(xyzBg, WolferColor(0, 0, 0, 100));

        MCR::drawText(posLabelPosReal, xyzLabel, waveNoIndex, textSize);

        float startX = posLabelPos.x + labelWidth;
        float startXReal = startX * (winReal.x / winSize.x);
        float posYReal = posLabelPos.y * (winReal.y / winSize.y);

        MCR::drawText(Vector2<float>(startXReal, posYReal), " [", waveNoIndex, textSize);
        startX += MCR::getTextWidth(" [", textSize);
        startXReal = startX * (winReal.x / winSize.x);

        MCR::drawText(Vector2<float>(startXReal, posYReal), xStr, white, textSize);
        startX += MCR::getTextWidth(xStr, textSize);
        startXReal = startX * (winReal.x / winSize.x);

        MCR::drawText(Vector2<float>(startXReal, posYReal), comma, gray, textSize);
        startX += MCR::getTextWidth(comma, textSize);
        startXReal = startX * (winReal.x / winSize.x);

        MCR::drawText(Vector2<float>(startXReal, posYReal), yStr, white, textSize);
        startX += MCR::getTextWidth(yStr, textSize);
        startXReal = startX * (winReal.x / winSize.x);

        MCR::drawText(Vector2<float>(startXReal, posYReal), comma, gray, textSize);
        startX += MCR::getTextWidth(comma, textSize);
        startXReal = startX * (winReal.x / winSize.x);

        MCR::drawText(Vector2<float>(startXReal, posYReal), zStr, white, textSize);
        startX += MCR::getTextWidth(zStr, textSize);
        startXReal = startX * (winReal.x / winSize.x);

        MCR::drawText(Vector2<float>(startXReal, posYReal), "]", waveNoIndex, textSize);

        std::string watermark = getClientName();
        float wmWidth = MCR::getTextWidth(watermark, textSize);
        Vector2<float> wmPos(winSize.x - wmWidth - pad, pad);
        Vector2<float> wmPosReal(wmPos.x * (winReal.x / winSize.x), wmPos.y * (winReal.y / winSize.y));
        Vector4<float> wmBg(wmPosReal.x - pad, wmPosReal.y - pad, wmPosReal.x + wmWidth + pad, wmPosReal.y + textHeight + pad);
        MCR::fillRectangle(wmBg, WolferColor(0, 0, 0, 100));
        MCR::drawText(wmPosReal, watermark, waveNoIndex, textSize);

        std::string welcome = getWelcomeText(*player->getNameTag());
        float welcomeWidth = MCR::getTextWidth(welcome, textSize);
        Vector2<float> welcomePos((winSize.x - welcomeWidth) / 2.f, pad);
        Vector2<float> welcomeReal(welcomePos.x * (winReal.x / winSize.x), welcomePos.y * (winReal.y / winSize.y));
        Vector4<float> welcomeBg(welcomeReal.x - pad, welcomeReal.y - pad, welcomeReal.x + welcomeWidth + pad, welcomeReal.y + textHeight + pad);
        MCR::fillRectangle(welcomeBg, WolferColor(0, 0, 0, 100));
        MCR::drawText(welcomeReal, welcome, waveNoIndex, textSize);
    }
};
