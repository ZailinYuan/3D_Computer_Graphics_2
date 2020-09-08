#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "DirectionLight.hpp"

using namespace std;

class Camera {

public:
	
	Camera(DirectionLight* camlight) {
		this->camlight = camlight;
	}

	// Camera positions:
	int rotateUnit = 10;									// Rotate 10 degree each time by default.
	int revolutionRate = 1;

	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 6.0f);
	// cameraUp, cameraFront must be unit vector:
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // Default intitial, don't change it.

								// view = glm::lookAt(cameraPos, targetPosition, worldUpVector);
								// view = glm::lookAt(cameraPos, cameraFront + cameraPos, worldVector); 
								// Since cameraPos and worldVector are fixed, the only thing we can change are cameraFront. 

	// get camera right:
	glm::vec3 getRight() {
		return normalize(cross(cameraUp, cameraFront));
	}

	// Camera rotate methods:
	void rotateRightPlus() {
		rotateAroundRight(1);
	}

	void rotateRightMinus() {
		rotateAroundRight(-1);
	}

	void rotateUpPlus() {
		rotateAroundUp(1);
	}

	void rotateUpMinus() {
		rotateAroundUp(-1);
	}

	void rotateFrontPlus() {
		rotateAroundFront(1);
	}

	void rotateFrontMinus() {
		rotateAroundFront(-1);
	}

	// Camera view:
	glm::mat4 camLookAt() {
		return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}

	// Camera revolution:
	void revolutionHorizontalPlus() {

	}

	void revolutionHorizontalMinus() {

	}

	void revolutionVerticalPlus() {
		revolutionVertical(1);
	}

	void revolutionVerticalMinus() {
		revolutionVertical(-1);
	}

	// Camera reset:
	void reset() {
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraPos[0] = 0.0f;
		cameraPos[1] = 0.0f;
		cameraPos[2] = 6.0f;
		rotateUnit = 10;
	}

private:
	DirectionLight* camlight;

	void rotateAroundUp(int dir) {
		float rad = glm::radians(float(dir * rotateUnit));
		float a1 = cos(rad);
		float a2 = sin(rad);
		cameraFront.x = a1 * cameraFront.x + a2 * cameraFront.z;
		cameraFront.z = -a2 * cameraFront.x + a1 * cameraFront.z;
	}

	void rotateAroundRight(int dir) {
		float rad = glm::radians(float(dir * rotateUnit));
		float a1 = cos(rad);
		float a2 = sin(rad);
		cameraUp.y = a1 * cameraUp.y - a2 * cameraUp.z;
		cameraUp.z = a2 * cameraUp.y + a1 * cameraUp.z;
		cameraFront.y = a1 * cameraFront.y - a2 * cameraFront.z;
		cameraFront.z = a2 * cameraFront.y + a1 * cameraFront.z;
	}

	void rotateAroundFront(int dir) {
		float rad = glm::radians(float(dir * rotateUnit));
		float a1 = cos(rad);
		float a2 = sin(rad);
		cameraUp.x = a1 * cameraUp.x - a2 * cameraUp.y;
		cameraUp.y = a2 * cameraUp.x + a1 * cameraUp.y;
	}

	void revolutionHorizontal(int dir) {

	}

	void revolutionVertical(int dir) {

	}
};