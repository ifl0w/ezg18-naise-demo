#pragma once

#include <nlohmann/json.hpp>

#include "systems/System.hpp"

using CameraController = std::pair<EntityID, EntityID>;
using CameraSequence = std::map<int, CameraController>;

class CameraSelectionSystem: public NAISE::Engine::System {
public:
	CameraSelectionSystem(CameraSequence cameraSequence);

	virtual void process(microseconds deltaTime);

private:

	bool demoPaused = false;

	bool debugCameraActive = false;
	std::shared_ptr<Entity> debugCamera;

	CameraSequence _cameraSequence;

	std::pair<int, CameraController> _selectedCamera;
};
