#pragma once

#include "Camera.hpp"

namespace NAISE {
namespace Engine {

class OrbitCamera
		: public Camera {
public:
	OrbitCamera()
			: Camera(),
			  radius(6),
			  yaw(0),
			  pitch(0) { };
	OrbitCamera(double fov, double near, double far, int viewportWidth, int viewportHeight);

	double radius;
	double pitch;
	double yaw;

	double mouseSpeed = 1;
	double zoomSpeed = 4;

	void initScene(btDynamicsWorld* dynamicsWorld) override;
	void update(std::chrono::microseconds deltaTime) override;

	void inputEvent(sf::Event event) override;

	void onMouseButtonPressed(sf::Event::MouseButtonEvent event);
	void onMouseButtonReleased(sf::Event::MouseButtonEvent event);
	void onMousePositionEvent(sf::Event::MouseMoveEvent event);
	void onScrollEvent(sf::Event::MouseWheelScrollEvent event);
	void onKeyPressed(sf::Event::KeyEvent event);

private:
	bool dragging = false;
	bool rightMousePressed = false;
	double lastMouseX = 0;
	double lastMouseY = 0;

	glm::mat4x4 getCameraCoordSystem();

	void calculatePosition();
};

}
}