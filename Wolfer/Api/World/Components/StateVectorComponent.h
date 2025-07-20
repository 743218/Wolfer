#pragma once
#include "../../../Api/Utils/Maths.h"

struct StateVectorComponent {
    Vector3<float> pos;
    Vector3<float> oldPos;
    Vector3<float> velocity;
};
