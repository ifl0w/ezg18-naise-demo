#pragma once

#include <nlohmann/json.hpp>

#include "systems/System.hpp"

class CameraSelectionSystem: public NAISE::Engine::System {
public:
	CameraSelectionSystem(nlohmann::json cameraConfig);

	virtual void process(microseconds deltaTime);

private:
	std::chrono::duration<float> accumTime;
};
