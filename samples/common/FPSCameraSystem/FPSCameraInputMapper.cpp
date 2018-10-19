#include "FPSCameraInputMapper.hpp"
#include <systems/WindowSystem.hpp>
#include <Engine.hpp>

vector<Action> FPSCameraInputMapper::resolve(const SDL_Event& event) {

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
		case SDLK_LSHIFT:
			return {make_action<Actions::Sprint>()};
		case SDLK_F11:
			return {make_action<Actions::SetFullscreen>()};
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

Input FPSCameraInputMapper::input(Action action, Input& inp, const SDL_Event& event) {

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

	if (is_action<Actions::Sprint>(action)) {
		inp.set("active", event.type == SDL_KEYDOWN);
	}

	if (is_action<Actions::SetFullscreen>(action)) {
		inp.set("fullscreen", event.type == SDL_KEYDOWN);
	}

	return inp;
}

void FPSCameraInputMapper::handleEvent(const SDL_Event& event) {
	switch (event.type) {
	case SDL_KEYUP: {
		switch (event.key.keysym.sym) {
		case SDLK_F11:
		{
			auto state = Engine::getSystemsManager().getSystem<WindowSystem>().isFullscreen();
			Engine::getEventManager().event<WindowEvents::SetFullscreen>().emit(!state);
			break;
		}
		case SDLK_ESCAPE:
			Engine::getEventManager().event<RuntimeEvents::Quit>().emit();
			break;
		}

		break;
	}
	}

}
