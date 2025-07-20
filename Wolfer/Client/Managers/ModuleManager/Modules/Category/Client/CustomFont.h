#pragma once

#include "../../ModuleBase/Module.h"
#include <string>
#include <vector>

class CustomFont : public Module {
public:
	EnumSetting* fontEnumSetting = nullptr;
	int fontMode = 1;
	int fontSize = 25;
	bool italic = false;
	bool shadow = false;

	CustomFont()
		: Module("Fonts", "Font of Client", Category::CLIENT) {
		fontEnumSetting = static_cast<EnumSetting*>(
			registerSetting(new EnumSetting("Font", "NULL", { "Arial", "Verdana", "Tahoma" }, &fontMode, 0))
			);
		registerSetting(new SliderSetting<int>("FontSize", "NULL", &fontSize, 25, 20, 30));
		registerSetting(new BoolSetting("Italic", "NULL", &italic, false));
		registerSetting(new BoolSetting("Shadow", "NULL", &shadow, false));
	}

	bool isEnabled() override {
		return true;
	}

	bool isVisible() override {
		return false;
	}

	std::string getSelectedFont() {
		return fontEnumSetting->enumList[fontMode];
	}
};
