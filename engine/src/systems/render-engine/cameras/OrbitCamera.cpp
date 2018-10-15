#include "OrbitCamera.h"

#include "glm/gtx/rotate_vector.hpp"

OrbitCamera::OrbitCamera(double fov, double near, double far, int viewportWidth, int viewportHeight)
		: Camera(fov, near, far, viewportWidth, viewportHeight),
		  radius(6),
		  yaw(0),
		  pitch(0) {

}

void OrbitCamera::initScene(btDynamicsWorld* dynamicsWorld) {
	calculatePosition();
}

void OrbitCamera::update(std::chrono::microseconds deltaTime) {
	Camera::update(deltaTime);
	setModelMatrix(getModelMatrix());
}

void OrbitCamera::calculatePosition() {
	double azimuthRad = yaw - glm::pi<float>();
	double polarRad = pitch - glm::half_pi<float>();

	double x = radius * glm::sin(polarRad) * glm::cos(azimuthRad);
	double y = radius * glm::sin(polarRad) * glm::sin(azimuthRad);
	double z = radius * glm::cos(polarRad);

	cameraPosition =
			position + glm::vec3((float) y, (float) z, (float) x); // swapped coordinates to represent correct arc ball

	auto tmp = glm::translate(glm::mat4(1), cameraPosition);

	glm::mat4 cameraCoordSystem = getCameraCoordSystem();

	setModelMatrix(tmp * cameraCoordSystem); // rotate and translate
}

glm::mat4x4 OrbitCamera::getCameraCoordSystem() {
	glm::vec3 camToCenter = position - cameraPosition;

	// calculate axis
	glm::vec3 forward = glm::normalize(camToCenter);
	glm::vec3 right = glm::cross(forward, glm::vec3(0, 1, 0));
	right = glm::normalize(right);
	glm::vec3 up = glm::cross(right, forward);

	// coordinate system matrix
	glm::tmat4x4<float> cameraCoordSystem = glm::mat4x4(glm::vec4(right, 0), glm::vec4(up, 0), glm::vec4(-forward, 0),
														glm::vec4(0, 0, 0, 1));

	return cameraCoordSystem;
}

void OrbitCamera::inputEvent(sf::Event event) {
	switch (event.type) {
	case sf::Event::MouseButtonPressed:
		onMouseButtonPressed(event.mouseButton);
		break;
	case sf::Event::MouseButtonReleased:
		onMouseButtonReleased(event.mouseButton);
		break;
	case sf::Event::MouseMoved:
		onMousePositionEvent(event.mouseMove);
		break;
	case sf::Event::MouseWheelScrolled:
		onScrollEvent(event.mouseWheelScroll);
		break;
	case sf::Event::KeyPressed:
		onKeyPressed(event.key);
		break;
	default:
		break;
	}
}

void OrbitCamera::onMousePositionEvent(sf::Event::MouseMoveEvent event) {
	double xDiff = lastMouseX - event.x;
	double yDiff = lastMouseY - event.y;

	if (dragging) {
		yaw += glm::radians(xDiff * mouseSpeed);
		pitch -= glm::radians(yDiff * mouseSpeed);

		double halfPi = glm::half_pi<float>() - 0.001;
		pitch = glm::clamp(pitch, -halfPi, halfPi);

		calculatePosition();
	}

	if (rightMousePressed) {
		glm::vec3 offset = glm::vec3(xDiff * mouseSpeed / 100, -yDiff * mouseSpeed / 100, 0);
		glm::vec3 cameraRelativeOffset = glm::vec3(getCameraCoordSystem() * glm::vec4(offset, 1));

		position += cameraRelativeOffset;
		calculatePosition();
	}

	lastMouseX = event.x;
	lastMouseY = event.y;
}

void OrbitCamera::onMouseButtonPressed(sf::Event::MouseButtonEvent event) {
	switch (event.button) {
	case sf::Mouse::Button::Left:
		dragging = true;
		break;
	case sf::Mouse::Button::Right:
		rightMousePressed = true;
		break;
	default:
		break;
	}
}

void OrbitCamera::onMouseButtonReleased(sf::Event::MouseButtonEvent event) {
	switch (event.button) {
	case sf::Mouse::Button::Left:
		dragging = false;
		break;
	case sf::Mouse::Button::Right:
		rightMousePressed = false;
		break;
	default:
		break;
	}
}

void OrbitCamera::onScrollEvent(sf::Event::MouseWheelScrollEvent event) {
	radius += event.delta * zoomSpeed / 10;
	if (radius <= 0) { radius = 0.001; }

	calculatePosition();
}

void OrbitCamera::onKeyPressed(sf::Event::KeyEvent event) {
	double halfPi = glm::half_pi<float>() - 0.001;
	switch (event.code) {
	case sf::Keyboard::Numpad4:
		yaw -= glm::radians(1.0);
		calculatePosition();
		break;
	case sf::Keyboard::Numpad6:
		yaw += glm::radians(1.0);
		calculatePosition();
		break;
	case sf::Keyboard::Numpad8:
		pitch += glm::radians(5.0);
		pitch = glm::clamp(pitch, -halfPi, halfPi);
		calculatePosition();
		break;
	case sf::Keyboard::Numpad2:
		pitch -= glm::radians(5.0);
		pitch = glm::clamp(pitch, -halfPi, halfPi);
		calculatePosition();
		break;
	default:
		break;
	}
}
