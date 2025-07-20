#pragma once
#include "../../../Api/Utils/Memory.h"

class ItemStackBase;
class ItemStack;
class Block;

class Item {
public:
	CLASS_MEMBER(std::string, texture_name, 0x60);
	CLASS_MEMBER(uint16_t, itemId, 0xA2);

private:
	bool nameContains(const char* str) {
		return texture_name.find(str) != std::string::npos;
	}

public:

	short getDamageValue(__int64 userData) {
		using func_t = short(__thiscall*)(Item*, __int64);
		static func_t Func = reinterpret_cast<func_t>(Addresses::Item_getDamageValue);
		return Func(this, userData);
	}

	bool isFood() {
		return Memory::CallVFunc<18, bool>(this);
	}

	bool isThrowable() {
		return Memory::CallVFunc<19, bool>(this);
	}

	bool isUseable() {
		return Memory::CallVFunc<20, bool>(this);
	}

	short getMaxDamage() {
		return Memory::CallVFunc<34, short>(this);
	}

	int getAttackDamage() {
		return Memory::CallVFunc<35, int>(this);
	}

	short setMaxUseDuration(int number) {
		return Memory::CallVFunc<25, short, int>(this, number);
	}

	bool isGlint(ItemStackBase* itemStackBase) {
		return Memory::CallVFunc<38, bool, ItemStackBase*>(this, itemStackBase);
	}

	int getArmorValue() {
		return Memory::CallVFunc<52, int>(this);
	}

	float getDestroySpeed(ItemStackBase* item, Block* block) {
		return Memory::CallVFunc<77, float, ItemStackBase*, Block*>(this, item, block);
	}

	bool isAxe() {
		return nameContains("_axe");
	}

	bool isBoomBox() {
		return nameContains("boombox");
	}

	bool isPickaxe() {
		return nameContains("_pickaxe");
	}

	bool isHelmet() {
		return nameContains("_helmet");
	}

	bool isChestplate() {
		return nameContains("_chestplate");
	}

	bool isLeggings() {
		return nameContains("_leggings");
	}

	bool isBoots() {
		return nameContains("_boots");
	}

	bool isSword() {
		return nameContains("_sword");
	}

	int getArmorValueFromName() {
		if (!isHelmet() && !isChestplate() && !isLeggings() && !isBoots()) return 0;
		if (nameContains("leather") || nameContains("golden")) return 1;
		if (nameContains("chainmail")) return 2;
		if (nameContains("iron")) return 3;
		if (nameContains("diamond")) return 4;
		return 0;
	}

	int getSwordValueFromName() {
		if (!isSword()) return 0;
		if (nameContains("wood") || nameContains("golden")) return 1;
		if (nameContains("stone")) return 2;
		if (nameContains("iron")) return 3;
		if (nameContains("diamond")) return 4;
		return 0;
	}
};
