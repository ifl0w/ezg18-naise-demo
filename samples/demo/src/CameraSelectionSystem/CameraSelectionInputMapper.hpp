#pragma once

#include <Engine.hpp>

class CameraSelectionInputMapper: public NAISE::Engine::InputMapper {
public:
	void handleEvent(const SDL_Event& event) override;
};


