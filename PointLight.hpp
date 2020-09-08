#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "nanogui/nanogui.h"

using namespace std;
using namespace nanogui;

class PointLight {
public:
	glm::vec3 lightPosition = glm::vec3(0.0f, 0.0f, 6.0f);
	Color ambientColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
	Color diffuseColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
	Color specularColor = Color(0.0f, 0.0f, 0.0f, 1.0f);

	Color ambientBlack = Color(0.0f, 0.0f, 0.0f, 1.0f);
	Color diffuseBlack = Color(0.0f, 0.0f, 0.0f, 1.0f);
	Color specularBlack = Color(0.0f, 0.0f, 0.0f, 1.0f);

	float revolutionSpeed = 1.0f;

	void rotateX() {
		float rad = glm::radians(revolutionSpeed);
		float a1 = cos(rad);
		float a2 = sin(rad);
		lightPosition.y = a1 * lightPosition.y - a2 * lightPosition.z;
		lightPosition.z = a2 * lightPosition.y + a1 * lightPosition.z;
	}

	void rotateY() {
		float rad = glm::radians(revolutionSpeed);
		float a1 = cos(rad);
		float a2 = sin(rad);
		lightPosition.x = a1 * lightPosition.x + a2 * lightPosition.z;
		lightPosition.z = -a2 * lightPosition.x + a1 * lightPosition.z;
	}

	void rotateZ() {
		float rad = glm::radians(revolutionSpeed);
		float a1 = cos(rad);
		float a2 = sin(rad);
		lightPosition.x = a1 * lightPosition.x - a2 * lightPosition.y;
		lightPosition.y = a2 * lightPosition.x + a1 * lightPosition.y;
	}

	void reset() {
		lightPosition.x = 0.0f;
		lightPosition.y = 0.0f;
		lightPosition.z = 6.0f;
	}
};