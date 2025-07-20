#pragma once
#include "../../FuncHook.h"

class GetBlockHook : public FuncHook {
private:
	using func_t = Block*(__thiscall*)(BlockSource*, const BlockPos&);
	static inline func_t oFunc;

	static Block* BlockSourceGetBlockCallback(BlockSource* _this, const BlockPos& blockPos) {
		Block* result = oFunc(_this, blockPos);
		StorageESP* storageESP = ModuleManager::getModule<StorageESP>();
		if (storageESP && storageESP->isEnabled() && result && result->blockLegacy) {
			std::string blockName = result->blockLegacy->blockName;
			std::transform(blockName.begin(), blockName.end(), blockName.begin(), ::tolower);

			if (blockName.find("chest") != std::string::npos && blockName.find("ender") == std::string::npos) {
				storageESP->blockList[blockPos] = { storageESP->chestFillColor, storageESP->chestLineColor };
			}
			else if (blockName.find("shulker") != std::string::npos) {
				storageESP->blockList[blockPos] = { storageESP->shulkerFillColor, storageESP->shulkerLineColor };
			}
			else if (storageESP->showHopper && (result->blockLegacy->blockId == 154 || blockName.find("hopper") != std::string::npos)) {
				storageESP->blockList[blockPos] = { storageESP->hopperFillColor, storageESP->hopperLineColor };
			}
			else if (storageESP->showDropper && blockName.find("dropper") != std::string::npos) {
				storageESP->blockList[blockPos] = { storageESP->dropperFillColor, storageESP->dropperLineColor };
			}
			else if (storageESP->showDispenser && (result->blockLegacy->blockId == 23 || blockName.find("dispenser") != std::string::npos)) {
				storageESP->blockList[blockPos] = { storageESP->dispenserFillColor, storageESP->dispenserLineColor };
			}
		}
		return result;
	}
public:
	GetBlockHook() {
		OriginFunc = (void*)&oFunc;
		func = (void*)&BlockSourceGetBlockCallback;
	}
};