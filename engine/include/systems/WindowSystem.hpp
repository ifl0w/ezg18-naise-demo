#pragma once

#include "Event.hpp"
#include "systems/System.hpp"

#include <SDL2/SDL.h>

namespace NAISE {
namespace Engine {

namespace WindowEvents {
struct Window: public Event<SDL_Window*, SDL_GLContext> {};
struct CaptureMouse: public Event<bool> {};
struct SetResolution: public Event<uint32_t, uint32_t> {};
struct SetFullscreen: public Event<bool> {};
}

using ProcAddress = void(*)();
ProcAddress getProcAddress(const char *);

class WindowSystem: public System {
public:
	WindowSystem();
	~WindowSystem();

	void process(microseconds deltaTime) override;

	bool isFullscreen(){
		return _fullscreen;
	};

private:
	SDL_Window *window;
	SDL_GLContext context;

	bool _fullscreen = false;

	bool _firstFrame = true;

	void captureMouse(bool capture);

	void setFullscreen(bool fullscreen);

	void setResolution(uint32_t width, uint32_t height);

	void setSDLAttribute(SDL_GLattr attr, int val);
};

}
}
