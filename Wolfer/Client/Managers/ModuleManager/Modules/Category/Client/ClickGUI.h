#pragma once
#include "../../ModuleBase/Module.h"
#include "../../../ModuleManager.h"

class ClickGUI : public Module {
private:
	struct ClickWindow {
		std::string name;
		Category category;
		Vector2<float> pos;
		bool extended = true;

		// Animation
		float selectedAnim = 0.f;

		std::vector<Module*> moduleList;

		ClickWindow(std::string windowName, Vector2<float> startPos, Category c);
	};

	std::vector<ClickWindow*> windowList;
	float openAnim = 0.0f;
	bool initialized = false;

	Vector2<float> mousePos = Vector2<float>(0.f, 0.f);
	Vector2<float> mouseDelta = Vector2<float>(0.f, 0.f);

	bool isLeftClickDown = false;
	bool isHoldingLeftClick = false;
	bool isRightClickDown = false;
	bool isHoldingRightClick = false;

	ClickWindow* draggingWindowPtr = nullptr;

	KeybindSetting* capturingKbSettingPtr = nullptr;
	SliderSettingBase* draggingSliderSettingPtr = nullptr;

	void updateSelectedAnimRect(Vector4<float>& rect, float& anim) {
		bool shouldUp = rect.contains(mousePos);

		if (draggingWindowPtr != nullptr)
			shouldUp = false;

		if (draggingSliderSettingPtr != nullptr) {
			if (&draggingSliderSettingPtr->selectedAnim != &anim)
				shouldUp = false;
			else
				shouldUp = true;
		}

		if (shouldUp)
			anim = 1.f;
		else
			anim -= D2D::deltaTime * 2.f;

		if (anim > 1.f)
			anim = 1.f;
		if (anim < 0.f)
			anim = 0.f;
	}

private:
	WolferColor mainColor = WolferColor(175, 0, 0, 175);
	float blurStrength = 5.f;
	bool showDescription = true;
public:
	ClickGUI() : Module("Gui", "Display all modules", Category::CLIENT, VK_INSERT) {
		registerSetting(new ColorSetting("Color", "NULL", &mainColor, mainColor));
		registerSetting(new SliderSetting<float>("Blur", "Background blur intensity", &blurStrength, 12.f, 0.f, 20.f));
		registerSetting(new BoolSetting("Description", "Show Description", &showDescription, true));
	}

	~ClickGUI();

	void onDisable() override {
		if (g_Data.getLocalPlayer()) {
			g_Data.clientInstance->grabMouse();
		}

		isLeftClickDown = false;
		isRightClickDown = false;
		isHoldingLeftClick = false;
		isHoldingRightClick = false;

		draggingWindowPtr = nullptr;

		capturingKbSettingPtr = nullptr;
		draggingSliderSettingPtr = nullptr;

		openAnim = 0.0f;
	}

	void onEnable() override {
		g_Data.clientInstance->releasebMouse();
		openAnim = 0.0f;
	}

	bool isVisible() override {
		return false;
	}

	void onKeyUpdate(int key, bool isDown) override {
		if (!isEnabled()) {
			if (key == getKeybind() && isDown) {
				setEnabled(true);
			}
		}
		else {
			if (isDown) {
				if (key < 192) {
					if (capturingKbSettingPtr != nullptr) {
						if (key != VK_ESCAPE)
							*capturingKbSettingPtr->value = key;
						capturingKbSettingPtr = nullptr;
						return;
					}
				}
				if (key == getKeybind() || key == VK_ESCAPE) {
					setEnabled(false);
				}
			}
		}
	}


	void onMouseUpdate(Vector2<float> mousePosA, char mouseButton, char isDown) {

		//MouseButtons
		//0 = mouse move
		//1 = left click
		//2 = right click
		//3 = middle click
		//4 = scroll   (isDown: 120 (SCROLL UP) and -120 (SCROLL DOWN))

		switch (mouseButton) {
		case 0:
			mousePos = mousePosA;
			break;
		case 1:
			isLeftClickDown = isDown;
			isHoldingLeftClick = isDown;
			break;
		case 2:
			isRightClickDown = isDown;
			isHoldingRightClick = isDown;
			break;
		case 4:
			float moveVec = (isDown < 0) ? -15.f : 15.f;
			for (auto& window : windowList) {
				if (window == draggingWindowPtr)
					continue;

				window->pos.y += moveVec;
			}
			break;
		}

		if (draggingWindowPtr != nullptr) {
			if (!isHoldingLeftClick)
				draggingWindowPtr = nullptr;
		}

		if (capturingKbSettingPtr != nullptr) {
			if (isRightClickDown) {
				*capturingKbSettingPtr->value = 0;
				capturingKbSettingPtr = nullptr;
				isRightClickDown = false;
			}
		}

		if (draggingSliderSettingPtr != nullptr) {
			if (!isHoldingLeftClick)
				draggingSliderSettingPtr = nullptr;
		}
	}

	void InitClickGUI() {
		setEnabled(false);

		Vector2<float> startPos = Vector2<float>(175.f, 75.f);
		windowList.push_back(new ClickWindow("Movement", startPos, Category::MOVEMENT));
		startPos.x += 210.f;
		windowList.push_back(new ClickWindow("Render", startPos, Category::RENDER));
		startPos.x += 210.f;
		windowList.push_back(new ClickWindow("Player", startPos, Category::PLAYER));
		startPos.x += 210.f;
		windowList.push_back(new ClickWindow("Misc", startPos, Category::MISC));
		startPos.x += 210.f;
		windowList.push_back(new ClickWindow("Client", startPos, Category::CLIENT));
		startPos.x += 210.f;
		windowList.push_back(new ClickWindow("Exploit", startPos, Category::EXPLOIT));

		initialized = true;
	}

	struct NetworkNode {
		Vector2<float> position;
		Vector2<float> velocity;
	};

	void Render() {
		if (!initialized)
			return;

		if (g_Data.canUseMoveKeys())
			g_Data.clientInstance->releasebMouse();

		static Vector2<float> oldMousePos = mousePos;
		mouseDelta = mousePos.sub(oldMousePos);
		oldMousePos = mousePos;

		Vector2<float> screenSize = g_Data.clientInstance->guiData->windowSizeReal;	//D2D::getWindowSize();
		float deltaTime = D2D::deltaTime;

		float textSize = 0.82f;
		float textPaddingX = 3.f * textSize;
		float textPaddingY = 1.f * textSize;
		float textHeight = D2D::getTextHeight("", textSize) + 2.f;

		std::string descriptionText = "NULL";

		openAnim += deltaTime * 2.f;
		if (openAnim > 1.f)
			openAnim = 1.f;

		if (blurStrength > 0.1f)
			D2D::addBlur(Vector4<float>(0.f, 0.f, screenSize.x, screenSize.y), blurStrength * openAnim);
		D2D::fillRectangle(Vector4<float>(0.f, 0.f, screenSize.x, screenSize.y), WolferColor(0, 0, 0, (int)(110 * openAnim)));

		static std::vector<NetworkNode> nodeNetwork;
		static std::vector<Vector2<float>> nodeStart;
		static std::vector<Vector2<float>> nodeEnd;
		static std::vector<float> nodeTimers;
		static std::vector<float> nodeDurations;
		static bool nodeNetworkInitialized = false;

		const int numNodes = 210;
		const float nodeRadius = 2.9f;
		const float linkDistance = 130.f;
		const float rainbowSeconds = 13.f;
		const float saturation = 0.9f;
		const float brightness = 1.f;

		if (!nodeNetworkInitialized) {
			nodeNetwork.clear();
			nodeStart.clear();
			nodeEnd.clear();
			nodeTimers.clear();
			nodeDurations.clear();
			for (int i = 0; i < numNodes; i++) {
				Vector2<float> start(rand() % (int)screenSize.x, rand() % (int)screenSize.y);
				Vector2<float> end(rand() % (int)screenSize.x, rand() % (int)screenSize.y);
				float duration = 12.f + ((rand() % 1000) / 1000.f) * 8.f;
				nodeNetwork.push_back({ start, Vector2<float>(0.f, 0.f) });
				nodeStart.push_back(start);
				nodeEnd.push_back(end);
				nodeTimers.push_back(0.f);
				nodeDurations.push_back(duration);
			}
			nodeNetworkInitialized = true;
		}

		for (int i = 0; i < numNodes; i++) {
			nodeTimers[i] += deltaTime;
			float duration = nodeDurations[i];
			float t = nodeTimers[i] / duration;

			if (t >= 1.f) {
				nodeStart[i] = nodeNetwork[i].position;
				nodeEnd[i] = Vector2<float>(rand() % (int)screenSize.x, rand() % (int)screenSize.y);
				nodeTimers[i] = 0.f;
				nodeDurations[i] = 12.f + ((rand() % 1000) / 1000.f) * 8.f;
				t = 0.f;
			}

			Vector2<float> a = nodeStart[i];
			Vector2<float> b = nodeEnd[i];
			Vector2<float> pos = Vector2<float>(
				a.x + (b.x - a.x) * t,
				a.y + (b.y - a.y) * t + sinf(t * 3.1415f) * 6.f
			);
			nodeNetwork[i].position = pos;
		}

		for (int i = 0; i < numNodes; i++) {
			Vector2<float> posA = nodeNetwork[i].position;
			float hueA = posA.x / screenSize.x;
			if (hueA < 0.f) hueA = 0.f;
			if (hueA > 1.f) hueA = 1.f;
			WolferColor nodeColor = Colors::getRainbowColor(rainbowSeconds, saturation, brightness, hueA * rainbowSeconds);
			D2D::fillCircle(posA, nodeColor, nodeRadius);

			for (int j = i + 1; j < numNodes; j++) {
				Vector2<float> posB = nodeNetwork[j].position;
				float dx = posA.x - posB.x;
				float dy = posA.y - posB.y;
				float dist = sqrtf(dx * dx + dy * dy);
				if (dist < linkDistance) {
					float alpha = 1.f - (dist / linkDistance);
					float midX = (posA.x + posB.x) * 0.5f;
					float midHue = midX / screenSize.x;
					if (midHue < 0.f) midHue = 0.f;
					if (midHue > 1.f) midHue = 1.f;
					WolferColor linkColor = Colors::getRainbowColor(rainbowSeconds, saturation, brightness, midHue * rainbowSeconds);
					linkColor.a = static_cast<int>(alpha * 100.f);
					D2D::drawLine(posA, posB, linkColor, 1.f);
				}
			}
		}

		for (auto& window : windowList) {
			if (window == draggingWindowPtr) {
				window->pos = window->pos.add(mouseDelta);
			}

			static CustomFont* customFontMod = ModuleManager::getModule<CustomFont>();
			float fontPercent = (float)customFontMod->fontSize / 25.f;

			Vector4<float> headerRectPos = Vector4<float>(window->pos.x,
				window->pos.y,
				window->pos.x + (int)(202.f) + (textPaddingX * 2.f),
				window->pos.y + textHeight + (textPaddingY * 2.f));

			Vector2<float> headerTextPos = Vector2<float>(
				headerRectPos.x + (headerRectPos.z - headerRectPos.x - D2D::getTextWidth(window->name, textSize)) / 2.f,
				headerRectPos.y + (headerRectPos.w - headerRectPos.y - D2D::getTextHeight(window->name, textSize)) / 2.f
			);

			if (headerRectPos.contains(mousePos)) {
				if (isLeftClickDown) {
					draggingWindowPtr = window;
					isLeftClickDown = false;
				}
				else if (isRightClickDown) {
					window->extended = !window->extended;
					isRightClickDown = false;
				}
			}

			updateSelectedAnimRect(headerRectPos, window->selectedAnim);

			D2D::fillRectangle(headerRectPos, mainColor);
			D2D::drawText(headerTextPos, window->name, WolferColor(255, 255, 255), textSize);

			D2D::fillRectangle(headerRectPos, WolferColor(255, 255, 255, (int)(45 * window->selectedAnim)));

			if (window->extended) {
				float moduleSpace = 2.f * textSize * fontPercent;
				float settingSpace = 2.f * textSize * fontPercent;
				float yHeight = 0.f;
				yHeight += moduleSpace;

				for (auto& mod : window->moduleList) {
					yHeight += textHeight + (textPaddingY * 2.f);

					if (mod->extended) {
						yHeight += settingSpace;
						for (auto& setting : mod->getSettingList()) {
							if (setting->type != SettingType::COLOR_S) {
								yHeight += textHeight + (textPaddingY * 2.f);
							}
							else {
								ColorSetting* colorSetting = static_cast<ColorSetting*>(setting);
								yHeight += textHeight + (textPaddingY * 2.f);
								if (colorSetting->extended) {
									yHeight += settingSpace;
									for (auto& slider : colorSetting->colorSliders) {
										yHeight += textHeight + (textPaddingY * 2.f) + settingSpace;
									}
									yHeight -= settingSpace;
								}
							}
							yHeight += settingSpace;
						}
					}

					yHeight += moduleSpace;
				}

				float wbgPaddingX = 2.f * textSize * fontPercent;
				Vector4<float> wbgRectPos = Vector4<float>(headerRectPos.x + wbgPaddingX, headerRectPos.w, headerRectPos.z - wbgPaddingX, headerRectPos.w + yHeight);
				D2D::fillRectangle(wbgRectPos, WolferColor(0, 0, 0, 145));

				float yOffset = headerRectPos.w + moduleSpace;
				for (auto& mod : window->moduleList) {
					float modPaddingX = wbgPaddingX + (2.f * textSize * fontPercent);
					Vector4<float> mRectPos = Vector4<float>(headerRectPos.x + modPaddingX,
						yOffset,
						headerRectPos.z - modPaddingX,
						yOffset + textHeight + (textPaddingY * 2.f));

					Vector2<float> mTextPos = Vector2<float>(
						mRectPos.x + (mRectPos.z - mRectPos.x - D2D::getTextWidth(mod->getModuleName(), textSize)) / 2.f,
						mRectPos.y + (mRectPos.w - mRectPos.y - D2D::getTextHeight(mod->getModuleName(), textSize)) / 2.f
					);

					if (mRectPos.contains(mousePos)) {
						descriptionText = mod->getDescription();
						if (isLeftClickDown) {
							mod->toggle();
							isLeftClickDown = false;
						}
						else if (isRightClickDown) {
							mod->extended = !mod->extended;
							isRightClickDown = false;
						}
					}

					updateSelectedAnimRect(mRectPos, mod->selectedAnim);

					D2D::fillRectangle(mRectPos, mod->isEnabled() ? mainColor : WolferColor(32, 32, 32, 200));
					D2D::drawText(mTextPos, mod->getModuleName(), mod->isEnabled() ? WolferColor(255, 255, 255) : Colors::lerp(WolferColor(175, 175, 175), WolferColor(255, 255, 255), mod->selectedAnim), textSize);
					D2D::fillRectangle(mRectPos, WolferColor(255, 255, 255, (int)(45 * mod->selectedAnim)));


					yOffset += textHeight + (textPaddingY * 2.f);

					if (mod->extended) {
						yOffset += settingSpace;
						float startY = yOffset;

						float settingPaddingX = 7.5f * textSize * fontPercent;
						float settingPaddingZ = 3.5f * textSize * fontPercent;

						for (auto& setting : mod->getSettingList()) {
							std::string settingName = setting->name;

							Vector4<float> sRectPos = Vector4<float>(mRectPos.x + settingPaddingX,
								yOffset,
								mRectPos.z - settingPaddingZ,
								yOffset + textHeight + (textPaddingY * 2.f));

							Vector2<float> sTextPos = Vector2<float>(
								sRectPos.x + textPaddingX,
								sRectPos.y + (sRectPos.w - sRectPos.y - D2D::getTextHeight(settingName, textSize)) / 2.f
							);

							if (sRectPos.contains(mousePos)) {
								descriptionText = setting->description;
							}

							updateSelectedAnimRect(sRectPos, setting->selectedAnim);

							switch (setting->type) {
							case SettingType::BOOL_S: {
								BoolSetting* boolSetting = static_cast<BoolSetting*>(setting);
								bool& boolValue = (*boolSetting->value);

								if (sRectPos.contains(mousePos)) {
									if (isLeftClickDown) {
										boolValue = !boolValue;
										isLeftClickDown = false;
									}
								}

								if (boolValue)
									D2D::fillRectangle(sRectPos, mainColor);

								D2D::drawText(sTextPos, settingName, WolferColor(255, 255, 255), textSize);

								yOffset += textHeight + (textPaddingY * 2.f);
								break;
							}
							case SettingType::KEYBIND_S: {
								KeybindSetting* keybindSetting = static_cast<KeybindSetting*>(setting);
								int& keyValue = (*keybindSetting->value);

								if (sRectPos.contains(mousePos)) {
									if (isLeftClickDown) {
										if (capturingKbSettingPtr == keybindSetting)
											capturingKbSettingPtr = nullptr;
										else
											capturingKbSettingPtr = keybindSetting;

										isLeftClickDown = false;
									}
								}

								std::string keybindName;
								if (setting == capturingKbSettingPtr) {
									keybindName = "...";
								}
								else {
									if (keyValue != 0)
										keybindName = KeyNames[keyValue];
									else
										keybindName = "None";
								}

								Vector2<float> keybindTextPos = Vector2<float>(sRectPos.z - textPaddingX - D2D::getTextWidth(keybindName, textSize), sTextPos.y);

								D2D::drawText(sTextPos, settingName + ":", WolferColor(255, 255, 255), textSize);
								D2D::drawText(keybindTextPos, keybindName, WolferColor(255, 255, 255), textSize);

								yOffset += textHeight + (textPaddingY * 2.f);
								break;
							}
							case SettingType::ENUM_S: {
								EnumSetting* enumSetting = static_cast<EnumSetting*>(setting);
								int& enumValue = (*enumSetting->value);

								if (sRectPos.contains(mousePos)) {
									if (isLeftClickDown) {
										enumValue++;
										if (enumValue > enumSetting->enumList.size() - 1)
											enumValue = 0;
										isLeftClickDown = false;
									}
									else if (isRightClickDown) {
										enumValue--;
										if (enumValue < 0)
											enumValue = (int)enumSetting->enumList.size() - 1;
										isRightClickDown = false;
									}
								}

								std::string modeName = enumSetting->enumList[enumValue];
								Vector2<float> modeTextPos = Vector2<float>(sRectPos.z - textPaddingX - D2D::getTextWidth(modeName, textSize), sTextPos.y);

								D2D::drawText(sTextPos, settingName + ":", WolferColor(255, 255, 255), textSize);
								D2D::drawText(modeTextPos, modeName, WolferColor(255, 255, 255), textSize);

								yOffset += textHeight + (textPaddingY * 2.f);
								break;
							}
							case SettingType::COLOR_S: {
								ColorSetting* colorSetting = static_cast<ColorSetting*>(setting);

								if (sRectPos.contains(mousePos)) {
									if (isRightClickDown) {
										colorSetting->extended = !colorSetting->extended;
										isRightClickDown = false;
									}
								}

								float colorBoxSize = std::round(textHeight / 1.5f);
								float colorBoxPaddingX = textPaddingX + (2.f * textSize);
								Vector4<float> colorBoxRect = Vector4<float>(sRectPos.z - colorBoxPaddingX - colorBoxSize,
									(sRectPos.y + sRectPos.w - colorBoxSize) / 2.f,
									sRectPos.z - colorBoxPaddingX,
									(sRectPos.y + sRectPos.w + colorBoxSize) / 2.f);

								D2D::drawText(sTextPos, settingName + ":", WolferColor(255, 255, 255), textSize);
								D2D::fillRectangle(colorBoxRect, (*colorSetting->colorPtr));

								yOffset += textHeight + (textPaddingY * 2.f);

								if (colorSetting->extended) {
									yOffset += settingSpace;

									float colorStartX = sRectPos.x;
									float colorStartY = yOffset;

									for (auto& slider : colorSetting->colorSliders) {

										Vector4<float> colorSliderRect = Vector4<float>(sRectPos.x + settingPaddingX,
											yOffset,
											sRectPos.z - settingPaddingZ,
											yOffset + textHeight + (textPaddingY * 2.f));

										Vector2<float> colorSliderTextPos = Vector2<float>(colorSliderRect.x + textPaddingX, colorSliderRect.y + textPaddingY);

										updateSelectedAnimRect(colorSliderRect, slider->selectedAnim);

										if (colorSliderRect.contains(mousePos)) {
											if (isLeftClickDown) {
												draggingSliderSettingPtr = slider;
												isLeftClickDown = false;
											}
										}

										uint8_t& value = (*slider->valuePtr);
										float minValue = (float)slider->minValue;
										float maxValue = (float)slider->maxValue;

										if (draggingSliderSettingPtr == slider) {
											float draggingPercent = (mousePos.x - colorSliderRect.x) / (colorSliderRect.z - colorSliderRect.x);

											if (draggingPercent > 1.f)
												draggingPercent = 1.f;
											if (draggingPercent < 0.f)
												draggingPercent = 0.f;

											value = (int)minValue + (int)std::round((maxValue - minValue) * draggingPercent);
										}

										float valuePercent = (value - minValue) / (maxValue - minValue);

										if (valuePercent > 1.f)
											valuePercent = 1.f;
										if (valuePercent < 0.f)
											valuePercent = 0.f;

										Vector4<float> valueRectPos = Vector4<float>(colorSliderRect.x, colorSliderRect.y, colorSliderRect.x + (colorSliderRect.z - colorSliderRect.x) * valuePercent, colorSliderRect.w);

										char valueText[10];
										sprintf_s(valueText, 10, "%i", (int)value);
										std::string valueTextStr(valueText);

										Vector2<float> valueTextPos = Vector2<float>(colorSliderRect.z - textPaddingX - D2D::getTextWidth(valueTextStr, textSize, (draggingSliderSettingPtr != slider)), colorSliderTextPos.y);

										D2D::fillRectangle(valueRectPos, mainColor);
										D2D::drawText(colorSliderTextPos, slider->name + ":", WolferColor(255, 255, 255), textSize);
										D2D::drawText(valueTextPos, valueTextStr, WolferColor(255, 255, 255), textSize, (draggingSliderSettingPtr != slider));

										D2D::fillRectangle(colorSliderRect, WolferColor(255, 255, 255, (int)(45 * slider->selectedAnim)));

										yOffset += textHeight + (textPaddingY * 2.f) + settingSpace;
									}

									yOffset -= settingSpace;

									float colorEndY = yOffset;
									float colorSLineWidth = 4.f * textSize * fontPercent;
									float colorSLinePaddingX = 1.f * textSize;
									Vector4<float> colorSLineRect = Vector4<float>(colorStartX + colorSLinePaddingX,
										colorStartY,
										colorStartX + colorSLinePaddingX + colorSLineWidth,
										colorEndY);

									D2D::fillRectangle(colorSLineRect, mainColor);
								}
								break;
							}
							case SettingType::SLIDER_S: {
								SliderSettingBase* sliderSettingBase = static_cast<SliderSettingBase*>(setting);

								if (sRectPos.contains(mousePos)) {
									if (isLeftClickDown) {
										draggingSliderSettingPtr = sliderSettingBase;
										isLeftClickDown = false;
									}
								}

								if (sliderSettingBase->valueType == ValueType::INT_T) {
									SliderSetting<int>* intSlider = static_cast<SliderSetting<int>*>(sliderSettingBase);
									int& value = (*intSlider->valuePtr);
									float minValue = (float)intSlider->minValue;
									float maxValue = (float)intSlider->maxValue;

									if (draggingSliderSettingPtr == sliderSettingBase) {
										float draggingPercent = (mousePos.x - sRectPos.x) / (sRectPos.z - sRectPos.x);

										if (draggingPercent > 1.f)
											draggingPercent = 1.f;
										if (draggingPercent < 0.f)
											draggingPercent = 0.f;

										value = (int)minValue + (int)std::round((maxValue - minValue) * draggingPercent);
									}

									float valuePercent = (value - minValue) / (maxValue - minValue);

									if (valuePercent > 1.f)
										valuePercent = 1.f;
									if (valuePercent < 0.f)
										valuePercent = 0.f;

									Vector4<float> valueRectPos = Vector4<float>(sRectPos.x, sRectPos.y, sRectPos.x + (sRectPos.z - sRectPos.x) * valuePercent, sRectPos.w);

									char valueText[10];
									sprintf_s(valueText, 10, "%i", value);
									std::string valueTextStr(valueText);

									Vector2<float> valueTextPos = Vector2<float>(sRectPos.z - textPaddingX - D2D::getTextWidth(valueTextStr, textSize, (draggingSliderSettingPtr != sliderSettingBase)), sTextPos.y);

									D2D::fillRectangle(valueRectPos, mainColor);
									D2D::drawText(sTextPos, settingName + ":", WolferColor(255, 255, 255), textSize);
									D2D::drawText(valueTextPos, valueTextStr, WolferColor(255, 255, 255), textSize, (draggingSliderSettingPtr != sliderSettingBase));
								}
								else if (sliderSettingBase->valueType == ValueType::FLOAT_T) {
									SliderSetting<float>* floatSlider = static_cast<SliderSetting<float>*>(sliderSettingBase);
									float& value = (*floatSlider->valuePtr);
									float minValue = floatSlider->minValue;
									float maxValue = floatSlider->maxValue;

									if (draggingSliderSettingPtr == sliderSettingBase) {
										float draggingPercent = (mousePos.x - sRectPos.x) / (sRectPos.z - sRectPos.x);

										if (draggingPercent > 1.f)
											draggingPercent = 1.f;
										if (draggingPercent < 0.f)
											draggingPercent = 0.f;

										value = minValue + (maxValue - minValue) * draggingPercent;
									}

									float valuePercent = (value - minValue) / (maxValue - minValue);

									if (valuePercent > 1.f)
										valuePercent = 1.f;
									if (valuePercent < 0.f)
										valuePercent = 0.f;

									Vector4<float> valueRectPos = Vector4<float>(sRectPos.x, sRectPos.y, sRectPos.x + (sRectPos.z - sRectPos.x) * valuePercent, sRectPos.w);

									char valueText[10];
									sprintf_s(valueText, 10, "%.2f", value);
									std::string valueTextStr(valueText);

									Vector2<float> valueTextPos = Vector2<float>(sRectPos.z - textPaddingX - D2D::getTextWidth(valueTextStr, textSize, (draggingSliderSettingPtr != sliderSettingBase)), sTextPos.y);

									D2D::fillRectangle(valueRectPos, mainColor);
									D2D::drawText(sTextPos, settingName + ":", WolferColor(255, 255, 255), textSize);
									D2D::drawText(valueTextPos, valueTextStr, WolferColor(255, 255, 255), textSize, (draggingSliderSettingPtr != sliderSettingBase));
								}

								yOffset += textHeight + (textPaddingY * 2.f);
								break;
							}
							}
							D2D::fillRectangle(sRectPos, WolferColor(255, 255, 255, (int)(45 * setting->selectedAnim)));
							yOffset += settingSpace;
						}
						float endY = yOffset - settingSpace;

						float sLineWidth = 4.f * textSize * fontPercent;
						float sLinePaddingX = 1.f * textSize;
						Vector4<float> sLineRect = Vector4<float>(mRectPos.x + sLinePaddingX,
							startY,
							mRectPos.x + sLinePaddingX + sLineWidth,
							endY);

						D2D::fillRectangle(sLineRect, mainColor);
					}

					yOffset += moduleSpace;
				}
			}
		}

		if (showDescription && descriptionText != "NULL" && draggingWindowPtr == nullptr && draggingSliderSettingPtr == nullptr) {
			Vector2<float> mousePadding = Vector2<float>(15.f, 15.f);
			Vector4<float> rectPos = Vector4<float>(mousePos.x + mousePadding.x,
				mousePos.y + mousePadding.y,
				mousePos.x + mousePadding.x + D2D::getTextWidth(descriptionText, 0.8f) + 2.f * 2.f,
				mousePos.y + mousePadding.y + D2D::getTextHeight(descriptionText, 0.8f));

			Vector2<float> textPos = Vector2<float>(rectPos.x, rectPos.y).add(Vector2<float>(2.f, 0.f));
			D2D::fillRectangle(rectPos, WolferColor(0, 0, 0, 125));
			D2D::drawText(textPos, descriptionText, WolferColor(255, 255, 255), 0.8f);

		}

		isLeftClickDown = false;
		isRightClickDown = false;
	}

	using json = nlohmann::json;

	void onLoadConfig(void* confVoid) override {
		json* conf = reinterpret_cast<json*>(confVoid);
		std::string modName = this->getModuleName();

		if (conf->contains(modName)) {
			json obj = conf->at(modName);
			if (obj.is_null())
				return;

			if (obj.contains("enabled")) {
				this->setEnabled(obj.at("enabled").get<bool>());
			}

			for (auto& setting : getSettingList()) {
				std::string settingName = setting->name;

				if (obj.contains(settingName)) {
					json confValue = obj.at(settingName);
					if (confValue.is_null())
						continue;

					switch (setting->type) {
					case SettingType::BOOL_S: {
						BoolSetting* boolSetting = static_cast<BoolSetting*>(setting);
						(*boolSetting->value) = confValue.get<bool>();
						break;
					}
					case SettingType::KEYBIND_S: {
						KeybindSetting* keybindSetting = static_cast<KeybindSetting*>(setting);
						(*keybindSetting->value) = confValue.get<int>();
						break;
					}
					case SettingType::ENUM_S: {
						EnumSetting* enumSetting = static_cast<EnumSetting*>(setting);
						(*enumSetting->value) = confValue.get<int>();
						break;
					}
					case SettingType::COLOR_S: {
						ColorSetting* colorSetting = static_cast<ColorSetting*>(setting);
						(*colorSetting->colorPtr) = Colors::HexStringToColor(confValue.get<std::string>());
						break;
					}
					case SettingType::SLIDER_S: {
						SliderSettingBase* sliderSettingBase = static_cast<SliderSettingBase*>(setting);
						if (sliderSettingBase->valueType == ValueType::INT_T) {
							SliderSetting<int>* intSlider = static_cast<SliderSetting<int>*>(sliderSettingBase);
							(*intSlider->valuePtr) = confValue.get<int>();
						}
						else if (sliderSettingBase->valueType == ValueType::FLOAT_T) {
							SliderSetting<float>* floatSlider = static_cast<SliderSetting<float>*>(sliderSettingBase);
							(*floatSlider->valuePtr) = confValue.get<float>();
						}
						break;
					}
					}
				}
			}

			for (auto& window : windowList) {
				std::string windowName = window->name;

				if (obj.contains(windowName)) {
					json confValue = obj.at(windowName);
					if (confValue.is_null())
						continue;

					if (confValue.contains("isExtended")) {
						window->extended = confValue["isExtended"].get<bool>();
					}

					if (confValue.contains("pos")) {
						window->pos.x = confValue["pos"]["x"].get<float>();
						window->pos.y = confValue["pos"]["y"].get<float>();
					}
				}
			}
		}
	}

	void onSaveConfig(void* confVoid) override {
		json* conf = reinterpret_cast<json*>(confVoid);
		std::string modName = this->getModuleName();
		json obj = (*conf)[modName];

		obj["enabled"] = this->isEnabled();

		for (auto& setting : getSettingList()) {
			std::string settingName = setting->name;

			switch (setting->type) {
			case SettingType::BOOL_S: {
				BoolSetting* boolSetting = static_cast<BoolSetting*>(setting);
				obj[settingName] = (*boolSetting->value);
				break;
			}
			case SettingType::KEYBIND_S: {
				KeybindSetting* keybindSetting = static_cast<KeybindSetting*>(setting);
				obj[settingName] = (*keybindSetting->value);
				break;
			}
			case SettingType::ENUM_S: {
				EnumSetting* enumSetting = static_cast<EnumSetting*>(setting);
				obj[settingName] = (*enumSetting->value);
				break;
			}
			case SettingType::COLOR_S: {
				ColorSetting* colorSetting = static_cast<ColorSetting*>(setting);
				obj[settingName] = Colors::ColorToHexString((*colorSetting->colorPtr));
				break;
			}
			case SettingType::SLIDER_S: {
				SliderSettingBase* sliderSettingBase = static_cast<SliderSettingBase*>(setting);
				if (sliderSettingBase->valueType == ValueType::INT_T) {
					SliderSetting<int>* intSlider = static_cast<SliderSetting<int>*>(sliderSettingBase);
					obj[settingName] = (*intSlider->valuePtr);
				}
				else if (sliderSettingBase->valueType == ValueType::FLOAT_T) {
					SliderSetting<float>* floatSlider = static_cast<SliderSetting<float>*>(sliderSettingBase);
					obj[settingName] = (*floatSlider->valuePtr);
				}
				break;
			}
			}
		}

		for (auto& window : windowList) {
			obj[window->name]["isExtended"] = window->extended;
			obj[window->name]["pos"]["x"] = window->pos.x;
			obj[window->name]["pos"]["y"] = window->pos.y;
		}

		(*conf)[modName] = obj;
	}
};