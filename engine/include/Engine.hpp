#pragma once


#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <systems/System.hpp>
#include <systems/input/InputSystem.hpp>

#include <systems/EventManager.hpp>
#include <scene/EntityManager.hpp>

using namespace std;

namespace NAISE {
namespace Engine {

namespace RuntimeEvents {
struct Quit: public Event<> {};
struct EntityAdded: public Event<EntityID> {};
struct EntityRemoved: public Event<EntityID> {};
struct EntityModified: public Event<EntityID> {};
}

class Engine {
public:
	Engine();
	~Engine();

	static void initialize();
	static void shutdown();

	void run();

	static EventManager& getEventManager();
	static EntityManager& getEntityManager();
	static SystemsManager& getSystemsManager();

private:
	static EventManager eventManager;
	static EntityManager entityManager;
	static SystemsManager systemsManager;

	microseconds _deltaTime = microseconds(0);
	steady_clock::time_point _lastFrame;
	uint32_t _fps;

	bool quit = false;
};

}
}