#include <components/CameraComponent.hpp>

using namespace NAISE::Engine;
using namespace glm;

CameraComponent::CameraComponent() {
	/* --------------------------------------------- */
	// Load video-settings.ini
	/* --------------------------------------------- */
	// init reader for ini files
	//INIReader videoIniReader("assets/video-settings.ini");

	// load window values from ini file
	// first param: section [window], second param: property name, third param: default value
	viewportWidth = 1024;
	viewportHeight = 768;

	// load camera values from ini file
	fov = 60.0f;
	near = 0.1f;
	far = 1000.0f;

	projectionMatrix = perspectiveFov<double>(glm::radians(getFovY()), viewportWidth, viewportHeight, near, far);
	frustum = Frustum(fov, getFovY(), near, far);
	positionMatrix = mat4(1);
}

CameraComponent::CameraComponent(double fov, double near, double far, int viewportWidth, int viewportHeight)
		: fov(fov),
		  near(near),
		  far(far),
		  viewportWidth(viewportWidth),
		  viewportHeight(viewportHeight),
		  positionMatrix(glm::mat4(0)) {
	projectionMatrix = perspectiveFov<double>(glm::radians(getFovY()), viewportWidth, viewportHeight, near, far);
	frustum = Frustum(fov, getFovY(), near, far);
	positionMatrix = mat4(1);
}

glm::mat4 CameraComponent::getViewMatrix() const {
	return glm::inverse(this->positionMatrix);
}

glm::mat4 CameraComponent::getProjectionMatrix() const {
	return this->projectionMatrix;
}

glm::vec3 CameraComponent::getCameraPosition() const{
	return vec3(positionMatrix * vec4(0,0,0,1));
}

double CameraComponent::getFovY()const {
	return fov * ((double) viewportHeight / (double) viewportWidth);
}

//AABB Camera::calculateViewFrustrum() {
//	return AABB(frustum.getBoundingVolume());
//}
