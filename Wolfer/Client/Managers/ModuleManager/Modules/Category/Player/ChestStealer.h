#pragma once
#include "../../ModuleBase/Module.h"

class ChestStealer : public Module {
public:
	ChestStealer();
	void onContainerTick(ContainerScreenController* controller) override;
	std::string getModeText() override;

private:
	int itemIndex = 0;
	const int maxItem = 56;
	float sps = 20;

	std::chrono::steady_clock::time_point lastStealTime;
};