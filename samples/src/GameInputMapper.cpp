#include "GameInputMapper.hpp"

vector<Action> GameInputMapper::resolve(const SDL_Event& event) {

	switch (event.type) {
	case SDL_KEYUP:
	case SDL_KEYDOWN:
	{
		switch (event.key.keysym.sym) {
		case SDLK_w:
			return { make_action<Actions::MoveForward>() };
		case SDLK_s:
			return { make_action<Actions::MoveBackward>() };
		case SDLK_a:
			return { make_action<Actions::MoveLeft>() };
		case SDLK_d:
			return { make_action<Actions::MoveRight>() };
		default:
			break;
		}
	}
	case SDL_MOUSEMOTION:
		return {  make_action<Actions::MouseMotion>()  };
	default:
		break;
	}

	return {};
}

Input GameInputMapper::input(Action action, const SDL_Event& event) {
	Input inp;

	if (action == make_action<Actions::MouseMotion>()) {
		inp.values["delta_x"] = event.motion.xrel;
		inp.values["delta_y"] = event.motion.yrel;
		inp.values["pos_x"] = event.motion.x;
		inp.values["pos_y"] = event.motion.y;
		inp.values["active"] = true;
	}

	if (action == make_action<Actions::MoveForward>()
		|| action == make_action<Actions::MoveBackward>()
		|| action == make_action<Actions::MoveLeft>()
		|| action == make_action<Actions::MoveRight>()) {
		inp.values["active"] = event.type == SDL_KEYDOWN;
	}

	return inp;
}
