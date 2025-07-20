#pragma once
#include "../Item/ItemStack.h"

class Container {
public:
	ItemStack* getItem(int slot) {
		return Memory::CallVFunc<7, ItemStack*, int>(this, slot);
	}

	void removeItem(int slot, int count) {
		Memory::CallVFunc<14, void, int, int>(this, slot, count);
	}
	void setItem(int slot, ItemStack const& stack) {
		Memory::CallVFunc<12, void, int, ItemStack const&>(this, slot, stack);
	}
};