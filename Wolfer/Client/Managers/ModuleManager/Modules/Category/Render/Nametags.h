#pragma once
#include "../../ModuleBase/Module.h"
#include <queue>

struct NameTagRenderData {
    Vector2<float> screenPos;
    std::string text;
    float scale;
};

struct NameTagsTransform {
    std::vector<NameTagRenderData> renderList;
};

class NameTags : public Module {
private:
    int renderMode = 2;
    int opacity = 125;
    bool displayArmor = true;
    WolferColor rectColor = WolferColor(255, 255, 255, 255);
    std::queue<NameTagsTransform> transformList;

public:
    NameTags() : Module("NameTags", "Better nametags", Category::RENDER) {
        registerSetting(new EnumSetting("Mode", "NULL", { "None", "Underline", "Outline" }, &renderMode, 2));
        registerSetting(new SliderSetting<int>("Opacity", "NULL", &opacity, 125, 0, 255));
        registerSetting(new BoolSetting("Armor", "Display armor", &displayArmor, true));
        registerSetting(new ColorSetting("Rect Color", "NULL", &rectColor, rectColor));
    }

    void Render() {
        if (!g_Data.canUseMoveKeys()) return;
        if (g_Data.clientInstance->getLevelRenderer() == nullptr) return;

        LocalPlayer* localPlayer = g_Data.getLocalPlayer();
        if (localPlayer == nullptr) return;

        Level* level = localPlayer->level;
        if (level == nullptr) return;

        Vector3<float> origin = MCR::origin;

        NameTagsTransform transform;

        for (auto& entity : ActorUtils::getActorList(false, false)) {
            if (!TargetUtil::isTargetValid(entity, false, false, 120.f)) continue;

            Vector2<float> textPos;
            Vector3<float> entityPos = entity->getEyePos();

            if (entity->getActorTypeComponent()->id != 319)
                entityPos.y += entity->aabbShape->height;

            float renderOffsetY = 0.75f;

            if (MCR::worldToScreen(entityPos.add(Vector3<float>(0.f, renderOffsetY, 0.f)), textPos)) {
                float dist = origin.dist(entityPos);
                float textSize = fmax(0.5f, fmin(2.f, 3.f / dist));
                float textPaddingX = 2.f * textSize;
                float textPaddingY = 1.f * textSize;

                std::string entityName = WorldUtil::getEntityNameTags(entity);

                float textWidth = MCR::getTextWidth(entityName, textSize);
                float textHeight = 9.f * textSize;

                textPos.x -= textWidth / 2.f;
                textPos.y -= textHeight / 2.f;

                Vector4<float> rectPos = Vector4<float>(textPos.x - textPaddingX,
                    textPos.y - textPaddingY,
                    textPos.x + textWidth + textPaddingX,
                    textPos.y + textHeight + textPaddingY);

                NameTagRenderData renderData;
                renderData.screenPos = Vector2<float>(textPos.x + (textWidth / 2.f), rectPos.y);
                renderData.text = entityName;
                renderData.scale = textSize;
                transform.renderList.push_back(renderData);

                if (displayArmor) {
                    float scale = 0.85f * textSize;
                    float spacing = 15.f * scale;

                    Vector2<float> armorHudPos = Vector2<float>((rectPos.x + rectPos.z) / 2.f, rectPos.y - 17.f * scale);

                    ItemStack* armorItemStack[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

                    ItemStack* offhandItem = entity->getOffhandSlot();
                    if (offhandItem->isValid()) {
                        armorItemStack[0] = offhandItem;
                        armorHudPos.x -= (spacing / 2.f);
                    }

                    for (int i = 0; i < 4; i++) {
                        ItemStack* stack = entity->getArmor(i);
                        if (stack->isValid()) {
                            armorItemStack[i + 1] = stack;
                            armorHudPos.x -= (spacing / 2.f);
                        }
                    }

                    ItemStack* holdingItem = entity->getCarriedItem();
                    if (holdingItem->isValid()) {
                        armorItemStack[5] = holdingItem;
                        armorHudPos.x -= (spacing / 2.f);
                    }

                    for (int i = 0; i < 6; i++) {
                        if (armorItemStack[i] != nullptr) {
                            MCR::drawItem(armorHudPos, armorItemStack[i], scale, true);
                            uint8_t itemCount = armorItemStack[i]->mCount;
                            if (itemCount > 1) {
                                Vector2<float> itemCountPos = Vector2<float>(armorHudPos.x + (17.f * scale), armorHudPos.y + (8.f * scale));
                                std::string itemCountText = std::to_string(itemCount);
                                itemCountPos.x -= MCR::getTextWidth(itemCountText, scale);
                                MCR::drawText(itemCountPos, itemCountText, WolferColor(255, 255, 255), scale);
                            }
                            armorHudPos.x += spacing;
                        }
                    }
                }
            }
        }

        transformList.push(transform);
    }

    void RenderD2D() {
        if (!g_Data.canUseMoveKeys()) return;

        LocalPlayer* localPlayer = g_Data.getLocalPlayer();
        if (localPlayer == nullptr) return;

        Vector2<float> windowSize = g_Data.clientInstance->guiData->windowSizeReal;
        Vector2<float> windowScaled = g_Data.clientInstance->guiData->windowSizeScaled;

        static NameTagsTransform currentTransform;

        while (transformList.size() > 1) {
            currentTransform = transformList.front();
            transformList.pop();
        }

        for (auto& renderData : currentTransform.renderList) {
            float textSize = renderData.scale;
            float textPaddingX = 3.f * textSize;
            float textPaddingY = 0.f * textSize;

            std::string entityName = renderData.text;

            float textWidth = D2D::getTextWidth(entityName, textSize);
            float textHeight = D2D::getTextHeight(entityName, textSize);

            Vector2<float> renderPos;
            renderPos.x = renderData.screenPos.x * (windowSize.x / windowScaled.x);
            renderPos.y = renderData.screenPos.y * (windowSize.y / windowScaled.y);

            Vector4<float> rectPos = Vector4<float>(renderPos.x - (textWidth / 2.f) - textPaddingX,
                renderPos.y,
                renderPos.x + (textWidth / 2.f) + textPaddingX,
                renderPos.y + textHeight + (textPaddingY * 2.f));

            Vector4<float> underlineRect = Vector4<float>(rectPos.x, rectPos.w, rectPos.z, rectPos.w + 2.f * textSize);

            Vector2<float> textPos = Vector2<float>(rectPos.x + textPaddingX, rectPos.y + textPaddingY);

            D2D::fillRectangle(rectPos, WolferColor(0, 0, 0, opacity));

            if (renderMode == 1)
                D2D::fillRectangle(underlineRect, rectColor);
            else if (renderMode == 2)
                D2D::drawRectangle(rectPos, rectColor, 1.f * textSize);

            if (!FriendManager::isInList(entityName))
                D2D::drawText(textPos, entityName, WolferColor(255, 255, 255), textSize);
            else
                D2D::drawText(textPos, entityName, WolferColor(0, 255, 255), textSize);
        }
    }
};
