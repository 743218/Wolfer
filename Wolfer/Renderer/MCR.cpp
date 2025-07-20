#include "MCR.h"
#include "../Client/Client.h"
#include "../Api/Game.h"

BaseActorRenderContext MCR::baseActorRenderCtx;
MinecraftUIRenderContext* MCR::renderCtx = nullptr;
ScreenContext* MCR::screenCtx = nullptr;
ShaderColor* MCR::currentColor = nullptr;
Tessellator* MCR::tessellator = nullptr;
mce::MaterialPtr* MCR::uiMaterial = nullptr;
mce::MaterialPtr* MCR::blendMaterial = nullptr;
Font* MCR::mcFont = nullptr;
Vector3<float> MCR::origin;
float MCR::deltaTime = 0.016f;
std::vector<MCR::Render3dData> MCR::quad3dRenderList;
std::vector<MCR::Render3dData> MCR::line3dRenderList;

void MCR::onRenderScreen(MinecraftUIRenderContext* ctx) {
	baseActorRenderCtx = BaseActorRenderContext(ctx->screenContext, ctx->clientInstance, ctx->clientInstance->mcGame);
	renderCtx = ctx;
	screenCtx = ctx->screenContext;
	currentColor = ctx->screenContext->currentShaderColor;
	tessellator = ctx->screenContext->tessellator;

	if (uiMaterial == nullptr)
		uiMaterial = mce::MaterialPtr::createMaterial(HashedString("ui_textured_and_glcolor"));

	if (blendMaterial == nullptr)
		blendMaterial = mce::MaterialPtr::createMaterial(HashedString("fullscreen_cube_overlay_blend"));

	mcFont = ctx->clientInstance->mcGame->mcFont;

	{
		static NameTags* nameTagsMod = ModuleManager::getModule<NameTags>();
		if (nameTagsMod->isEnabled())
			nameTagsMod->Render();
	}

	ModuleManager::onMCRender(renderCtx);
	MCR::renderCtx->flushText(0.f);
}

void MCR::onRenderWorld(LevelRenderer* levelRenderer, ScreenContext* screenContext) {

	quad3dRenderList.clear();
	line3dRenderList.clear();

	origin = levelRenderer->levelRendererPlayer->realOrigin;
	ModuleManager::onLevelRender();

	setColor(mce::Color(1.f, 1.f, 1.f, 1.f));
	if (!quad3dRenderList.empty()) {
		tessellator->begin(VertextFormat::QUAD);
		for (auto& quad3dRenderData : quad3dRenderList) {
			mce::Color mcColor = quad3dRenderData.color.toMCColor();
			tessellator->color(mcColor.r, mcColor.g, mcColor.b, mcColor.a);
			tessellator->vertex(quad3dRenderData.pos.x, quad3dRenderData.pos.y, quad3dRenderData.pos.z);
		}
		MeshHelpers::renderMeshImmediately(screenCtx, tessellator, blendMaterial);
		quad3dRenderList.clear();
	}

	if (!line3dRenderList.empty()) {
		tessellator->begin(VertextFormat::LINE_LIST);
		for (auto& line3dRenderData : line3dRenderList) {
			mce::Color mcColor = line3dRenderData.color.toMCColor();
			tessellator->color(mcColor.r, mcColor.g, mcColor.b, mcColor.a);
			tessellator->vertex(line3dRenderData.pos.x, line3dRenderData.pos.y, line3dRenderData.pos.z);
		}
		MeshHelpers::renderMeshImmediately(screenCtx, tessellator, blendMaterial);
		line3dRenderList.clear();
	}

	// Conicalhat
	{
		static ConicalHat* cone = ModuleManager::getModule<ConicalHat>();
		if (cone->isEnabled())
			cone->render();
	}
}

void MCR::setColor(const mce::Color& color) {
	currentColor->color = color;
	currentColor->dirty = true;
}

void MCR::setColor(const WolferColor& color) {
	currentColor->color = color.toMCColor();
	currentColor->dirty = true;
}

void MCR::drawLine(const Vector2<float>& start, const Vector2<float>& end, float thickness, const WolferColor& color) {
	setColor(color);
	tessellator->begin(VertextFormat::LINE_LIST);

	for (float i = -thickness / 2; i <= thickness / 2; i += 0.5f) {
		tessellator->vertex(start.x + i, start.y + i, 0.0f);
		tessellator->vertex(end.x + i, end.y + i, 0.0f);
	}

	MeshHelpers::renderMeshImmediately(screenCtx, tessellator, uiMaterial);
}

void MCR::drawText(const Vector2<float>& textPos, const std::string& textStr, const WolferColor& color, float textSize) {

	std::string text = textStr;
	mce::Color mcColor = color.toMCColor();

	RectangleArea rect = {};
	rect._x0 = textPos.x;
	rect._x1 = textPos.x;
	rect._y0 = textPos.y;
	rect._y1 = textPos.y;

	TextMeasureData textMeasureData = {};
	textMeasureData.fontSize = textSize;
	textMeasureData.linePadding = 0.f;
	textMeasureData.renderShadow = true;
	textMeasureData.showColorSymbol = false;
	textMeasureData.hideHyphen = false;

	static CaretMeasureData caretMeasureData = {};
	caretMeasureData.position = -1.f;
	caretMeasureData.shouldRender = false;

	renderCtx->drawText(mcFont, &rect, &text, &mcColor, mcColor.a, 0, &textMeasureData, &caretMeasureData);
}

float MCR::getTextWidth(const std::string& textStr, float textSize) {
	std::string text = textStr;
	return renderCtx->getLineLength(mcFont, &text, textSize, false);
}

float MCR::getTextHeight(float textSize) {
	return (9.f * textSize);
}

void MCR::drawQuad(const Vector2<float>& p1, const Vector2<float>& p2, const Vector2<float>& p3, const Vector2<float>& p4, const WolferColor& color) {
	setColor(color);
	tessellator->begin(VertextFormat::QUAD, 4);

	tessellator->vertex(p1.x, p1.y, 0.f);
	tessellator->vertex(p2.x, p2.y, 0.f);
	tessellator->vertex(p3.x, p3.y, 0.f);
	tessellator->vertex(p4.x, p4.y, 0.f);

	MeshHelpers::renderMeshImmediately(screenCtx, tessellator, uiMaterial);
}

void MCR::fillRectangle(const Vector4<float>& rect, const WolferColor& color) {
	drawQuad(Vector2<float>(rect.x, rect.w), Vector2<float>(rect.z, rect.w), Vector2<float>(rect.z, rect.y), Vector2<float>(rect.x, rect.y), color);
}

/*void MCR::drawItem(const Vector2<float>& pos, ItemStack* itemStack, float scale, bool showDurabilityBar) {
	baseActorRenderCtx.itemRenderer->renderGuiItemNew(&baseActorRenderCtx, itemStack, 0, pos.x, pos.y, 1.f, scale, false);

	if (itemStack->item.get()->isGlint(itemStack)) {
		setColor(mce::Color(1.f, 1.f, 1.f, 1.f));
		baseActorRenderCtx.itemRenderer->renderGuiItemNew(&baseActorRenderCtx, itemStack, 0, pos.x, pos.y, 1.f, scale, true);
	}

	if (showDurabilityBar) {
		Item* item = itemStack->item.get();
		short maxDamage = item->getMaxDamage();
		short damageValue = item->getDamageValue(itemStack->mUserData);
		short durability = maxDamage - damageValue;

		if (maxDamage != 0 && durability != maxDamage) {
			float percent = (float)durability / (float)maxDamage;
			WolferColor color = Colors::lerp(WolferColor(255, 0, 0), WolferColor(0, 255, 0), percent);
			Vector4<float> barRect = Vector4<float>(pos.x + (2.f * scale), pos.y + (13.5f * scale), pos.x + (14.f * scale), pos.y + (15.f * scale));
			Vector4<float> durabilityRect = Vector4<float>(barRect.x, barRect.y, barRect.x + ((barRect.z - barRect.x) * percent), barRect.y + (1.f * scale));

			fillRectangle(barRect, WolferColor(0, 0, 0, 255));
			fillRectangle(durabilityRect, color);
		}
	}
}*/
void MCR::drawItem(const Vector2<float>& pos, ItemStack* itemStack, float scale, bool showDurabilityBar) {
	baseActorRenderCtx.itemRenderer->renderGuiItemNew(&baseActorRenderCtx, itemStack, 0, pos.x, pos.y, 1.f, scale, false);

	if (itemStack->item.get()->isGlint(itemStack)) {
		setColor(mce::Color(1.f, 1.f, 1.f, 1.f));
		baseActorRenderCtx.itemRenderer->renderGuiItemNew(&baseActorRenderCtx, itemStack, 0, pos.x, pos.y, 1.f, scale, true);
	}

	if (showDurabilityBar) {
		Item* item = itemStack->item.get();
		const short maxDamage = item->getMaxDamage();
		const short damageValue = item->getDamageValue(itemStack->mUserData);
		const short durability = maxDamage - damageValue;

		if (maxDamage != 0 && durability != maxDamage) {
			float percent = (float)durability / (float)maxDamage;
			WolferColor color = Colors::lerp(WolferColor(255, 0, 0), WolferColor(0, 255, 0), percent);
			Vector4<float> barRect = Vector4<float>(pos.x + (2.f * scale), pos.y + (13.5f * scale), pos.x + (14.f * scale), pos.y + (15.f * scale));
			Vector4<float> durabilityRect = Vector4<float>(barRect.x, barRect.y, barRect.x + ((barRect.z - barRect.x) * percent), barRect.y + (1.f * scale));

			fillRectangle(barRect, WolferColor(0, 0, 0, 255));
			fillRectangle(durabilityRect, color);
		}
	}
}

void MCR::drawItemDurability(ItemStack* itemStack, Vector2<float> itemPos, float scale, float opacity, int mode) {
	Item* item = itemStack->item.get();
	float maxDamage = item->getMaxDamage();
	float damageValue = item->getDamageValue(itemStack->mUserData);
	float currentDamage = maxDamage - damageValue;
	if (currentDamage == maxDamage) return;
	float percentage = currentDamage / maxDamage * 100;
	float fraction = percentage / 100;
	WolferColor durabilityColor = Colors::lerp(WolferColor(255, 0, 0), WolferColor(0, 255, 0), fraction);
	if (mode == 0 || mode == 2) {
		Vector4<float> barPos = Vector4<float>(itemPos.x + 1.5f * scale, itemPos.y + 14.f * scale, itemPos.x + 15.f * scale, itemPos.y + 16.f * scale);
		Vector4<float> rect = Vector4<float>(barPos.x, barPos.y, barPos.x + ((barPos.z - barPos.x) / 100 * (int)percentage), barPos.w - 1.f * scale);
		MCR::fillRectangle(barPos, WolferColor(static_cast<u_int>(0.f, 0.f, 0.f)));
		MCR::fillRectangle(rect, durabilityColor);
	}
	if (mode == 1 || mode == 2) {
		std::string duraText = std::to_string((int)percentage);
		MCR::drawText(Vector2<float>(itemPos.x + 7.5f, itemPos.y - 6.5f), duraText, durabilityColor, scale);
	}
}

void MCR::drawBox3dFilled(const AABB& aabb, const WolferColor& color, const WolferColor& lineColor, float scale) {
	Vector3<float> diff = aabb.upper.sub(aabb.lower);
	Vector3<float> newLower = aabb.lower.sub(MCR::origin);

	Vector3<float> vertices[8] = {
		{newLower.x, newLower.y, newLower.z},
		{newLower.x + diff.x, newLower.y, newLower.z},
		{newLower.x, newLower.y, newLower.z + diff.z},
		{newLower.x + diff.x, newLower.y, newLower.z + diff.z},

		{newLower.x, newLower.y + diff.y, newLower.z},
		{newLower.x + diff.x, newLower.y + diff.y, newLower.z},
		{newLower.x, newLower.y + diff.y, newLower.z + diff.z},
		{newLower.x + diff.x, newLower.y + diff.y, newLower.z + diff.z} 
	};

	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(scale), 0.f, glm::vec3(1.0f, 1.0f, 1.0f));
	Vector3<float> newLowerReal = newLower.add(Vector3<float>(0.5f, 0.5f, 0.5f));

	for (int i = 0; i < 8; i++) {
		glm::vec4 rotatedVertex = rotationMatrix * glm::vec4(vertices[i].x - newLowerReal.x, vertices[i].y - newLowerReal.y, vertices[i].z - newLowerReal.z, 0.0f);
		vertices[i] = Vector3<float>(rotatedVertex.x + newLowerReal.x, rotatedVertex.y + newLowerReal.y, rotatedVertex.z + newLowerReal.z);
	}

	if (color.a > 0) {
		static uint8_t v[48] = { 5, 7, 6, 4, 4, 6, 7, 5, 1, 3, 2, 0, 0, 2, 3, 1, 4, 5, 1, 0, 0, 1, 5, 4, 6, 7, 3, 2, 2, 3, 7, 6, 4, 6, 2, 0, 0, 2, 6, 4, 5, 7, 3, 1, 1, 3, 7, 5 };
		for (int i = 0; i < 48; i++)
			quad3dRenderList.push_back(Render3dData(vertices[v[i]], color));
	}

	if (lineColor.a > 0) {
		static auto drawLine = [](const Vector3<float>& start, const Vector3<float>& end, const WolferColor& _color) {
			line3dRenderList.push_back(Render3dData(start, _color));
			line3dRenderList.push_back(Render3dData(end, _color));
		};

		// Top square
		drawLine(vertices[4], vertices[5], lineColor);
		drawLine(vertices[5], vertices[7], lineColor);
		drawLine(vertices[7], vertices[6], lineColor);
		drawLine(vertices[6], vertices[4], lineColor);

		// Bottom Square
		drawLine(vertices[0], vertices[1], lineColor);
		drawLine(vertices[1], vertices[3], lineColor);
		drawLine(vertices[3], vertices[2], lineColor);
		drawLine(vertices[2], vertices[0], lineColor);

		// Sides
		drawLine(vertices[0], vertices[4], lineColor);
		drawLine(vertices[1], vertices[5], lineColor);
		drawLine(vertices[2], vertices[6], lineColor);
		drawLine(vertices[3], vertices[7], lineColor);
	}
}

bool MCR::worldToScreen(const Vector3<float>& pos, Vector2<float>& out) {

	ClientHMDState* hmdState = g_Data.clientInstance->getHMDState();

	Vector2<float> screenSize = g_Data.clientInstance->guiData->windowSizeScaled;
	Vector3<float> relativePos = pos.sub(origin);

	glm::mat4 mvpMatrix = hmdState->mLastLevelProjMatrix * hmdState->mLastLevelViewMatrix;
	glm::vec4 clipSpacePosition = mvpMatrix * glm::vec4(relativePos.x, relativePos.y, relativePos.z, 1.0f);

	if (clipSpacePosition.w <= 0.0f)
		return false;

	clipSpacePosition /= clipSpacePosition.w;

	out = Vector2<float>(
		(clipSpacePosition.x + 1.0f) * (0.5f * screenSize.x),
		(1.0f - clipSpacePosition.y) * (0.5f * screenSize.y)
	);

	return true;
}

void MCR::drawTextInWorld(const Vector3<float>& pos, const std::string& textStr, const WolferColor& color, float textSize) {
	Vector2<float> textPos;
	const float textWidth = getTextWidth(textStr, textSize);
	const float textHeight = getTextHeight(textSize);
	if (worldToScreen(pos.add(0.5f), textPos)) {
		textPos.y -= textHeight;
		textPos.x -= textWidth / 2;
		drawText(textPos, textStr, color, textSize);
	}
}