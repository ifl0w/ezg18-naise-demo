#pragma once

#include <systems/input/InputMapper.hpp>

using namespace NAISE::Engine;

namespace Actions {
struct MoveForward {};
struct MoveBackward {};
struct MoveLeft {};
struct MoveRight {};
struct Sprint {};
struct MouseMotion {};
struct MouseGrab {};
struct SetFullscreen {};
}

class FPSCameraInputMapper: public NAISE::Engine::InputMapper {
public:
	vector<Action> resolve(const SDL_Event& event) override;
	Input input(Action action, Input& input, const SDL_Event& event) override;
	void handleEvent(const SDL_Event& event) override;
};