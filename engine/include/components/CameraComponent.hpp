#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <systems/render-engine/frustum-culling/Frustum.hpp>

#include "Component.hpp"

// cuz windows.h
#undef far
#undef near

using namespace NAISE::RenderCore;

namespace NAISE {
namespace Engine {

class CameraComponent: public Component {
public:
	CameraComponent();
	CameraComponent(double fov, double near, double far, int viewportWidth, int viewportHeight);

	glm::mat4 getViewMatrix() const;

	glm::mat4 getProjectionMatrix() const;

	glm::vec3 getCameraPosition() const;

	Frustum frustum;

	AABB calculateViewFrustrum();

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
};

}
}