#pragma once
#include "../../ModuleBase/Module.h"

class ChunkBorders : public Module {
public:
    bool* showChunkBorderBoolPtr = nullptr;

    ChunkBorders() : Module("ChunkBorders", "Show chunk borders", Category::RENDER) {}

    void onDisable() override {
        if (showChunkBorderBoolPtr != nullptr) {
            *showChunkBorderBoolPtr = false;
        }
    }

    void onClientTick() override {
        if (showChunkBorderBoolPtr != nullptr) {
            *showChunkBorderBoolPtr = true;
        }
    }
};