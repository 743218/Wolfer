#pragma once
#include "../../Api/Game.h"

namespace WorldUtil {
	Block* getBlock(const BlockPos& blockPos);
	BlockLegacy* getBlockLegacy(const BlockPos& blockPos);
	bool isContainer(const BlockPos& blockPos);
	bool canBuildOn(const BlockPos& blockPos);
	float distanceToPoint(const Vector3<float>& pos, const Vector3<float>& point);
	float distanceToEntity(const Vector3<float>& pos, Actor* entity);
	float distanceToBlock(const Vector3<float>& pos, const BlockPos& blockPos);
	float getSeenPercent(const Vector3<float>& pos, const AABB& aabb);
	float getSeenPercent(const Vector3<float>& pos, Actor* actor);
	float getExposure(const Vector3<float>& pos, const AABB& aabb, const int& ignoredBlock);
	bool rayCast(const Vector3<float>& start, const Vector3<float>& end, const int& ignoredBlock);
	float sign(const float f);
	float boundary(float start, float distance);
	std::string getEntityNameTags(Actor* entity);
}