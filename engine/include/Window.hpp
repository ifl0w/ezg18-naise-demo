#pragma once

#include <SDL2/SDL.h>
#include <glbinding/ProcAddress.h>

namespace NAISE {
namespace Engine {

using ProcAddress = void(*)();
ProcAddress getProcAddress(const char *);

class Window {
public:
	Window();
	~Window();

	SDL_Window *window;
	SDL_GLContext context;

	bool running = true;

	void handleEvent(SDL_Event& event);
};

}
}



