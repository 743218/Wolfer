#pragma once
#include "../../ModuleBase/Module.h"

struct BlockPosHash2 {
    size_t operator()(const BlockPos& pos) const noexcept {
        size_t hx = std::hash<int>()(pos.x);
        size_t hy = std::hash<int>()(pos.y);
        size_t hz = std::hash<int>()(pos.z);
        return hx ^ (hy << 1) ^ (hz << 2);
    }
};

struct BlockVisualInfo {
    WolferColor fillColor;
    WolferColor lineColor;
};

class StorageESP : public Module {
public:
    bool tracer2D;

    WolferColor chestFillColor;
    WolferColor chestLineColor;
    WolferColor shulkerFillColor;
    WolferColor shulkerLineColor;

    WolferColor hopperFillColor;
    WolferColor hopperLineColor;
    WolferColor dropperFillColor;
    WolferColor dropperLineColor;
    WolferColor dispenserFillColor;
    WolferColor dispenserLineColor;

    bool showHopper;
    bool showDropper;
    bool showDispenser;

    std::unordered_map<BlockPos, BlockVisualInfo, BlockPosHash2> blockList;

    StorageESP()
        : Module("StorageESP", "ESP for chests, shulkers, and more", Category::RENDER),
        chestFillColor(200, 125, 0, 50),
        chestLineColor(200, 125, 0, 255),
        shulkerFillColor(140, 0, 180, 50),
        shulkerLineColor(140, 0, 180, 255),
        hopperFillColor(128, 128, 128, 50),
        hopperLineColor(128, 128, 128, 255),
        dropperFillColor(192, 192, 192, 50),
        dropperLineColor(192, 192, 192, 255),
        dispenserFillColor(169, 169, 169, 50),
        dispenserLineColor(169, 169, 169, 255),
        showHopper(true),
        showDropper(true),
        showDispenser(true),
        tracer2D(true)
    {
        registerSetting(new ColorSetting("Chest Fill", "Chest fill color", &chestFillColor, chestFillColor));
        registerSetting(new ColorSetting("Chest Line", "Chest line color", &chestLineColor, chestLineColor));
        registerSetting(new ColorSetting("Shulker Fill", "Shulker fill color", &shulkerFillColor, shulkerFillColor));
        registerSetting(new ColorSetting("Shulker Line", "Shulker line color", &shulkerLineColor, shulkerLineColor));
        registerSetting(new ColorSetting("Hopper Fill", "Hopper fill color", &hopperFillColor, hopperFillColor));
        registerSetting(new ColorSetting("Hopper Line", "Hopper line color", &hopperLineColor, hopperLineColor));
        registerSetting(new ColorSetting("Dropper Fill", "Dropper fill color", &dropperFillColor, dropperFillColor));
        registerSetting(new ColorSetting("Dropper Line", "Dropper line color", &dropperLineColor, dropperLineColor));
        registerSetting(new ColorSetting("Dispenser Fill", "Dispenser fill color", &dispenserFillColor, dispenserFillColor));
        registerSetting(new ColorSetting("Dispenser Line", "Dispenser line color", &dispenserLineColor, dispenserLineColor));

        registerSetting(new BoolSetting("Hopper", "Render hoppers", &showHopper, showHopper));
        registerSetting(new BoolSetting("Dropper", "Render droppers", &showDropper, showDropper));
        registerSetting(new BoolSetting("Dispenser", "Render dispensers", &showDispenser, showDispenser));
        registerSetting(new BoolSetting("2D Tracers", "Draw 2D lines to blocks", &tracer2D, tracer2D));
    }

    void onLevelRender() override {
        LocalPlayer* localPlayer = g_Data.getLocalPlayer();
        if (!localPlayer) return;

        Vec3<float> playerPos = localPlayer->getPos();

        for (auto it = blockList.begin(); it != blockList.end();) {
            BlockPos pos = it->first;

            float dx = static_cast<float>(pos.x) + 0.5f - playerPos.x;
            float dy = static_cast<float>(pos.y) + 0.5f - playerPos.y;
            float dz = static_cast<float>(pos.z) + 0.5f - playerPos.z;
            float distSq = dx * dx + dy * dy + dz * dz;

            if (distSq > 90000.f) {
                it = blockList.erase(it);
                continue;
            }

            Block* block = g_Data.clientInstance->getRegion()->getBlock(pos);
            if (!block || !block->blockLegacy || block->blockLegacy->blockId == 0) {
                it = blockList.erase(it);
                continue;
            }

            std::string name = block->blockLegacy->blockName;
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);

            bool valid = (name.find("chest") != std::string::npos && name.find("ender") == std::string::npos)
                || name.find("shulker") != std::string::npos
                || (showHopper && (block->blockLegacy->blockId == 154 || name.find("hopper") != std::string::npos))
                || (showDropper && name.find("dropper") != std::string::npos)
                || (showDispenser && (block->blockLegacy->blockId == 23 || name.find("dispenser") != std::string::npos));

            if (!valid) {
                it = blockList.erase(it);
                continue;
            }

            AABB box;
            block->blockLegacy->getOutline(block, g_Data.clientInstance->getRegion(), &pos, &box);

            MCR::drawBox3dFilled(box, it->second.fillColor, it->second.lineColor, 1.f);

            ++it;
        }
    }

    void onMCRender(MinecraftUIRenderContext* renderCtx) override {
        if (!tracer2D || blockList.empty()) return;
        if (!g_Data.getLocalPlayer()) return;

        Vec2<float> screenSize(
            renderCtx->clientInstance->getguiData()->windowSizeScaled.x,
            renderCtx->clientInstance->getguiData()->windowSizeScaled.y
        );
        Vec2<float> center(screenSize.x / 2.f, screenSize.y / 2.f);

        for (const auto& entry : blockList) {
            const BlockPos& pos = entry.first;
            const BlockVisualInfo& vis = entry.second;

            Block* block = g_Data.clientInstance->getRegion()->getBlock(pos);
            if (!block || !block->blockLegacy) continue;

            std::string name = block->blockLegacy->blockName;
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);

            float tracerHeight = 0.4375f;
            if (name.find("shulker") != std::string::npos) {
                tracerHeight = 0.5f;
            }

            Vec3<float> target(pos.x + 0.5f, pos.y + tracerHeight, pos.z + 0.5f);
            Vec2<float> screenPos;

            if (MCR::worldToScreen(target, screenPos)) {
                MCR::DrawLine(center, screenPos, 0.2f, WolferColor(vis.fillColor.r, vis.fillColor.g, vis.fillColor.b, 255));
            }
        }
    }
};