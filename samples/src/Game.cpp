#include <Engine.hpp>
#include <components/TransformComponent.hpp>
#include <components/MeshComponent.hpp>
#include <meshes/MeshFactory.hpp>

#include "Game.hpp"

using namespace NAISE::Engine;

int main(int argc, char** argv) {
	NAISE::Engine::Engine engine;

	auto sphere = make_shared<NAISE::Engine::Entity>(1);
	//sphere->addComponent<TransformComponent>();
	//MeshFactory::createBox();
	//sphere->addComponent<MeshComponent>();

	engine.entityManager.addEntity(sphere);

	engine.run();
}