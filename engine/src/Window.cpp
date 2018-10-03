#include <Window.hpp>

#include <stdexcept>
#include <spdlog/spdlog.h>

#include <glbinding/glbinding.h>
#include <glbinding/Binding.h>
#include <glbinding-aux/debug.h>
#include <iostream>

using namespace NAISE::Engine;
using namespace std;

Window::Window() {
	SDL_Init(SDL_INIT_VIDEO);              	// Initialize SDL2

	// Create an application window with the following settings:
	window = SDL_CreateWindow(
			"An SDL2 window",                  // window title
			SDL_WINDOWPOS_UNDEFINED,           // initial x position
			SDL_WINDOWPOS_UNDEFINED,           // initial y position
			1024,                              // width, in pixels
			768,                               // height, in pixels
			SDL_WINDOW_OPENGL                  // flags - see below
	);

	// Check that the window was successfully created
	if (window == nullptr) {
		// In the case that the window could not be made...
		throw runtime_error(string("Could not create window: ").append(SDL_GetError()));
	}

	int setAttributeRet = SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	setAttributeRet = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	setAttributeRet = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	setAttributeRet = SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	context = SDL_GL_CreateContext(window);
	// Check that the window was successfully created
	if (window == nullptr) {
		// In the case that the window could not be made...
		throw runtime_error(string("Could not create OpenGL context: ").append(SDL_GetError()));
	}

	glbinding::initialize(getProcAddress, false);
	glbinding::aux::enableGetErrorCallback();
}

Window::~Window() {
	// Close and destroy the window
	SDL_DestroyWindow(window);

	// Clean up
	SDL_Quit();
}

void Window::handleEvent(SDL_Event& event) {
	if (event.type == SDL_WINDOWEVENT) {
		switch (event.window.event) {
		case SDL_WINDOWEVENT_CLOSE:
			running = false;
			break;
		default:
			// swallow unhandled events
			break;
		}
	}
}

void Window::captureMouse(bool capture) {
	SDL_RaiseWindow(window);

	int ret = SDL_SetRelativeMouseMode((SDL_bool) capture);
	if (ret != 0) {
		throw runtime_error(string("Could not capture mouse: ").append(SDL_GetError()));
	}
}

void Window::setFullscreen(bool) {
	int ret = SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	if (ret != 0) {
		throw runtime_error(string("Could not start fullscreen mode: ").append(SDL_GetError()));
	}
}

void Window::setResolution(uint32_t width,uint32_t  height) {
	SDL_SetWindowSize(window, width, height);
}

ProcAddress NAISE::Engine::getProcAddress(const char* name) {
	return (ProcAddress) SDL_GL_GetProcAddress(name);
}
