#pragma once
#include <stack>
#include "../../Api/Utils/Maths.h"

class MatrixStack {
public:
	std::stack<glm::mat4x4> stack;
	bool isDirty;
};