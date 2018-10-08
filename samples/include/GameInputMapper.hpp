#pragma once

#include <systems/input/InputMapper.hpp>

using namespace NAISE::Engine;

namespace Actions {
struct MoveForward {};
struct MoveBackward {};
struct MoveLeft {};
struct MoveRight {};
struct MouseMotion {};
struct MouseGrab {};
}

class GameInputMapper: public NAISE::Engine::InputMapper {
public:
	vector<Action> resolve(const SDL_Event& event) override;
	Input input(Action action, Input& input, const SDL_Event& event) override;
};