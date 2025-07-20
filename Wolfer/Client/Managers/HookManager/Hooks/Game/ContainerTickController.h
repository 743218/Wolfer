#pragma once

class TextHolder {
public:
	// union that allows for the same memory location to be accessed with different names (gonna copy paste this comment alot)
	union {
		char inlineText[16];  //0x0000
		char* pText;          //0x0000
	};

	// length of the text
	size_t textLength;         //0x0010

	// aligned length of the text
	size_t alignedTextLength;  //0x0018

	// default constructor that sets all member variables to 0
	TextHolder() {
		memset(this, 0, sizeof(TextHolder));
	}

	// copy constructor that copies the values of the specified TextHolder object
	TextHolder(TextHolder const& copy) {
		memset(this, 0, sizeof(TextHolder));
		textLength = copy.textLength;
		alignedTextLength = copy.alignedTextLength;
		if (copy.textLength < 16)
			memcpy(inlineText, copy.inlineText, 16);
		else {
			size_t size = textLength + 1;

			if (size + 1 >= 0x1000)
				size += 8;

			pText = reinterpret_cast<char*>(malloc(size + 1));
			alignedTextLength = size;
			if (size + 1 >= 0x1000) {
				*reinterpret_cast<char**>(pText) = pText;
				pText += 8;
			}

			if (pText != 0x0 && copy.pText != 0x0) {
				memcpy(pText, copy.pText, size);
				pText[size] = 0;
			}
		}
	}

	// assignment operator that copies the values from the specified TextHolder object to the current object
	TextHolder& operator=(TextHolder const& copy) {
		deleteText();
		memset(this, 0, sizeof(TextHolder));
		textLength = copy.textLength;
		alignedTextLength = copy.alignedTextLength;
		if (copy.textLength < 16)
			memcpy(inlineText, copy.inlineText, 16);
		else {
			size_t size = textLength + 1;

			if (size + 1 >= 0x1000)
				size += 8;

			pText = reinterpret_cast<char*>(malloc(size + 1));
			alignedTextLength = size;
			if (size + 1 >= 0x1000) {
				*reinterpret_cast<char**>(pText) = pText;
				pText += 8;
			}

			if (pText != 0x0 && copy.pText != 0x0) {
				memcpy(pText, copy.pText, textLength);
				pText[textLength] = 0;
			}
		}
		return *this;
	}

	// constructor that initializes the TextHolder object with the provided string
	TextHolder(std::string str) {
		memset(this, 0, sizeof(TextHolder));
		textLength = str.size();
		alignedTextLength = textLength | 0xF;
		if (str.size() < 16) {
			memcpy(inlineText, str.c_str(), str.size());
			if (str.size() < 15)
				inlineText[str.size()] = 0;
		}
		else {
			size_t size = str.size();
			if (size + 1 >= 0x1000)
				size += 8;

			pText = reinterpret_cast<char*>(malloc(size + 1));
			alignedTextLength = size;
			if (size + 1 >= 0x1000) {
				*reinterpret_cast<char**>(pText) = pText;
				pText += 8;
			}

			if (pText != 0x0) {
				memcpy(pText, str.c_str(), str.size());
				pText[str.size()] = 0;
			}
		}
	}

	// constructor that allocates memory for the text with the specified size
	TextHolder(size_t allocSize) {
		memset(this, 0, sizeof(TextHolder));
		textLength = 0;
		alignedTextLength = allocSize | 0xF;
		if (alignedTextLength < 16) {
			memset(&inlineText, 0, 16);
		}
		else {
			size_t size = alignedTextLength;
			if (size + 1 >= 0x1000)
				size += 8;

			pText = reinterpret_cast<char*>(malloc(size + 1));
			alignedTextLength = size;
			if (size + 1 >= 0x1000) {
				*reinterpret_cast<char**>(pText) = pText;

				pText += 8;
				memset(pText, 0, alignedTextLength - 8);
			}
			else
				memset(pText, 0, alignedTextLength);
		}
	}

	// constructor that initializes the TextHolder object with the provided data pointer and size
	TextHolder(void* ptr, size_t sizeOfData) {
		memset(this, 0, sizeof(TextHolder));
		textLength = sizeOfData;
		alignedTextLength = sizeOfData;
		if (alignedTextLength < 16)
			memcpy(inlineText, ptr, sizeOfData);
		else
			pText = reinterpret_cast<char*>(ptr);
	}

	// frees memory allocated for pText if necessary and resets the object to its default state
	void deleteText() {
		if (alignedTextLength >= 16 && pText != nullptr) {
			if (alignedTextLength + 1 >= 0x1000) {
				pText = *reinterpret_cast<char**>(reinterpret_cast<__int64>(pText) - 8);
			}
			free(pText);
		}
	}

	// returns a pointer to the text data stored in the object
	~TextHolder() {
		this->deleteText();
	}

	// returns the length of the text data stored in the object
	char* getText() {
		if (alignedTextLength < 16)
			return this->inlineText;
		else
			return this->pText;
	}

	// sets the text data and length for the object
	size_t getTextLength() {
		return textLength;
	}

	// returns the length of the allocated memory for pText
	void setText(std::string str) {
		this->deleteText();
		memset(this, 0, sizeof(TextHolder));
		textLength = str.size();
		alignedTextLength = textLength | 0xF;
		if (str.size() < 16) {
			memcpy(inlineText, str.c_str(), str.size());
			if (str.size() < 15)
				inlineText[str.size()] = 0;
		}
		else {
			size_t size = str.size();
			if (size + 1 >= 0x1000)
				size += 8;

			pText = reinterpret_cast<char*>(malloc(size + 1));
			alignedTextLength = size;
			if (size + 1 >= 0x1000) {
				*reinterpret_cast<char**>(pText) = pText;
				pText += 8;
			}

			if (pText != 0x0) {
				memcpy(pText, str.c_str(), str.size());
				pText[str.size()] = 0;
			}
		}
	}

	// returns a pointer to the allocated memory for pText
	void resetWithoutDelete() {
		memset(this, 0, sizeof(TextHolder));
	}
};

class ItemStack;

namespace Containers
{
	inline const std::string Inventory = "inventory_items";
	inline const std::string Hotbar = "hotbar_items";
	inline const std::string Container = "container_items";
	inline const std::string Armor = "armor_items";
}

class ContainerScreenController {
private:
	virtual __int64 Destructor();

private:
	virtual __int64 tick(void);
	virtual __int64 handleEvent(__int64&);
	virtual __int64 getRoute(void);
	virtual __int64 setScreenState(__int64, const std::string&);
	virtual __int64 onOpen(void);
	virtual __int64 onTerminate(void);
	virtual __int64 onInit(void);
	virtual __int64 onDelete(void);
	virtual __int64 unknown1();

public:
	virtual bool canExit(void);
	virtual __int64 tryExit(void);

private:
	virtual __int64 areControllerTabsEnabled(void);
	virtual __int64 onCreation(void);
	virtual __int64 logCreationTime(const std::string&, double, double, unsigned char);
	virtual __int64 unknown2();
	virtual __int64 unknown3();
	virtual __int64 unknown4();
	virtual __int64 unknown5();
	virtual __int64 unknown6();

public:
	virtual __int64 onLeave(void);
	virtual __int64 leaveScreen(void);

private:
	virtual __int64 handleGameEventNotification(__int64);
	virtual __int64 bind(const std::string&, unsigned int, int, const std::string&, unsigned int, const std::string&, __int64&);
	virtual __int64 bind(const std::string&, unsigned int, const std::string&, __int64&);
	virtual __int64 handleLicenseChanged(void);
	virtual __int64 onDictationEvent(const std::string&);

public:
	virtual void setAssociatedBlockPos(Vector3<int> const&);
	virtual void setAssociatedEntityUniqueID(__int64);
	virtual void setSuspendInput(bool);

private:
	virtual __int64 getCallbackInterval(void);
	virtual __int64 onRender(void);
	virtual __int64 addStaticScreenVars(__int64&);
	virtual __int64 getAdditionalScreenInfo(void);
	virtual __int64 getTelemetryOverride(void);
	virtual __int64 addEventProperties(__int64) const;
	virtual __int64 getSceneType(void);
	virtual __int64 getScreenVersion(void);
	virtual __int64 screenHandlesGamepadMenuButton(void);
	virtual __int64 getProxy(void);
	virtual __int64 onEntered(void);
	virtual __int64 getNameId(const std::string&);
	virtual __int64 _doesScreenHaveExitBehavior(void);
	virtual __int64 _isStillValid(void);
	virtual __int64 _getGamepadHelperVisible(void);
	virtual __int64 _getMixedHelperVisible(void);
	virtual __int64 _getKeyboardHelperVisible(void);
	virtual __int64 _getButtonADescription(void);
	virtual __int64 _getButtonBDescription(void);
	virtual __int64 _getButtonXDescription(void);
	virtual __int64 _getButtonYDescription(void);
	virtual __int64 _getButtonKeyboardDescription(void);

public:
	virtual __int64 _handlePlaceAll(const std::string&, int);
	virtual __int64 _handlePlaceOne(const std::string&, int);
	virtual __int64 _handleSelectSlot(const std::string&, int);
	virtual __int64 _getSelectedSlotInfo(void);
	virtual __int64 unknown7();
	virtual __int64 unknown8();
	virtual __int64 unknown9();
	virtual __int64 unknown10();
	virtual __int64 unknown11();
	virtual __int64 unknown12();
	virtual ItemStack* _getItemStack(std::string, int) const;

private:
	virtual __int64 _getVisualItemStack(const std::string&, int);
	virtual __int64 _getTakeableItemStackBase(const std::string&, int);
	virtual __int64 _onContainerSlotHovered(const std::string&, int);
	virtual __int64 _onContainerSlotSelected(const std::string&, int);
	virtual __int64 _onContainerSlotPressed(const std::string&, int);
	virtual __int64 _shouldSwap(const std::string&, int, const std::string&, int);
	virtual __int64 _getCollectionName(__int64*);
	virtual __int64 _canSplit(const std::string&, int);
	virtual __int64 _sendFlyingItem(const __int64&, const std::string&, int, const std::string&, int);

public:
	// handleAutoPlace
	void shiftItems(const std::string& containerName, int slots) {
		static void* shiftClick = reinterpret_cast<void*>(Memory::findSig("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 8B E1 49 8B F0 44 8B FA 89 54 24 40 48 8B F9 48 8B 01 48 8B 80 ? ? ? ? FF 15 ? ? ? ? 84 C0 0F 84 ? ? ? ? 48 8B 8F ? ? ? ? 48 85 C9 74 6B E8 ? ? ? ? 84 C0 74 62 48 8D 8F ? ? ? ? 8B 91 ? ? ? ? 39 91 ? ? ? ? 74 13 44 89 64 24 ? 4C 8B CE"));

		Memory::CallFunc<void, ContainerScreenController*, uintptr_t, TextHolder, int>(
			shiftClick, this, 0x7FFFFFFF, containerName, slots);
	}

	// handleSwap
	void handleSwap(const std::string& name1, int slot1, const std::string& name2, int slot2) {
		this->shiftItems(name1, slot1);
		this->shiftItems(name2, slot2);
	}

	void closeContainer() {
		this->leaveScreen();
		this->canExit();
		this->tryExit();
	}

	void moveItem(const std::string& fromContainer, int fromSlot, const std::string& toContainer, int toSlot) {
		this->_onContainerSlotSelected(fromContainer, fromSlot);
		this->_onContainerSlotPressed(toContainer, toSlot);
	}

	void swapSlots(const std::string& container1, int slot1, const std::string& container2, int slot2) {
		this->handleSwap(container1, slot1, container2, slot2);
	}

	void sortHotbar() {
		const int hotbarSize = 9;
		std::vector<std::pair<int, ItemStack*>> items;
		for (int i = 0; i < hotbarSize; ++i) {
			items.emplace_back(i, this->_getItemStack(Containers::Hotbar, i));
		}
		std::sort(items.begin(), items.end(), [](const auto& a, const auto& b) {
			if (!a.second) return false;
			if (!b.second) return true;
			return a.second->item->itemId < b.second->item->itemId;
			});
		for (int i = 0; i < hotbarSize; ++i) {
			int currentSlot = items[i].first;
			if (currentSlot != i) {
				this->moveItem(Containers::Hotbar, currentSlot, Containers::Hotbar, i);
			}
		}
	}

};

class ChestScreenController : public ContainerScreenController {
};