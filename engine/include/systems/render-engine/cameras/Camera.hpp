#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include "../utilities/Utils.h"
#include "../InputHandler.h"
#include "../scene/Entity.h"
#include "../frustum-culling/Frustum.h"

// Because f*** y** windows.h
#undef far
#undef near

namespace NAISE {
namespace Engine {

class Camera
		: public Entity,
		  public InputHandler {
public:
	Camera();
	Camera(double fov, double near, double far, int viewportWidth, int viewportHeight);

	void update(std::chrono::microseconds deltaTime) override;

	glm::mat4 getViewMatrix() const;

	glm::mat4 getProjectionMatrix() const;

	glm::vec3 getCameraPosition() const;

	Frustum frustum;

protected:
	double fov;
	double near;
	double far;

	//TODO: update viewport values on resize.
	int viewportWidth;
	int viewportHeight;

	glm::vec3 cameraPosition;

	glm::mat4 positionMatrix;
	glm::mat4 projectionMatrix;

	/**
	 * Returns the vertical field of view in degrees
	 * @return
	 */
	double getFovY() const;
	AABB calculateViewFrustrum();
};

}
}