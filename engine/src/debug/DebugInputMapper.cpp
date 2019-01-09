#include "debug/DebugInputMapper.hpp"
#include "systems/RenderSystem.hpp"
#include "Engine.hpp"

vector<Action> DebugInputMapper::resolve(const SDL_Event& event) {

    switch (event.type) {
        case SDL_KEYUP:
        case SDL_KEYDOWN: {
            switch (event.key.keysym.sym) {
                case SDLK_1:
                    return {make_action<Actions::RecompileShaders>()};
                default:
                    break;
            }

            break;
        }
        default:
            break;
    }

    return {};
}

Input DebugInputMapper::input(Action action, Input& inp, const SDL_Event& event) {

    return inp;
}

void DebugInputMapper::handleEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_KEYUP: {
            switch (event.key.keysym.sym) {
                case SDLK_1:
                    Engine::getEventManager().event<RenderEvents::ReloadShaders>().emit();
                    break;
            }
        }
    }

}
