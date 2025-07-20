#pragma once
#include "../../ModuleBase/Module.h"

class ChestStealer : public Module {
public:
	ChestStealer()
		: Module("ChestStealer", "Steals items from containers automatically", Category::PLAYER) {
		registerSetting(new SliderSetting<float>("SPS", "Items per second", &sps, 1, 1, 20));
		lastStealTime = std::chrono::steady_clock::now();
	}

	void onContainerTick(ContainerScreenController* controller) override {
		if (!controller)
			return;

		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastStealTime).count();

		int delay = static_cast<int>(1000.f / sps);
		if (elapsed < delay)
			return;

		lastStealTime = now;

		for (int i = itemIndex; i < maxItem; ++i) {
			ItemStack* item = controller->_getItemStack(Containers::Container, i);
			if (item && item->item) {
				controller->shiftItems(Containers::Container, i);
				itemIndex = i + 1;
				return;
			}
		}

		controller->closeContainer();
		itemIndex = 0;
	}

	std::string getModeText() override {
		return "Delayed";
	}

private:
	int itemIndex = 0;
	const int maxItem = 56;
	float sps = 20;
	std::chrono::steady_clock::time_point lastStealTime;
};
