#include "Camera.h"
#include "../Engine.h"

Camera::Camera() {
	/* --------------------------------------------- */
	// Load video-settings.ini
	/* --------------------------------------------- */
	// init reader for ini files
	INIReader videoIniReader("resources/video-settings.ini");

	// load window values from ini file
	// first param: section [window], second param: property name, third param: default value
	viewportWidth = videoIniReader.GetInteger("window", "width", 1024);
	viewportHeight = videoIniReader.GetInteger("window", "height", 768);

	// load camera values from ini file
	fov = videoIniReader.GetReal("camera", "fov", 60.0f);
	near = videoIniReader.GetReal("camera", "near", 0.1f);
	far = videoIniReader.GetReal("camera", "far", 1000.0f);

	projectionMatrix = perspectiveFov<double>(glm::radians(getFovY()), viewportWidth, viewportHeight, near, far);
	frustum = Frustum(fov, getFovY(), near, far);
}

Camera::Camera(double fov, double near, double far, int viewportWidth, int viewportHeight)
		: fov(fov),
		  near(near),
		  far(far),
		  viewportWidth(viewportWidth),
		  viewportHeight(viewportHeight),
		  positionMatrix(glm::mat4(0)) {
	projectionMatrix = perspectiveFov<double>(glm::radians(getFovY()), viewportWidth, viewportHeight, near, far);
	frustum = Frustum(fov, getFovY(), near, far);
}

glm::mat4 Camera::getViewMatrix() const {
	return glm::inverse(this->positionMatrix);
}

glm::mat4 Camera::getProjectionMatrix() const {
	return this->projectionMatrix;
}

void Camera::update(std::chrono::microseconds deltaTime) {
	positionMatrix = glm::mat4(getOriginTransformation() * getModelMatrix());
	frustum.recalculate(positionMatrix);
}

glm::vec3 Camera::getCameraPosition() const{
	return vec3(positionMatrix * vec4(0,0,0,1));
}

double Camera::getFovY()const {
	return fov * ((double) viewportHeight / (double) viewportWidth);
}

AABB Camera::calculateViewFrustrum() {
	return AABB(frustum.getBoundingVolume());
}
