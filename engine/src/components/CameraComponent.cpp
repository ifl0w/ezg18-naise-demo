#include <components/CameraComponent.hpp>

using namespace NAISE::Engine;
using namespace glm;

CameraComponent::CameraComponent() {
	// load camera values from ini file
	aspectRatio = (double) 4 / (double) 3;
	fovX = radians(60.0f);
	fovY = getFovY(fovX, aspectRatio);
	near = 0.1f;
	far = 1000.0f;

	projectionMatrix = perspective<double>(getFovY(fovX, aspectRatio), aspectRatio, near, far);
	frustum = Frustum(fovX, fovY, near, far);
	positionMatrix = mat4(1);
}

CameraComponent::CameraComponent(double fovX, double near, double far, int viewportWidth, int viewportHeight)
		: fovX(fovX),
		  fovY(getFovY(fovX, (double) viewportWidth / (double) viewportHeight)),
		  aspectRatio((double) viewportWidth / (double) viewportHeight),
		  near(near),
		  far(far),
		  positionMatrix(glm::mat4(0)) {

	projectionMatrix = perspectiveFov<double>(fovY, viewportWidth, viewportHeight, near, far);
	frustum = Frustum(fovX, fovY, near, far);
	positionMatrix = mat4(1);
}

CameraComponent::CameraComponent(double yfov, double near, double far, double aspectRatio)
		: fovX(getFovX(yfov, aspectRatio)),
		  fovY(yfov),
		  aspectRatio(aspectRatio),
		  near(near),
		  far(far),
		  positionMatrix(glm::mat4(0)) {

	projectionMatrix = perspective<double>(getFovY(fovX, aspectRatio), aspectRatio, near, far);
	frustum = Frustum(fovX, getFovY(fovY, aspectRatio), near, far);
	positionMatrix = mat4(1);
}

glm::mat4 CameraComponent::getViewMatrix() const {
	return glm::inverse(this->positionMatrix);
}

glm::mat4 CameraComponent::getProjectionMatrix() const {
	return this->projectionMatrix;
}

glm::vec3 CameraComponent::getCameraPosition() const {
	return vec3(positionMatrix * vec4(0, 0, 0, 1));
}

double CameraComponent::getFovY(double fovX, double aspectRatio) {
	return fovX / aspectRatio;
}

double CameraComponent::getFovX(double fovY, double aspectRatio) {
	return fovY * aspectRatio;
}

AABB CameraComponent::calculateViewFrustrum() {
	return AABB(frustum.getBoundingVolume());
}



