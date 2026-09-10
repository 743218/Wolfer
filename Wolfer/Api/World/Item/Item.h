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

	bool isDye() {
		return Memory::CallVFunc<15, bool>(this);
	}

	bool isPattern() {
		return Memory::CallVFunc<39, bool>(this);
	}

	bool canBeDepleted() {
		return Memory::CallVFunc<30, bool>(this);
	}

	bool isDyeable() {
		return Memory::CallVFunc<14, bool>(this);
	}

	bool isDamagable() {
		return Memory::CallVFunc<13, bool>(this);
	}

	bool isCandle() {
		return Memory::CallVFunc<12, bool>(this);
	}

	bool isBlockPlanterItem() {
		return Memory::CallVFunc<10, bool>(this);
	}

	bool isBucket() {
		return Memory::CallVFunc<11, bool>(this);
	}

	bool isHumanoidArmor() {
		return Memory::CallVFunc<9, bool>(this);
	}

	bool isMusicDisc() {
		return Memory::CallVFunc<6, bool>(this);
	}

	bool isActorPlacerItem() {
		return Memory::CallVFunc<60, bool>(this);
	}

	int getEnchantValue() {
		return Memory::CallVFunc<51, int>(this);
	}

	int getEnchantSlot() {
		return Memory::CallVFunc<50, int>(this);
	}

	int getDamageChange(int unbreaking) {
		return Memory::CallVFunc<52, int, int>(this, unbreaking);
	}

	void hurtActor(class ItemStack& item, class Actor& actor, class Mob& attacker) {
		return Memory::CallVFunc<78, void, class ItemStack, class Actor, class Mob>(this, item, actor, attacker);
	}

	void hitActor(class ItemStack& item, class Actor& actor, class Mob& attacker) {
		return Memory::CallVFunc<79, void, class ItemStack, class Actor, class Mob>(this, item, actor, attacker);
	}

	bool isHandEquipped() {
		return Memory::CallVFunc<37, bool>(this);
	}

	bool canDestroyInCreative() {
		return Memory::CallVFunc<43, bool>(this);
	}

	bool isLiquidClipItem() {
		return Memory::CallVFunc<45, bool>(this);
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

	bool isNotStackableOverOne() { //wow arent i lazy
		if (nameContains("potion")) return true;
		if (nameContains("_sword")) return true;
				if (nameContains("_axe")) return true;
				if (nameContains("_pickaxe")) return true;
				if (nameContains("_hoe")) return true;
				if (nameContains("_shovel")) return true;
				if (nameContains("_helmet")) return true;
				if (nameContains("_chestplate")) return true;
				if (nameContains("_leggings")) return true;
				if (nameContains("_boots")) return true;
				if (nameContains("elytra")) return true;
				if (nameContains("trident")) return true;
				if (nameContains("shears")) return true;
				if (nameContains("flint_and_steel")) return true;
				if (nameContains("rod")) return true;
				if (nameContains("carrot_on_a_stick")) return true;
				if (nameContains("compass")) return true;
		return false;
	}

	bool isCrystal() {
		return nameContains("end_crystal");
	}

	bool isTrident() {
		return nameContains("trident");
	}

	bool isHoe() {
		return nameContains("_hoe");
	}

	bool isShovel() {
		return nameContains("_shovel");
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
		if (nameContains("netherite")) return 5;
		if (nameContains("turtle")) return 3;
		return 0;
	}

	int getSwordValueFromName() {
		if (!isSword()) return 0;
		if (nameContains("wood") || nameContains("golden")) return 1;
		if (nameContains("stone")) return 2;
		if (nameContains("iron")) return 3;
		if (nameContains("diamond")) return 4;
		if (nameContains("netherite")) return 5;
		return 0;
	}
};
