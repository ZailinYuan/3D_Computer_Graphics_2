#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#pragma once

#include "nanogui/nanogui.h"

using namespace std;
using namespace nanogui;

class DirectionLight {
public:
	glm::vec3 lightDirection = glm::vec3(0.0f, -1.0f, -1.0f);
	Color ambientColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
	Color diffuseColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
	Color specularColor = Color(0.0f, 0.0f, 0.0f, 1.0f);

	Color ambientBlack = Color(0.0f, 0.0f, 0.0f, 1.0f);
	Color diffuseBlack = Color(0.0f, 0.0f, 0.0f, 1.0f);
	Color specularBlack = Color(0.0f, 0.0f, 0.0f, 1.0f);
};