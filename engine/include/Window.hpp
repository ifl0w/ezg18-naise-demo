#pragma once

#include <SDL2/SDL.h>

namespace NAISE {
namespace Engine {

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



