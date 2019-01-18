#pragma once

#include <systems/input/InputMapper.hpp>

#include <systems/debugging/DebugSystem.hpp>

class DebugInputMapper: public NAISE::Engine::InputMapper {
public:
	void handleEvent(const SDL_Event& event) override;
};
