#include "GameInputMapper.hpp"

vector<Action> GameInputMapper::resolve(const SDL_Event& event) {

	switch (event.type) {
	case SDL_KEYUP:
	case SDL_KEYDOWN: {
		switch (event.key.keysym.sym) {
		case SDLK_w:
			return {make_action<Actions::MoveForward>()};
		case SDLK_s:
			return {make_action<Actions::MoveBackward>()};
		case SDLK_a:
			return {make_action<Actions::MoveLeft>()};
		case SDLK_d:
			return {make_action<Actions::MoveRight>()};
		default:
			break;
		}

		break;
	}
	case SDL_MOUSEMOTION: {
		return {make_action<Actions::MouseMotion>()};
	}
	case SDL_MOUSEBUTTONDOWN: {
		return {make_action<Actions::MouseGrab>()};
	}
	case SDL_MOUSEBUTTONUP: {
		return {make_action<Actions::MouseGrab>()};
	}
	default:
		break;
	}

	return {};
}

Input GameInputMapper::input(Action action, Input& inp, const SDL_Event& event) {

	if (is_action<Actions::MouseGrab>(action)) {
		inp.set("active", !inp.get<bool>("active"));
	}

	if (is_action<Actions::MouseMotion>(action)) {
		inp.set("delta_x", inp.get<int>("delta_x") + event.motion.xrel);
		inp.set("delta_y", inp.get<int>("delta_y") + event.motion.yrel);
		inp.set("pos_x", event.motion.x);
		inp.set("pos_y", event.motion.y);
		inp.set("active", true);
	}

	if (is_action<Actions::MoveForward>(action)
			|| is_action<Actions::MoveBackward>(action)
			|| is_action<Actions::MoveLeft>(action)
			|| is_action<Actions::MoveRight>(action)) {
		inp.set("active", event.type == SDL_KEYDOWN);
	}

	return inp;
}
