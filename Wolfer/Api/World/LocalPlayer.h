#pragma once
#include "Player.h"

class LocalPlayer : public Player {
public:
	void displayClientMessage(const std::string& message) {
		Memory::CallVFunc<213, void, const std::string&>(this, message);
	}
};
