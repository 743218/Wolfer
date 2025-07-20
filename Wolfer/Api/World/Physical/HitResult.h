#pragma once
#include "../../../Api/Utils/Memory.h"
#include "../../../Api/Utils/Maths.h"

enum class HitResultType : int32_t {
    BLOCK,
    ENTITY,
    ENTITY_OUT_OF_RANGE,
    AIR
};

class HitResult {
public:
    Vector3<float> startPos;
    Vector3<float> rayDirection;
    HitResultType type;
    int32_t selectedFace;
    BlockPos blockPos;
    Vector3<float> endPos;
};