#include <systems/physics/BulletDebugDrawer.hpp>
#include <systems/render-engine/materials/SolidColorMaterial.hpp>
#include <components/TransformComponent.hpp>
#include <Engine.hpp>

using namespace NAISE::Engine;

BulletDebugDrawer::BulletDebugDrawer()
		: m_debugMode(0) {
}

void BulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
	auto c = glm::vec3(color.getX(), color.getY(), color.getZ());

	shared_ptr<Entity> entity;
	for (const auto& e: debugEntities) {
		if (e->component<PhysicsDebugComponent>().color == c) {
			entity = e;
			break;
		}
	}

	if (!entity) {
		entity = make_shared<Entity>();
		debugEntities.push_back(entity);
		entity->add<PhysicsDebugComponent>();
		entity->component<PhysicsDebugComponent>().color = c;
		Engine::getEntityManager().addEntity(entity);
	}

	entity->component<PhysicsDebugComponent>().mesh.addLine(glm::vec3(from.x(), from.y(), from.z()), glm::vec3(to.x(), to.y(), to.z()));
}

void BulletDebugDrawer::setDebugMode(int debugMode) {
	m_debugMode = debugMode;
}

void BulletDebugDrawer::draw3dText(const btVector3& location, const char* textString) {
	spdlog::get("logger")->warn("NAISE::ENGINE::BulletDebugDrawer::draw3dText(...) not implemented");
}

void BulletDebugDrawer::reportErrorWarning(const char* warningString) {
	spdlog::get("logger")->warn("NAISE::ENGINE::BulletDebugDrawer :: {}", warningString);
}

void BulletDebugDrawer::finishMeshes() {
	for (auto& e: debugEntities) {
		e->component<PhysicsDebugComponent>().mesh.finishDebugMesh();
	}
}

void BulletDebugDrawer::beginDebugFrame() {
	for (auto& e: debugEntities) {
		e->component<PhysicsDebugComponent>().mesh.startDebugFrame();
	}
}

void BulletDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance,
										 int lifeTime, const btVector3& color) {
	spdlog::get("logger")->warn("NAISE::ENGINE::BulletDebugDrawer::drawContactPoint(...) not implemented");
}

