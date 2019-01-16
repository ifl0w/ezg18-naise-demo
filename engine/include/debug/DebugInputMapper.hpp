#pragma once

#include <systems/input/InputMapper.hpp>

namespace NAISE {
namespace Engine {
namespace Debug {


namespace Actions {

    struct RecompileShaders{};
}

class DebugInputMapper: public InputMapper {
public:
    vector<Action> resolve(const SDL_Event& event) override;
    Input input(Action action, Input& input, const SDL_Event& event) override;
    void handleEvent(const SDL_Event& event) override;
};
}
}
}