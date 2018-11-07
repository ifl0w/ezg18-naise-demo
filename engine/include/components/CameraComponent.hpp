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
	CameraComponent(double fovX, double near, double far, int viewportWidth, int viewportHeight);
	CameraComponent(double fovY, double near, double far, double aspectRatio);

	glm::mat4 getViewMatrix() const;

	glm::mat4 getProjectionMatrix() const;

	glm::vec3 getCameraPosition() const;

	Frustum frustum;

	bool active = false;

	AABB calculateViewFrustrum();

protected:
	double fovY; // fov in Y direction in radians
	double fovX; // fov in X direction in radians
	double aspectRatio;
	double near;
	double far;

	glm::vec3 cameraPosition;

	glm::mat4 positionMatrix;
	//TODO: update aspect ratio and projection values on resize.
	glm::mat4 projectionMatrix;

	/**
	 * Returns the vertical field of view in degrees
	 * @return
	 */
	static double getFovY(double fovX, double aspectRatio);
	static double getFovX(double fovY, double aspectRatio);
};

}
}