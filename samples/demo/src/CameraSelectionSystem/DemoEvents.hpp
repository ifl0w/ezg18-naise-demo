#pragma once

#include "systems/Event.hpp"

namespace CameraEvents {
struct ToggleDebugCamera: public NAISE::Engine::Event<> {};
struct RestartDemo: public NAISE::Engine::Event<> {};
struct PauseDemo: public NAISE::Engine::Event<> {};
}