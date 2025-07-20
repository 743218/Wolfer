#pragma once
#include "../../Api/Game.h"
#include "../../Client/Managers/FriendManager/FriendManager.h"

namespace TargetUtil {
	AABB makeAABB(float width, float height, const Vec3<float>& lowerPos);
	bool isTargetValid(Actor* target, bool isMob = false, bool friendCheck = true, float rangeCheck = 999999.f);
	static bool sortByDist(Actor* a1, Actor* a2) {
		return a1->getPos().dist(g_Data.getLocalPlayer()->getPos()) < a2->getPos().dist(g_Data.getLocalPlayer()->getPos());
	}
}