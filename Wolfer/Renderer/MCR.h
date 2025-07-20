#pragma once
#include "../Api/Render/BaseActorRenderContext.h"
#include "../Api/Render/MinecraftUIRenderContext.h"
#include "../Api/Render/ScreenContext.h"
#include "../Api/Render/Tessellator.h"
#include "../Api/Render/MeshHelpers.h"

#include "../Api/Utils/Maths.h"
#include "../Api/Utils/ColorUtil.h"

namespace MCR {

	struct Render3dData {
		Vector3<float> pos;
		WolferColor color;

		Render3dData(const Vector3<float>& _pos, const WolferColor& _color) {
			pos = _pos;
			color = _color;
		}
	};

	extern BaseActorRenderContext baseActorRenderCtx;
	extern MinecraftUIRenderContext* renderCtx;
	extern ScreenContext* screenCtx;
	extern ShaderColor* currentColor;
	extern Tessellator* tessellator;
	extern mce::MaterialPtr* uiMaterial;
	extern mce::MaterialPtr* blendMaterial;
	extern Font* mcFont;
	extern Vector3<float> origin;
	extern float deltaTime;
	extern std::vector<Render3dData> quad3dRenderList;
	extern std::vector<Render3dData> line3dRenderList;

	void drawLine(const Vector2<float>& start, const Vector2<float>& end, float thickness, const WolferColor& color);
	void onRenderScreen(MinecraftUIRenderContext* ctx);
	void onRenderWorld(LevelRenderer* levelRenderer, ScreenContext* screenContext);
	void setColor(const mce::Color& color);
	void setColor(const WolferColor& color);
	void drawText(const Vector2<float>& textPos, const std::string& textStr, const WolferColor& color, float textSize = 1.f);
	float getTextWidth(const std::string& textStr, float textSize = 1.f);
	float getTextHeight(float textSize = 1.f);
	void drawQuad(const Vector2<float>& p1, const Vector2<float>& p2, const Vector2<float>& p3, const Vector2<float>& p4, const WolferColor& color);
	void fillRectangle(const Vector4<float>& rect, const WolferColor& color);
	void drawItem(const Vector2<float>& pos, ItemStack* itemStack, float scale, bool showDurabilityBar);
	void drawItemDurability(ItemStack* item, Vector2<float> itemPos, float scale, float opacity, int mode);
	void drawBox3dFilled(const AABB& aabb, const WolferColor& color, const WolferColor& lineColor, float scale = 1.0f);
	void drawTextInWorld(const Vector3<float>& pos, const std::string& textStr, const WolferColor& color, float textSize = 1.f);
	bool worldToScreen(const Vector3<float>& pos, Vector2<float>& out);
}