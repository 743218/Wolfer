#pragma once

#include <string>
#include <stdint.h>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <d2d1.h>

#include "../Core/mce.h"
#include "TimerUtil.h"

struct WolferColor {
	union {
		struct {
			uint8_t r, g, b, a;
		};
		int arr[4];
	};

	WolferColor(uint8_t red = 255, uint8_t green = 255, uint8_t blue = 255, uint8_t alpha = 255) {
		this->r = red;
		this->g = green;
		this->b = blue;
		this->a = alpha;
	}

	WolferColor(const mce::Color& color) {
		this->r = static_cast<uint8_t>(color.r * 255.f);
		this->g = static_cast<uint8_t>(color.g * 255.f);
		this->b = static_cast<uint8_t>(color.b * 255.f);
		this->a = static_cast<uint8_t>(color.a * 255.f);
	}

	bool operator==(const WolferColor& other) const {
		return (r == other.r && g == other.g && b == other.b && a == other.a);
	}

	D2D1_COLOR_F toD2D1Color() const {
		return D2D1_COLOR_F(static_cast<float>(r) / 255.f, static_cast<float>(g) / 255.f, static_cast<float>(b) / 255.f, static_cast<float>(a) / 255.f);
	}

	mce::Color toMCColor() const {
		return mce::Color(static_cast<float>(r) / 255.f, static_cast<float>(g) / 255.f, static_cast<float>(b) / 255.f, static_cast<float>(a) / 255.f);
	}
};

namespace Colors {

	inline uint32_t ColorToUInt(const WolferColor& color) {
		uint32_t result = (static_cast<uint32_t>(color.a) << 24) |
			(static_cast<uint32_t>(color.r) << 16) |
			(static_cast<uint32_t>(color.g) << 8) |
			color.b;
		return result;
	}

	inline std::string ColorToHexString(const WolferColor& color) {
		std::stringstream ss;
		ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(color.r);
		ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(color.g);
		ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(color.b);
		ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(color.a);
		return ss.str();
	}

	inline WolferColor HexStringToColor(std::string hexString) {
		if (hexString.substr(0, 2) == "0x") {
			hexString = hexString.substr(2);
		}

		if (hexString.length() != 8) {
			return WolferColor(0, 0, 0);
		}

		std::stringstream ss;
		ss << std::hex << hexString;

		uint32_t colorValue;
		ss >> colorValue;

		WolferColor color;
		color.r = static_cast<uint8_t>((colorValue >> 24) & 0xFF);
		color.g = static_cast<uint8_t>((colorValue >> 16) & 0xFF);
		color.b = static_cast<uint8_t>((colorValue >> 8) & 0xFF);
		color.a = static_cast<uint8_t>(colorValue & 0xFF);

		return color;
	}

	inline void ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b) {
		if (s == 0.0f) {
			out_r = out_g = out_b = v;
			return;
		}

		h = fmodf(h, 1.0f) / (60.0f / 360.0f);
		int i = static_cast<int>(h);
		float f = h - static_cast<float>(i);
		float p = v * (1.0f - s);
		float q = v * (1.0f - s * f);
		float t = v * (1.0f - s * (1.0f - f));

		switch (i) {
		case 0: out_r = v; out_g = t; out_b = p; break;
		case 1: out_r = q; out_g = v; out_b = p; break;
		case 2: out_r = p; out_g = v; out_b = t; break;
		case 3: out_r = p; out_g = q; out_b = v; break;
		case 4: out_r = t; out_g = p; out_b = v; break;
		case 5:
		default: out_r = v; out_g = p; out_b = q; break;
		}
	}

	inline WolferColor lerp(const WolferColor& start, const WolferColor& end, float t) {
		WolferColor result;
		result.r = static_cast<uint8_t>(start.r + (end.r - start.r) * t);
		result.g = static_cast<uint8_t>(start.g + (end.g - start.g) * t);
		result.b = static_cast<uint8_t>(start.b + (end.b - start.b) * t);
		result.a = static_cast<uint8_t>(start.a + (end.a - start.a) * t);
		return result;
	}

	inline WolferColor getRainbowColor(float seconds, float saturation, float brightness, long index) {
		float currentHue = (((TimerUtil::getCurrentMs() + index) % static_cast<int>(seconds * 1000)) / (float)(seconds * 1000));
		float red = 0.f, green = 0.f, blue = 0.f;
		ColorConvertHSVtoRGB(currentHue, saturation, brightness, red, green, blue);
		return WolferColor(static_cast<uint8_t>(red * 255.f), static_cast<uint8_t>(green * 255.f), static_cast<uint8_t>(blue * 255.f));
	}

	inline WolferColor getWaveColor(const WolferColor& startColor, const WolferColor& endColor, long index) {
		uint64_t currentMs = TimerUtil::getCurrentMs();
		double percent = ((currentMs - index) % 2000 / 2000.0) * 2.0;
		if (static_cast<int>(percent) % 2 == 1) {
			percent = 1.0 - (percent - static_cast<int>(percent));
		}
		else {
			percent = percent - static_cast<int>(percent);
		}
		double inverse = 1.0 - percent;
		int r = static_cast<int>(startColor.r * inverse + endColor.r * percent);
		int g = static_cast<int>(startColor.g * inverse + endColor.g * percent);
		int b = static_cast<int>(startColor.b * inverse + endColor.b * percent);
		int a = static_cast<int>(startColor.a * inverse + endColor.a * percent);
		return WolferColor(r, g, b, a);
	}
}
