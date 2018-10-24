#include <systems/WindowSystem.hpp>

#include <string>

#include <glbinding/glbinding.h>
#include <glbinding-aux/debug.h>
#include <glbinding-aux/ContextInfo.h>

#include <Engine.hpp>

using namespace NAISE::Engine;
using namespace std;

WindowSystem::WindowSystem() {
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

	setSDLAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	setSDLAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	setSDLAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	setSDLAttribute(SDL_GL_DOUBLEBUFFER, 1);

	context = SDL_GL_CreateContext(window);
	// Check that the window was successfully created
	if (context == nullptr) {
		// In the case that the window could not be made...
		throw runtime_error(string("Could not create OpenGL context: ").append(SDL_GetError()));
	}

	SDL_GL_SetSwapInterval(0);

	glbinding::initialize(getProcAddress, false);
#ifdef DEBUG
	glbinding::aux::enableGetErrorCallback();

	spdlog::get("console")->info("OpenGL Version: {}", glbinding::aux::ContextInfo::version().toString());
	spdlog::get("console")->info("OpenGL Vendor: {}", glbinding::aux::ContextInfo::vendor());
	spdlog::get("console")->info("OpenGL Renderer: {}", glbinding::aux::ContextInfo::renderer());
#endif

	Engine::getEventManager().event<WindowEvents::CaptureMouse>().subscribe([&](bool capture){
	  captureMouse(capture);
	});

	Engine::getEventManager().event<WindowEvents::SetFullscreen>().subscribe([&](bool fullscreen){
	  setFullscreen(fullscreen);
	});

	Engine::getEventManager().event<WindowEvents::SetResolution>().subscribe([&](uint32_t width, uint32_t height){
	  setResolution(width, height);
	});
}

ProcAddress NAISE::Engine::getProcAddress(const char* name) {
	return (ProcAddress) SDL_GL_GetProcAddress(name);
}

WindowSystem::~WindowSystem() {
	// Close and destroy the window
	SDL_DestroyWindow(window);

	// Clean up
	SDL_Quit();
}



void WindowSystem::captureMouse(bool capture) {
	SDL_RaiseWindow(window);

	int ret = SDL_SetRelativeMouseMode((SDL_bool) capture);
	if (ret != 0) {
		throw runtime_error(string("Could not capture mouse: ").append(SDL_GetError()));
	}
}

void WindowSystem::setFullscreen(bool fullscreen) {
	int ret;
	if(_fullscreen){
		ret = SDL_SetWindowFullscreen(window, 0);
	} else {
		ret = SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	}

	if (ret != 0) {
		throw runtime_error(string("Could not start fullscreen mode: ").append(SDL_GetError()));
	}
	_fullscreen = fullscreen;
}

void WindowSystem::setResolution(uint32_t width,uint32_t  height) {
	SDL_SetWindowSize(window, width, height);
}

void WindowSystem::setSDLAttribute(SDL_GLattr attr, int val) {
	int setAttributeRet = SDL_GL_SetAttribute(attr, val);

	if (setAttributeRet != 0) {
		// In the case that the window could not be made...
		spdlog::get("logger")->error("SDL_GL_SetAttribute error! Attribute: {}, Value: {}", attr, val);
		throw runtime_error((SDL_GetError()));
	}
}
