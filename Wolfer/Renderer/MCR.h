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
		Vec3<float> pos;
		WolferColor color;

		Render3dData(const Vec3<float>& _pos, const WolferColor& _color) {
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
	extern Vec3<float> origin;
	extern float deltaTime;
	extern std::vector<Render3dData> quad3dRenderList;
	extern std::vector<Render3dData> line3dRenderList;

	void DrawLine(const Vec2<float>& start, const Vec2<float>& end, float thickness, const WolferColor& color);
	void onRenderScreen(MinecraftUIRenderContext* ctx);
	void onRenderWorld(LevelRenderer* levelRenderer, ScreenContext* screenContext);
	void setColor(const mce::Color& color);
	void setColor(const WolferColor& color);
	void drawText(const Vec2<float>& textPos, const std::string& textStr, const WolferColor& color, float textSize = 1.f);
	float getTextWidth(const std::string& textStr, float textSize = 1.f);
	float getTextHeight(float textSize = 1.f);
	void drawQuad(const Vec2<float>& p1, const Vec2<float>& p2, const Vec2<float>& p3, const Vec2<float>& p4, const WolferColor& color);
	void fillRectangle(const Vec4<float>& rect, const WolferColor& color);
	void drawItem(const Vec2<float>& pos, ItemStack* itemStack, float scale, bool showDurabilityBar);
	void drawItemDurability(ItemStack* item, Vec2<float> itemPos, float scale, float opacity, int mode);
	void drawBox3dFilled(const AABB& aabb, const WolferColor& color, const WolferColor& lineColor, float scale = 1.0f);
	void drawTextInWorld(const Vec3<float>& pos, const std::string& textStr, const WolferColor& color, float textSize = 1.f);
	bool worldToScreen(const Vec3<float>& pos, Vec2<float>& out);
}