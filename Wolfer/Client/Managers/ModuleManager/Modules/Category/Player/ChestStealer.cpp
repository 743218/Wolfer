#include "ChestStealer.h"

ChestStealer::ChestStealer()
	: Module("ChestStealer", "Steals items from containers automatically", Category::PLAYER)
{
	registerSetting(new SliderSetting<float>("SPS", "Items per second", &sps, 1, 1, 20));
	lastStealTime = std::chrono::steady_clock::now();
}

std::string ChestStealer::getModeText() {
	return "Delayed";
}

void ChestStealer::onContainerTick(ContainerScreenController* controller) {
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